#include "HWCDC.h"
#include "lv_conf.h"
#include "lv_conf_internal.h"
#include "pin_config.h"
#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <NimBLEDevice.h>
#include <Wire.h>
#include <lvgl.h>

extern "C" {
LV_FONT_DECLARE(tilt_neon);
}

HWCDC USBSerial;

#define SCREEN_WIDTH 410
#define SCREEN_HEIGHT 502
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
int current_hour = 12;
int current_minute = 00;
int current_second = 00;
bool time_updated = false;

Arduino_DataBus *bus = new Arduino_ESP32QSPI(
    LCD_CS /* CS */, LCD_SCLK /* SCK */, LCD_SDIO0 /* SDIO0 */,
    LCD_SDIO1 /* SDIO1 */, LCD_SDIO2 /* SDIO2 */, LCD_SDIO3 /* SDIO3 */);

Arduino_GFX *gfx =
    new Arduino_CO5300(bus, LCD_RESET /* RST */, 0 /* rotation */, LCD_WIDTH,
                       LCD_HEIGHT, 22 /* col_offset1 */, 0 /* row_offset1 */,
                       0 /* col_offset2 */, 0 /* row_offset2 */);

// --- GADGETBRIDGE PARSER ---
// Gadgetbridge sends commands like: "setTime(1717141200);" or raw time updates.
// For a simple implementation, we look for time patterns or you can force
// Gadgetbridge to send plain strings depending on your profile choice.
void parse_gadgetbridge_data(String data) {
  // Basic structural parse for "setTime" or generic hourly updates
  if (data.indexOf("setTime") != -1) {
    // Extract epoch or hours/minutes if your app variant passes text
    // For demonstration, let's assume you've grabbed the hours/minutes:
    // (Alternatively, use Gadgetbridge's fallback string format)

    // Let's say you parse out the integers:
    // current_hour = ...
    // current_minute = ...
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

// --- LVGL UI INITIALIZATION ---
void create_7segment_clock_ui() {
  // 1. Create a label in the absolute middle of your 410x410 screen
  time_label = lv_label_create(lv_screen_active());
  lv_obj_align(time_label, LV_ALIGN_CENTER, 0, 0);

  // 2. Styling for a retro aesthetic
  lv_obj_set_style_text_color(time_label, lv_color_hex(0x00FF00),
                              LV_PART_MAIN); // Classic Green LED

  // 3. Set a fallback font or your custom 7-segment font (See Step 2 below)
  // For now, we use a large built-in font size
  lv_obj_set_style_text_font(time_label, &tilt_neon, LV_PART_MAIN);

  lv_label_set_text(time_label, "12:00:00");
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

  // Init Display
  if (!gfx->begin()) {
    USBSerial.println("gfx->begin() failed!");
  }
  gfx->fillScreen(RGB565_WHITE);
  // Initialize core UI layout
  lv_init();

  // Allocate frame buffers (Dual buffering addresses QSPI tearing/flickering)
  static uint8_t buf1[SCREEN_WIDTH * 40 * sizeof(lv_color_t)];
  static uint8_t buf2[SCREEN_WIDTH * 40 * sizeof(lv_color_t)];

  // Target API setup for LVGL 9.5 display rendering engine
  lv_display_t *disp = lv_display_create(SCREEN_WIDTH, SCREEN_HEIGHT);
  lv_display_set_buffers(disp, buf1, buf2, sizeof(buf1),
                         LV_DISPLAY_RENDER_MODE_PARTIAL);
  lv_display_set_flush_cb(disp, my_disp_flush);

  // (Your hardware display and LVGL setup code should run here)
  create_7segment_clock_ui();

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

  pService->start();
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
