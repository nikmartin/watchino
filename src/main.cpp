#include "Arduino_GFX.h"
#include "HWCDC.h"
#include "pin_config.h"
#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <NimBLEDevice.h>
#include <SensorPCF85063.hpp> //RTC
#include <SensorQMI8658.hpp>  //IMU
#include <Wire.h>
#include <lvgl.h>
#include <ArduinoJson.h>
#include <Arduino_DriveBus_Library.h>

// --custom lvgl font from Google Fonts--
LV_FONT_DECLARE(tilt_neon_48_4bpp);
LV_FONT_DECLARE(michroma_48_4bpp);
LV_FONT_DECLARE(seven_segment_48_4bpp);

// for stringifying compiler defines
#define WATCHINO_STRINGIFY_VALUE(value) #value
#define WATCHINO_STRINGIFY(value) WATCHINO_STRINGIFY_VALUE(value)

// IMU parameter and config
SensorQMI8658 qmi;
uint32_t step_counter = 0;

HWCDC USBSerial;
SensorPCF85063 rtc; // SensorLib RTC driver instance

// DriveBus Touch Driver instance
std::shared_ptr<Arduino_IIC_DriveBus> i2c_bus;
Arduino_FT3x68 *touch_chip = nullptr;
SemaphoreHandle_t i2c_mutex = NULL;

// Power and Timeout State Management
bool screen_on = true;
unsigned long last_activity_time = 0;
const unsigned long SCREEN_TIMEOUT_MS = 15000;

#define BYTE_PER_PIXEL (LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_RGB565))
#define BUF_SIZE (LCD_WIDTH * 80)

// LVGL UI Objects
lv_obj_t *time_label;
lv_obj_t *step_label;

// BLE Configuration
#define SERVICE_UUID "6e400001-b5a3-f393-e0a9-e50e24dcca9e"           // NUS Service
#define RX_CHARACTERISTIC_UUID "6e400002-b5a3-f393-e0a9-e50e24dcca9e" // RX (Write)
#define TX_CHARACTERISTIC_UUID "6e400003-b5a3-f393-e0a9-e50e24dcca9e" // TX (Notify)
#define DEVICE_NAME "Bangle.js C6"

NimBLECharacteristic *pTxCharacteristic;
bool deviceConnected = false;
String gadgetbridge_buffer;

// Variables to hold parsed time
uint16_t current_year = 2026;
uint8_t current_month = 1;
uint8_t current_date = 1;
uint8_t current_hour = 12;
uint8_t current_minute = 0;
uint8_t current_second = 0;

bool time_updated = false;

// Graphics bus and display initialization
Arduino_DataBus *bus = new Arduino_ESP32QSPI(
    LCD_CS /* CS */, LCD_SCLK /* SCK */, LCD_SDIO0 /* SDIO0 */,
    LCD_SDIO1 /* SDIO1 */, LCD_SDIO2 /* SDIO2 */, LCD_SDIO3 /* SDIO3 */);

Arduino_GFX *gfx =
    new Arduino_CO5300(bus, LCD_RESET /* RST */, 0 /* rotation */, LCD_WIDTH,
                       LCD_HEIGHT, 22 /* col_offset1 */, 0 /* row_offset1 */,
                       0 /* col_offset2 */, 0 /* row_offset2 */);

// Init Hardware RTC via SensorLib ---
void init_hardware_rtc()
{

  // SensorLib initialization pattern
  if (!rtc.begin(Wire, IIC_SDA, IIC_SCL))
  {
    USBSerial.println("Error: SensorLib could not find PCF85063 chip!");
    return;
  }
  // Start the clock internal oscillator circuit
  rtc.start();
  USBSerial.println("SensorLib RTC Initialized.");
}

void update_clock_ui_cb(lv_timer_t *timer)
{
  if (!time_label)
    return;

  RTC_DateTime now;
  if (i2c_mutex && xSemaphoreTake(i2c_mutex, pdMS_TO_TICKS(10)) == pdTRUE)
  {
    now = rtc.getDateTime();
    xSemaphoreGive(i2c_mutex);
  }
  else
  {
    return;
  }

  char time_str[12];
  // Formats text cleanly to show Hours:Minutes:Seconds (e.g., 14:05:32)
  snprintf(time_str, sizeof(time_str), "%02d:%02d:%02d", now.getHour(),
           now.getMinute(), now.getSecond());

  lv_label_set_text(time_label, time_str);
}
// --- GADGETBRIDGE PARSER ---
void parse_gadgetbridge_json(const String &json_text)
{
  JsonDocument document;
  USBSerial.print("Parsing Gadgetbridge JSON: ");
  USBSerial.println(json_text);

  DeserializationError error = deserializeJson(document, json_text);
  if (error)
  {
    USBSerial.print("Gadgetbridge JSON parse failed: ");
    USBSerial.println(error.c_str());
    return;
  }

  USBSerial.print("Parsed Gadgetbridge JSON: ");
  serializeJson(document, USBSerial);
  USBSerial.println();

  const char *command = document["cmd"] | document["command"] | document["t"] | "";
  JsonVariantConst timestamp_value = document["timestamp"];
  if (timestamp_value.isNull())
    timestamp_value = document["time"];

  USBSerial.print("Gadgetbridge command: ");
  USBSerial.println(command);

  if (strcmp(command, "setTime") != 0 && strcmp(command, "time") != 0)
    return;

  time_t epoch = timestamp_value | 0;
  if (epoch <= 0)
  {
    USBSerial.println("Gadgetbridge JSON has no valid timestamp");
    return;
  }

  struct tm *timeinfo = gmtime(&epoch);
  if (!timeinfo)
  {
    USBSerial.println("Unable to convert Gadgetbridge timestamp");
    return;
  }

  current_year = timeinfo->tm_year + 1900;
  current_month = timeinfo->tm_mon + 1;
  current_date = timeinfo->tm_mday;
  current_hour = timeinfo->tm_hour;
  current_minute = timeinfo->tm_min;
  current_second = timeinfo->tm_sec;
  rtc.setDateTime(current_year, current_month, current_date, current_hour,
                  current_minute, current_second);
  time_updated = true;
  USBSerial.printf("SensorLib RTC synced to epoch: %lld\n", epoch);
}

void process_gadgetbridge_bytes(const std::string &fragment)
{
  for (unsigned char byte : fragment)
    gadgetbridge_buffer += static_cast<char>(byte);

  while (true)
  {
    int start = gadgetbridge_buffer.indexOf("GB(");
    if (start < 0)
    {
      gadgetbridge_buffer = "";
      return;
    }

    if (start > 0)
      gadgetbridge_buffer.remove(0, start);

    int depth = 0;
    bool inside_string = false;
    bool escaped = false;
    int closing_index = -1;

    for (int index = 3; index < gadgetbridge_buffer.length(); ++index)
    {
      char character = gadgetbridge_buffer[index];
      if (escaped)
      {
        escaped = false;
        continue;
      }
      if (character == '\\' && inside_string)
      {
        escaped = true;
        continue;
      }
      if (character == '"')
      {
        inside_string = !inside_string;
        continue;
      }
      if (inside_string)
        continue;

      if (character == '{')
        ++depth;
      else if (character == '}' && --depth == 0)
      {
        if (index + 1 < gadgetbridge_buffer.length() &&
            gadgetbridge_buffer[index + 1] == ')')
        {
          closing_index = index;
          break;
        }
      }
    }

    if (closing_index < 0)
      return;

    String json_text = gadgetbridge_buffer.substring(3, closing_index + 1);
    parse_gadgetbridge_json(json_text);
    gadgetbridge_buffer.remove(0, closing_index + 2);
  }
}
// BLE Callback Class
class MyServerCallbacks : public NimBLEServerCallbacks
{
  void onConnect(NimBLEServer *pServer, NimBLEConnInfo &connInfo) override
  {
    deviceConnected = true;
    USBSerial.println("Gadgetbridge connected!");
  }

  void onDisconnect(NimBLEServer *pServer, NimBLEConnInfo &connInfo,
                    int reason) override
  {
    deviceConnected = false;
    USBSerial.println("Gadgetbridge disconnected!");
    NimBLEDevice::startAdvertising();
  }
};

class MyCharacteristicCallbacks : public NimBLECharacteristicCallbacks
{
  void onWrite(NimBLECharacteristic *pCharacteristic,
               NimBLEConnInfo &connInfo) override
  {
    // USBSerial.println("onWrite callback entered");
    std::string rxValue = pCharacteristic->getValue();
    // USBSerial.print("Write received on UUID: ");
    // USBSerial.println(pCharacteristic->getUUID().toString().c_str());

    process_gadgetbridge_bytes(rxValue);
  }
};

bool detectStep(const IMUdata data)
{
  static float gravity_magnitude = 1.0f;
  static float last_dynamic_accel = 0.0f;
  static unsigned long last_step_time = 0;
  const unsigned long minimum_step_interval = 300;
  const float movement_threshold = 0.22f;

  float accel_magnitude = sqrt(data.x * data.x + data.y * data.y + data.z * data.z);
  gravity_magnitude = (gravity_magnitude * 0.95f) + (accel_magnitude * 0.05f);
  float dynamic_accel = accel_magnitude - gravity_magnitude;

  // Print the acceleration magnitude for debugging
  // USBSerial.printf("Magnitude: %.2f Dynamic: %.2f Z: %.2f\n",
  //                  accel_magnitude, dynamic_accel, data.z);

  if (dynamic_accel > movement_threshold &&
      last_dynamic_accel <= movement_threshold &&
      millis() - last_step_time >= minimum_step_interval)
  {
    last_dynamic_accel = dynamic_accel;
    last_step_time = millis();
    return true; // Step detected
  }

  last_dynamic_accel = dynamic_accel;
  return false; // No step detected
}
void init_step_counter()
{
  // Initialize and configure the QMI8658 through SensorLib.
  if (!qmi.begin(Wire, QMI8658_L_SLAVE_ADDRESS, IIC_SDA, IIC_SCL))
  {
    USBSerial.println("Failed to find QMI8658 - check your wiring!");
    while (true)
    {
      delay(1000);
    }
  }
  Wire.setClock(400000);

  USBSerial.print("QMI8658 Chip ID: 0x");
  USBSerial.println(qmi.getChipID(), HEX);

  qmi.configAccelerometer(SensorQMI8658::ACC_RANGE_2G,
                          SensorQMI8658::ACC_ODR_62_5Hz,
                          SensorQMI8658::LPF_MODE_0);

  qmi.enableAccelerometer();
  USBSerial.println("Hardware Accelerometer engine enabled");
}
// --- LVGL UI INITIALIZATION ---
void create_clock_ui()
{

  // get the screen
  lv_obj_t *screen = lv_screen_active();

  // setup the BG color
  lv_obj_set_style_bg_color(screen, lv_color_hex(0x000000), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, LV_PART_MAIN);

  // Create a label in the absolute middle of 410x502 screen
  time_label = lv_label_create(screen);
  lv_obj_align(time_label, LV_ALIGN_CENTER, 0, 0);

  // Styling for a retro aesthetic
  lv_color_t tomato_color = lv_color_hex(RGB16TO24(RGB565_TOMATO));
  lv_obj_set_style_text_color(time_label, tomato_color, LV_PART_MAIN);

  // Set a custom font
  lv_obj_set_style_text_font(time_label, &michroma_48_4bpp, LV_PART_MAIN);
  // set some dummy text
  lv_label_set_text(time_label, "12:00:00");

  step_label = lv_label_create(screen);
  lv_obj_align(step_label, LV_ALIGN_BOTTOM_MID, 0, -12);
  lv_obj_set_style_text_color(step_label, lv_color_hex(0xD0D4D0), LV_PART_MAIN);
  lv_obj_set_style_text_font(step_label, &lv_font_montserrat_28, LV_PART_MAIN);
  lv_label_set_text(step_label, "Steps: 0");
}

// LVGL v9 Rounder Callback for CO5300 Display Alignment
void my_rounder_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_INVALIDATE_AREA) {
        // Retrieve the area being updated
        lv_area_t * area = (lv_area_t *)lv_event_get_param(e);
        if (area) {
            area->x1 = area->x1 & (~0x1); /* Round down to even */
            area->y1 = area->y1 & (~0x1); /* Round down to even */
            area->x2 = area->x2 | 0x1;    /* Round up to odd */
            area->y2 = area->y2 | 0x1;    /* Round up to odd */
        }
    }
}

// LVGL 9.5 Display Flush Callback using Arduino_GFX draw API

void my_disp_flush(lv_display_t *display, const lv_area_t *area,
                   uint8_t *px_map)
{
  int32_t w = (area->x2 - area->x1 + 1);
  int32_t h = (area->y2 - area->y1 + 1);


  // Push the native LVGL 16-bit RGB pixels via standard Arduino_GFX DMA burst
  gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)px_map, w, h);

  // Advise the LVGL UI thread that processing has completed
  lv_display_flush_ready(display);
}

void init_bt_gadgetbridge()
{
  // Initialize BLE
  NimBLEDevice::init(DEVICE_NAME); // Name your device Bangle.js to trigger
                                   // the Gadgetbridge profile
  NimBLEServer *pServer = NimBLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  NimBLEService *pService = pServer->createService(SERVICE_UUID);
  pTxCharacteristic = pService->createCharacteristic(TX_CHARACTERISTIC_UUID,
                                                     NIMBLE_PROPERTY::NOTIFY);

  NimBLECharacteristic *pRxCharacteristic = pService->createCharacteristic(
      RX_CHARACTERISTIC_UUID,
      NIMBLE_PROPERTY::WRITE); // | NIMBLE_PROPERTY::WRITE_NR
  pRxCharacteristic->setCallbacks(new MyCharacteristicCallbacks());
  USBSerial.println(pService->getUUID().toString().c_str());
  USBSerial.println(pRxCharacteristic->getUUID().toString().c_str());
  USBSerial.println(pTxCharacteristic->getUUID().toString().c_str());

  if (!pServer->start())
  {
    USBSerial.println("ERROR: GATT server failed to start");
  }

  NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setName(DEVICE_NAME);
  pAdvertising->enableScanResponse(true);
  if (!pAdvertising->start())
  {
    USBSerial.println("ERROR: BLE advertising failed to start");
  }

  USBSerial.println("BLE Watch Ready for Gadgetbridge Pairing...");
}

void setup()
{
  USBSerial.begin(115200);
  USBSerial.setDebugOutput(true);
  while (!USBSerial)
    ;
  USBSerial.print("Watchino Arduino Smart Watch v");
  USBSerial.println(WATCHINO_STRINGIFY(APP_VERSION));

#ifdef GFX_EXTRA_PRE_INIT
  GFX_EXTRA_PRE_INIT();
#endif

  // Init Display
  if (!gfx->begin())
  {
    USBSerial.println("gfx->begin() failed!");
  }
  gfx->fillScreen(RGB565_BLACK);
  // Initialize core UI layout
  lv_init();
  lv_tick_set_cb([]() -> uint32_t
                 { return millis(); });

  // Allocate frame buffers (Dual buffering addresses QSPI tearing/flickering)
  static uint16_t buf1[BUF_SIZE * BYTE_PER_PIXEL];
  static uint16_t buf2[BUF_SIZE * BYTE_PER_PIXEL];

  // Target API setup for LVGL 9.5 display rendering engine
  lv_display_t *disp = lv_display_create(LCD_WIDTH, LCD_HEIGHT);

  lv_display_set_buffers(
      disp,
      buf1,
      buf2,
      sizeof(buf1),
      LV_DISPLAY_RENDER_MODE_PARTIAL);

  lv_display_set_flush_cb(disp, my_disp_flush);

  // THE CO5300 FIX: Register the rounder event callback to the display
    lv_display_add_event_cb(disp, my_rounder_event_cb, LV_EVENT_INVALIDATE_AREA, NULL);


  // start RTC
  init_hardware_rtc();
  create_clock_ui();

  // Poll the RTC every 500ms natively via LVGL's internal thread clock
  lv_timer_create(update_clock_ui_cb, 500, NULL);
  init_bt_gadgetbridge();

  i2c_mutex = xSemaphoreCreateMutex();

  // Configure Touch Interrupt Pin
  pinMode(TP_INT, INPUT_PULLUP);

  init_step_counter();

  // Initialize Touch Screen using global Wire bus
  i2c_bus = std::make_shared<Arduino_HWIIC>(IIC_SDA, IIC_SCL, &Wire);
  touch_chip = new Arduino_FT3x68(i2c_bus, FT3168_DEVICE_ADDRESS, TP_RESET, TP_INT);

  if (i2c_mutex && xSemaphoreTake(i2c_mutex, pdMS_TO_TICKS(100)) == pdTRUE)
  {
    // Note: Arduino_HWIIC should not re-call Wire.begin() since init_hardware_rtc/Wire is already initialized
    touch_chip->IIC_Write_Device_State(Arduino_IIC_Touch::Device::TOUCH_GESTUREID_MODE,
                                       Arduino_IIC_Touch::Device_State::TOUCH_DEVICE_ON);
    USBSerial.println("FT3168 Touch controller initialized successfully.");
    xSemaphoreGive(i2c_mutex);
  }

  last_activity_time = millis();
}

void check_touch_input()
{
  if (!touch_chip) return;

  // Poll I2C touch chip only when touch interrupt pin (TP_INT) is active (LOW)
  // or rate-limit when screen is ON
  bool tp_int_active = (digitalRead(TP_INT) == LOW);
  if (!tp_int_active)
  {
    // If screen is awake and timeout is reached, transition to screen off
    if (screen_on && (millis() - last_activity_time >= SCREEN_TIMEOUT_MS))
    {
      USBSerial.println("15s inactivity timeout reached. Entering low power mode.");
      screen_on = false;
      gfx->displayOff();
    }
    return;
  }

  if (i2c_mutex && xSemaphoreTake(i2c_mutex, pdMS_TO_TICKS(10)) != pdTRUE)
  {
    return;
  }

  String gesture = touch_chip->IIC_Read_Device_State(Arduino_IIC_Touch::Status_Information::TOUCH_GESTURE_ID);
  double finger_num = touch_chip->IIC_Read_Device_Value(Arduino_IIC_Touch::Value_Information::TOUCH_FINGER_NUMBER);

  xSemaphoreGive(i2c_mutex);

  static bool was_touching = false;
  static unsigned long last_press_time = 0;
  static uint8_t tap_count = 0;

  bool is_touching = (finger_num > 0);
  bool press_event = (is_touching && !was_touching);
  was_touching = is_touching;

  if (!screen_on)
  {
    // Screen is OFF: check for FT3168 gesture or press edge transitions for double tap wake-up
    bool double_tap_detected = (gesture == "Double Click");

    if (press_event)
    {
      unsigned long now = millis();
      if (now - last_press_time < 500 && now - last_press_time > 50)
      {
        tap_count++;
      }
      else
      {
        tap_count = 1;
      }
      last_press_time = now;

      if (tap_count >= 2)
      {
        double_tap_detected = true;
        tap_count = 0;
      }
    }

    if (double_tap_detected)
    {
      USBSerial.println("Double tap detected! Waking up screen.");
      screen_on = true;
      gfx->displayOn();
      last_activity_time = millis();
      tap_count = 0;
      lv_obj_invalidate(lv_screen_active());
      lv_refr_now(nullptr);
    }
  }
  else
  {
    // Screen is ON: any touch input or gesture (including swipes) resets the 15s inactivity timer
    if (is_touching || (gesture != "No Gesture" && gesture != "->Read TOUCH_GESTURE_ID fail" && gesture != "->Read FT3x68_RD_DEVICE_GESTUREID fail"))
    {
      last_activity_time = millis();
    }

    // Check timeout
    if (millis() - last_activity_time >= SCREEN_TIMEOUT_MS)
    {
      USBSerial.println("15s inactivity timeout reached. Entering low power mode.");
      screen_on = false;
      gfx->displayOff();
      tap_count = 0;
    }
  }
}

void loop()
{

  // Continuously handle step counting regardless of screen state
  if (i2c_mutex && xSemaphoreTake(i2c_mutex, pdMS_TO_TICKS(10)) == pdTRUE)
  {
    bool data_ready = qmi.getDataReady();
    IMUdata accel;
    bool got_accel = false;
    if (data_ready)
    {
      got_accel = qmi.getAccelerometer(accel.x, accel.y, accel.z);
    }
    xSemaphoreGive(i2c_mutex);

    if (data_ready && got_accel)
    {
      if (detectStep(accel))
      {
        step_counter++;
        USBSerial.print("Step Detected, steps: ");
        USBSerial.println(step_counter);

        if (step_label)
        {
          lv_label_set_text_fmt(
              step_label,
              "Steps: %lu",
              static_cast<unsigned long>(step_counter));
        }
      }
    }
  }

  // Handle touch input and screen timeout / wake state
  check_touch_input();

  // Handle LVGL UI rendering when screen is awake
  if (screen_on)
  {
    lv_timer_handler();
    delay(32);
  }
  else
  {
    // Low power mode: pause LVGL timer handler, delay to conserve power while polling sensors
    delay(16);
  }
}
