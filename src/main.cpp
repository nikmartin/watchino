#include "Arduino_GFX.h"
#include "HWCDC.h"
#include "lv_conf.h"
#include "pin_config.h"
#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <NimBLEDevice.h>
#include <Wire.h>
#include <cstdint>
#include <lvgl.h>
// Include the specific SensorLib header for your hardware RTC
#include <SensorPCF85063.hpp>

// --custom lvgl font from Google Fonts--
LV_FONT_DECLARE(tilt_neon_28);
LV_FONT_DECLARE(michroma_28);
LV_FONT_DECLARE(seven_segment_28);
LV_FONT_DECLARE(seven_segment_48);

HWCDC USBSerial;
SensorPCF85063 rtc; // SensorLib RTC driver instance
#define SCREEN_WIDTH 410
#define SCREEN_HEIGHT 502
#define BYTE_PER_PIXEL (LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_RGB565))
// 1/10th buffer strategy to preserve the ESP32-C6's internal 512KB SRAM
#define BUF_SIZE (SCREEN_WIDTH * 40)

// LVGL UI Objects
lv_obj_t *time_label;

// BLE Configuration
#define SERVICE_UUID "6e400001-b5a3-f393-e0a9-e50e24dcca9e" // NUS Service
#define RX_CHARACTERISTIC_UUID                                                 \
  "6e400002-b5a3-f393-e0a9-e50e24dcca9e" // RX (Write)
#define TX_CHARACTERISTIC_UUID                                                 \
  "6e400003-b5a3-f393-e0a9-e50e24dcca9e" // TX (Notify)

NimBLECharacteristic *pTxCharacteristic;
bool deviceConnected = false;

// Variables to hold parsed time
uint16_t current_year = 2026;
uint8_t current_month = 1;
uint8_t current_date = 1;
uint8_t current_hour = 12;
uint8_t current_minute = 0;
uint8_t current_second = 0;

bool time_updated = false;

Arduino_DataBus *bus = new Arduino_ESP32QSPI(
    LCD_CS /* CS */, LCD_SCLK /* SCK */, LCD_SDIO0 /* SDIO0 */,
    LCD_SDIO1 /* SDIO1 */, LCD_SDIO2 /* SDIO2 */, LCD_SDIO3 /* SDIO3 */);

Arduino_GFX *gfx =
    new Arduino_CO5300(bus, LCD_RESET /* RST */, 0 /* rotation */, LCD_WIDTH,
                       LCD_HEIGHT, 22 /* col_offset1 */, 0 /* row_offset1 */,
                       0 /* col_offset2 */, 0 /* row_offset2 */);


// --- 1. Init Hardware RTC via SensorLib ---
void init_hardware_rtc() {

  // SensorLib initialization pattern
  if (!rtc.begin(Wire, IIC_SDA, IIC_SCL)) {
    USBSerial.println("Error: SensorLib could not find PCF85063 chip!");
    return;
  }

  // Start the clock internal oscillator circuit
  rtc.start();
  USBSerial.println("SensorLib RTC Initialized.");
}

void update_clock_ui_cb(lv_timer_t *timer) {
  if (!time_label)
    return;

  // Grab time from hardware registers
  RTC_DateTime now = rtc.getDateTime();

  char time_str[12];
  // Formats text cleanly to show Hours:Minutes:Seconds (e.g., 14:05:32)
  snprintf(time_str, sizeof(time_str), "%02d:%02d:%02d", 
            now.getHour(), now.getMinute(), now.getSecond());

  lv_label_set_text(time_label, time_str);
}
// --- GADGETBRIDGE PARSER ---
// Gadgetbridge sends commands like: "setTime(1717141200);" or raw time updates.
// For a simple implementation, we look for time patterns or you can force
// Gadgetbridge to send plain strings depending on your profile choice.
void parse_gadgetbridge_data(String data) {
  // Basic structural parse for "setTime" or generic hourly updates
  if (data.indexOf("setTime") != -1) {

    int startIdx = data.indexOf('(');
    int endIdx = data.indexOf(')');
    if (startIdx != -1 && endIdx != -1) {
      String timestampStr = data.substring(startIdx + 1, endIdx);
      time_t epoch = (time_t)timestampStr.toInt();

      if (epoch > 0) {
        // SensorLib uses standard time_t (UNIX seconds) directly for
        // synchronization
        struct tm *timeinfo = gmtime(&epoch);

        current_year = timeinfo->tm_year + 1900;
        current_month = timeinfo->tm_mon + 1; // tm_mon is 0-11
        current_date = timeinfo->tm_mday;
        current_hour = timeinfo->tm_hour;
        current_minute = timeinfo->tm_min;
        current_second = timeinfo->tm_sec;
        rtc.setDateTime(current_year, current_month, current_date, current_hour,
                        current_minute, current_second);

        Serial.printf("SensorLib RTC synced to epoch: %lld\n", epoch);
      }
    }
    time_updated = true;
  }

  // Fallback: If Gadgetbridge passes a raw system string via notification test
  // or sync lines containing "HH:MM"
  int colonIndex = data.indexOf(':');
  // Extract seconds if present after a second colon
  int secondColonIndex = data.indexOf(':', colonIndex + 1);
  if (secondColonIndex != -1) {
    String sStr = data.substring(secondColonIndex + 1, secondColonIndex + 3);
    current_second = sStr.toInt();
  }
  if (colonIndex > 0 && colonIndex < data.length() - 1) {
    String hStr = data.substring(colonIndex - 2, colonIndex);
    String mStr = data.substring(colonIndex + 1, colonIndex + 3);

    current_hour = hStr.toInt();
    current_minute = mStr.toInt();
    time_updated = true;
  }
}
// BLE Callback Class
class MyServerCallbacks : public NimBLEServerCallbacks {
  void onConnect(NimBLEServer *pServer) { deviceConnected = true; };
  void onDisconnect(NimBLEServer *pServer) { deviceConnected = false; }
};

class MyCharacteristicCallbacks : public NimBLECharacteristicCallbacks {
  void onWrite(NimBLECharacteristic *pCharacteristic) {
    std::string rxValue = pCharacteristic->getValue();
    if (rxValue.length() > 0) {
      String data = String(rxValue.c_str());
      parse_gadgetbridge_data(data);
    }
  }
};

// --- LVGL UI INITIALIZATION ---
void create_clock_ui() {
  // 1. Create a label in the absolute middle of your 410x502 screen
  time_label = lv_label_create(lv_screen_active());
  lv_obj_align(time_label, LV_ALIGN_CENTER, 0, 0);

  // 2. Styling for a retro aesthetic
  lv_obj_set_style_text_color(time_label, lv_color_hex(RGB565_LAWNGREEN),
                              LV_PART_MAIN); // Classic Green LED

  // 3. Set a custom font
 
  lv_obj_set_style_text_font(time_label, &seven_segment_48, LV_PART_MAIN);

  lv_label_set_text(time_label, "12:00:00");
}

// 3. LVGL 9.5 Display Flush Callback using Arduino_GFX draw API
void my_disp_flush(lv_display_t *display, const lv_area_t *area,
                   uint8_t *px_map) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  // Push the native LVGL 16-bit RGB pixels via standard Arduino_GFX DMA burst
  gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)px_map, w, h);

  // Advise the LVGL UI thread that processing has completed
  lv_display_flush_ready(display);
}

void setup() {
  USBSerial.begin(115200);
  // USBSerial.setDebugOutput(true);
  // while(!USBSerial);
  USBSerial.println("Arduino_GFX Hello World example");

#ifdef GFX_EXTRA_PRE_INIT
  GFX_EXTRA_PRE_INIT();
#endif
  // get i2c bus going
  Wire.begin(IIC_SDA, IIC_SCL, 400000);
  // Init Display
  if (!gfx->begin()) {
    USBSerial.println("gfx->begin() failed!");
  }
  gfx->fillScreen(RGB565_BLACK);
  // Initialize core UI layout
  lv_init();

  // Allocate frame buffers (Dual buffering addresses QSPI tearing/flickering)
  static uint8_t buf1[BUF_SIZE * BYTE_PER_PIXEL];
  static uint8_t buf2[BUF_SIZE * BYTE_PER_PIXEL];

  // Target API setup for LVGL 9.5 display rendering engine
  lv_display_t *disp = lv_display_create(SCREEN_WIDTH, SCREEN_HEIGHT);
  lv_display_set_buffers(disp, buf1, buf2, sizeof(buf1),
                         LV_DISPLAY_RENDER_MODE_PARTIAL);
  lv_display_set_flush_cb(disp, my_disp_flush);

  // (Your hardware display and LVGL setup code should run here)
  create_clock_ui();

  // Poll the RTC every 500ms natively via LVGL's internal thread clock
  lv_timer_create(update_clock_ui_cb, 500, NULL);
  // Initialize BLE
  NimBLEDevice::init("Bangle.js-C6"); // Name your device Bangle.js to trigger
                                      // the Gadgetbridge profile
  NimBLEServer *pServer = NimBLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  NimBLEService *pService = pServer->createService(SERVICE_UUID);
  pTxCharacteristic = pService->createCharacteristic(TX_CHARACTERISTIC_UUID,
                                                     NIMBLE_PROPERTY::NOTIFY);

  NimBLECharacteristic *pRxCharacteristic = pService->createCharacteristic(
      RX_CHARACTERISTIC_UUID,
      NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_NR);
  pRxCharacteristic->setCallbacks(new MyCharacteristicCallbacks());

  // pService->start();
  //  pServer->start();
  pServer->getAdvertising()->addServiceUUID(SERVICE_UUID);
  pServer->getAdvertising()->start();

  Serial.println("BLE Watch Ready for Gadgetbridge Pairing...");
  delay(2000); // 2 seconds
}

void loop() {

  lv_timer_handler(); // Keep LVGL spinning

  // Update the UI if Gadgetbridge updated our variables
  if (time_updated) {
    char time_str[6];
    snprintf(time_str, sizeof(time_str), "%02d:%02d", current_hour,
             current_minute);

    lv_label_set_text(time_label, time_str);
    time_updated = false;
  }

  delay(5);
}
