#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RTClib.h>
#include <Adafruit_Fingerprint.h>
#include <Adafruit_NeoPixel.h>
#include <pgmspace.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>

// ============================================================
// A.T.O.M. (Access, Tracking & Operation Monitoring) - ADMIN MODE V1.7.5
// ESP32-S3 DevKitC-1 N16R8
// ============================================================

// ================= GPIO ================================

#define BTN_UP        4
#define BTN_DOWN      5
#define BTN_OK        6
#define BTN_BACK      7

#define I2C_SDA       8
#define I2C_SCL       9

#define BUZZER_PIN    12

#define FP_RX         18
#define FP_TX         17

// RGB onboard ESP32-S3 DevKitC-1 menggunakan GPIO48.
#define RGB_LED_PIN   48
#define RGB_LED_COUNT 1

#define BUZZER_ON     HIGH
#define BUZZER_OFF    LOW

// ================= OLED =================================

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define OLED_ADDRESS  0x3C

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  -1
);

GFXcanvas1 canvasOld(SCREEN_WIDTH, SCREEN_HEIGHT);
GFXcanvas1 canvasNew(SCREEN_WIDTH, SCREEN_HEIGHT);

// ================= RTC ==================================

RTC_DS3231 rtc;

// ================= FINGERPRINT ==========================

HardwareSerial FPSerial(1);
Adafruit_Fingerprint finger(&FPSerial);

// ================= RGB LED =============================

Adafruit_NeoPixel rgbLed(
  RGB_LED_COUNT,
  RGB_LED_PIN,
  NEO_GRB + NEO_KHZ800
);

void setRGB(uint8_t r, uint8_t g, uint8_t b) {
  rgbLed.setPixelColor(0, rgbLed.Color(r, g, b));
  rgbLed.show();
}

void ledWaiting()      { setRGB(0, 0, 255); }
void ledGranted()      { setRGB(0, 255, 0); }
void ledDenied()       { setRGB(255, 100, 0); }
void ledSystemFailure(){ setRGB(255, 0, 0); }
void ledOff()          { setRGB(0, 0, 0); }

// ================= BITMAP PLACEHOLDERS ==================

#define LOGO_WIDTH   128
#define LOGO_HEIGHT  64
#define ICON_WIDTH   20
#define ICON_HEIGHT  20
#define ICON_BITMAP_SIZE (((ICON_WIDTH + 7) / 8) * ICON_HEIGHT)

const uint8_t logo_bitmap[LOGO_WIDTH * LOGO_HEIGHT / 8] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 
	0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 
	0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 
	0x00, 0x00, 0x77, 0xff, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xb8, 
	0x00, 0x00, 0x6f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xdc, 
	0x00, 0x00, 0x6f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xec, 
	0x00, 0x00, 0x6e, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x7f, 0xff, 0xe0, 0xff, 0xff, 0xec, 
	0x00, 0x00, 0x6f, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x3f, 0xfe, 0x00, 0x0f, 0xff, 0xec, 
	0x00, 0x00, 0x6f, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x9f, 0xfc, 0x0f, 0x07, 0xff, 0xcc, 
	0x00, 0x00, 0x6f, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcf, 0xf0, 0xff, 0xe1, 0xff, 0xcc, 
	0x00, 0x00, 0x6f, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcf, 0xe3, 0xf8, 0xf0, 0xff, 0xcc, 
	0x00, 0x00, 0x6f, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x9f, 0xc7, 0x18, 0x1c, 0x7f, 0xcc, 
	0x00, 0x00, 0x6f, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x3f, 0x8c, 0x1e, 0x0e, 0x7f, 0xec, 
	0x00, 0x00, 0x6e, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x7f, 0x98, 0xff, 0xc3, 0xff, 0xec, 
	0x00, 0x00, 0x6f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x19, 0xf1, 0xe1, 0x9f, 0xec, 
	0x00, 0x00, 0x6f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x33, 0x80, 0xf8, 0x8f, 0xec, 
	0x00, 0x00, 0x6f, 0xc1, 0xfc, 0x00, 0x7f, 0x07, 0xf8, 0x78, 0x7e, 0x63, 0x07, 0x9c, 0xcf, 0xec, 
	0x00, 0x00, 0x6f, 0x80, 0xfc, 0x00, 0x7e, 0x03, 0xf8, 0x30, 0x7e, 0x66, 0x3f, 0x8e, 0x4f, 0xec, 
	0x00, 0x00, 0x6f, 0x00, 0x7c, 0x00, 0x7c, 0x21, 0xf8, 0x00, 0x7e, 0x66, 0x79, 0xc6, 0x4f, 0xec, 
	0x00, 0x00, 0x6f, 0x08, 0x7f, 0xc7, 0xfc, 0x71, 0xf8, 0x00, 0x7c, 0x66, 0x60, 0xe6, 0x67, 0xec, 
	0x00, 0x00, 0x6f, 0x1c, 0x7f, 0xc7, 0xfc, 0x71, 0xf8, 0x84, 0x7c, 0xc6, 0x60, 0x67, 0xe7, 0xdc, 
	0x00, 0x00, 0xef, 0x1c, 0x7f, 0xc7, 0xfc, 0x71, 0xf8, 0xcc, 0x7c, 0xce, 0x66, 0x67, 0xe7, 0xb8, 
	0x00, 0x01, 0xdf, 0x1c, 0x7f, 0xc7, 0xfc, 0x71, 0xf8, 0xcc, 0x7c, 0xc6, 0x7e, 0x66, 0x7f, 0x70, 
	0x00, 0x03, 0xbf, 0x00, 0x7f, 0xc7, 0xfc, 0x71, 0xf8, 0xfc, 0x7c, 0x66, 0x7e, 0x66, 0x67, 0x60, 
	0x00, 0x07, 0x7f, 0x00, 0x7f, 0xc7, 0xfc, 0x21, 0xf8, 0xfc, 0x7e, 0x7e, 0x67, 0xe6, 0x67, 0x60, 
	0x00, 0x0e, 0xff, 0x1c, 0x67, 0xc7, 0xce, 0x03, 0x98, 0xfc, 0x7e, 0x66, 0x67, 0xfc, 0xe7, 0x60, 
	0x00, 0x1d, 0xff, 0x1c, 0x67, 0xc7, 0xcf, 0x07, 0x98, 0xfc, 0x7e, 0x66, 0x66, 0x7c, 0xcf, 0x60, 
	0x00, 0x3b, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x66, 0x66, 0x64, 0xcf, 0x60, 
	0x00, 0x77, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x67, 0xe6, 0x66, 0x4f, 0x60, 
	0x00, 0xee, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xe7, 0xe0, 0x66, 0x4f, 0x60, 
	0x01, 0xdf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe6, 0x70, 0xe7, 0xff, 0x60, 
	0x03, 0xbf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x66, 0x7f, 0xe6, 0x7f, 0x60, 
	0x07, 0x7e, 0x73, 0x98, 0x63, 0x1c, 0x11, 0xce, 0x6d, 0x5b, 0x3e, 0x66, 0x4f, 0x26, 0xcf, 0x60, 
	0x0e, 0xfd, 0xad, 0x6b, 0xde, 0xff, 0x76, 0xb5, 0xab, 0x4a, 0xfe, 0x77, 0xe0, 0x7c, 0xcf, 0x60, 
	0x1d, 0xfd, 0xaf, 0x78, 0xe7, 0x3f, 0x76, 0xb5, 0xe7, 0x52, 0x9f, 0xff, 0xf0, 0xfc, 0xef, 0x60, 
	0x3b, 0xfc, 0x2d, 0x6b, 0xfb, 0xdf, 0x71, 0x85, 0xab, 0x5a, 0xdf, 0xfe, 0x7f, 0xff, 0xff, 0x60, 
	0x37, 0xfd, 0xb3, 0x98, 0x46, 0x3f, 0x76, 0xb6, 0x6d, 0x5b, 0x3f, 0xfe, 0x7f, 0xff, 0xff, 0x60, 
	0x37, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x70, 
	0x37, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xb8, 
	0x37, 0xff, 0xf9, 0x8c, 0x23, 0x98, 0x2c, 0xdb, 0x37, 0xe1, 0xcd, 0xa8, 0x33, 0x1a, 0xd8, 0xdc, 
	0x33, 0xff, 0xf6, 0xb5, 0xed, 0x6e, 0xeb, 0x4a, 0xd7, 0xea, 0xb4, 0xae, 0xed, 0x6a, 0x57, 0xec, 
	0x33, 0xff, 0xf6, 0xb4, 0x6d, 0x6e, 0xeb, 0x52, 0xd7, 0xea, 0xb5, 0x2e, 0xed, 0x6a, 0x94, 0xec, 
	0x33, 0xff, 0xf6, 0x8d, 0xe3, 0x0e, 0xeb, 0x5a, 0x17, 0xea, 0xb5, 0xae, 0xed, 0x1a, 0xd6, 0xec, 
	0x33, 0xff, 0xf9, 0xbc, 0x2d, 0x6e, 0xec, 0xda, 0xd0, 0xea, 0xcd, 0xae, 0xf3, 0x6a, 0xd9, 0xec, 
	0x33, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xec, 
	0x33, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xec, 
	0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xdc, 
	0x33, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xb8, 
	0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x70, 
	0x33, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xe0, 
	0x33, 0x06, 0xed, 0xfb, 0x86, 0x0c, 0x18, 0x7c, 0x18, 0x38, 0xee, 0xff, 0x99, 0x99, 0x9d, 0xc0, 
	0x33, 0x7e, 0x6d, 0xfb, 0x7f, 0xbd, 0xfb, 0xbf, 0x7b, 0xf7, 0x6e, 0xff, 0x33, 0x33, 0x3b, 0x80, 
	0x37, 0x1e, 0xad, 0xfb, 0x8f, 0xbc, 0x7b, 0xbf, 0x78, 0xf7, 0xe0, 0xfe, 0x66, 0x66, 0x77, 0x00, 
	0x37, 0x7e, 0xad, 0xfb, 0xf7, 0xbd, 0xf8, 0x7f, 0x7b, 0xf7, 0xee, 0xfc, 0xcc, 0xcc, 0xee, 0x00, 
	0x37, 0x7e, 0xcd, 0xfb, 0x77, 0xbd, 0xfb, 0xbf, 0x7b, 0xf7, 0x6e, 0xf9, 0x99, 0x99, 0xdc, 0x00, 
	0x3b, 0x06, 0xee, 0x1b, 0x8f, 0xbc, 0x1b, 0xbf, 0x78, 0x38, 0xee, 0xd3, 0x33, 0x33, 0xb8, 0x00, 
	0x1d, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x70, 0x00, 
	0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x00, 
	0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 
	0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const uint8_t clock_icon[ICON_BITMAP_SIZE] PROGMEM = {
	0x00, 0x60, 0x00, 0x03, 0xfc, 0x00, 0x0f, 0x9f, 0x00, 0x1e, 0x07, 0x80, 0x38, 0x01, 0xc0, 0x30, 
	0x60, 0xc0, 0x70, 0x60, 0xe0, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0xc7, 0xe0, 0x30, 0xc7, 0xe0, 
	0x30, 0x60, 0x10, 0x60, 0x60, 0x08, 0x60, 0x70, 0x04, 0xe0, 0x30, 0x00, 0xc0, 0x38, 0x01, 0xc0, 
	0x1e, 0x07, 0x80, 0x0f, 0x9f, 0x00, 0x03, 0xfc, 0x00, 0x00, 0x60, 0x00
};
const uint8_t shield_icon[ICON_BITMAP_SIZE] PROGMEM = {
	0x00, 0x60, 0x00, 0x31, 0xf8, 0xc0, 0x7f, 0x9f, 0xe0, 0x7e, 0x07, 0xe0, 0x60, 0x00, 0x60, 0x60, 
	0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 
	0x60, 0x60, 0x60, 0x60, 0x60, 0x00, 0x60, 0x70, 0x60, 0xe0, 0x30, 0x60, 0xc0, 0x38, 0x01, 0xc0, 
	0x1e, 0x07, 0x80, 0x0f, 0x9f, 0x00, 0x03, 0xfc, 0x00, 0x00, 0x60, 0x00
};

const uint8_t fingerprint_icon[ICON_BITMAP_SIZE] PROGMEM = {
  0x03, 0xe4, 0x00, 0x04, 0x02, 0x00, 0x09, 0x99, 0x00, 0x12, 0x04, 0x80,
  0x24, 0xf2, 0x40, 0x29, 0x09, 0x20, 0x4a, 0x64, 0xa0, 0x52, 0x92, 0x80,
  0x44, 0x88, 0x80, 0x45, 0x28, 0x20, 0x55, 0x4a, 0x20, 0x15, 0x4a, 0x80,
  0x15, 0x52, 0xa0, 0x55, 0x54, 0xa0, 0x45, 0x54, 0x80, 0x51, 0x54, 0x80,
  0x52, 0x54, 0x80, 0x0a, 0x15, 0x00, 0x01, 0x80, 0x00, 0x00, 0x60, 0x00
};

const uint8_t fail_icon[ICON_BITMAP_SIZE] PROGMEM = {
	0x01, 0xf8, 0x00, 0x06, 0x06, 0x00, 0x18, 0x01, 0x80, 0x20, 0x00, 0x40, 0x2f, 0x0f, 0x40, 0x46, 
	0x06, 0x20, 0x46, 0x06, 0x20, 0x86, 0x06, 0x10, 0x8c, 0x03, 0x10, 0x99, 0xf9, 0x90, 0xb2, 0x04, 
	0xd0, 0xb4, 0x02, 0xd0, 0xb4, 0x02, 0xd0, 0x70, 0x00, 0xe0, 0x70, 0x00, 0xe0, 0x30, 0x00, 0xc0, 
	0x30, 0x00, 0xc0, 0x38, 0x01, 0xc0, 0x36, 0x06, 0xc0, 0x31, 0xf8, 0xc0
};

#define INFO_ICON_WIDTH  20
#define INFO_ICON_HEIGHT 20
const uint8_t information_icon[60] PROGMEM = {
  0x01, 0xf8, 0x00, 0x07, 0xfe, 0x00, 0x1f, 0x0f, 0x80, 0x38, 0x01, 0xc0,
  0x30, 0x60, 0xc0, 0x60, 0x60, 0x60, 0x60, 0x00, 0x60, 0xe0, 0x00, 0x70,
  0xc0, 0x60, 0x30, 0xc0, 0x60, 0x30, 0xc0, 0x60, 0x30, 0xc0, 0x60, 0x30,
  0xe0, 0x60, 0x30, 0x60, 0x60, 0x60, 0x60, 0x60, 0xe0, 0x30, 0x60, 0xc0,
  0x38, 0x03, 0xc0, 0x1f, 0x0f, 0x80, 0x07, 0xfe, 0x00, 0x01, 0xf8, 0x00
};

const uint8_t success_icon[ICON_BITMAP_SIZE] PROGMEM = {
  0x91, 0xf8, 0x90, 0x46, 0x06, 0x20, 0x18, 0x01, 0x80, 0xa0, 0x00, 0x50,
  0x20, 0x00, 0x40, 0x4f, 0x0f, 0x20, 0x50, 0x90, 0xa0, 0x90, 0x90, 0x90,
  0x80, 0x00, 0x10, 0x80, 0x00, 0x10, 0x8f, 0xff, 0x10, 0x90, 0x00, 0x90,
  0x90, 0x00, 0x90, 0x48, 0xf1, 0x20, 0x49, 0x69, 0x20, 0x25, 0x0a, 0x40,
  0x23, 0xfc, 0x40, 0x18, 0x01, 0x80, 0x06, 0x06, 0x00, 0x01, 0xf8, 0x00
};


// ================= DEVICE STATUS ========================

bool oledOK = false;
bool rtcOK = false;
bool fingerOK = false;


// ================= SCREEN SYSTEM ========================

enum ScreenId : uint8_t {
  SCREEN_MAIN_A = 0,
  SCREEN_MAIN_B,
  SCREEN_CLOCK_A,
  SCREEN_CLOCK_B,
  SCREEN_ADMIN_A,
  SCREEN_ADMIN_B,
  SCREEN_INFORMATION_A,
  SCREEN_INFORMATION_B,
  SCREEN_LOGO
};

ScreenId currentScreen = SCREEN_LOGO;

// Main menu order requested by user.
const ScreenId menuScreens[] = {
  SCREEN_MAIN_A,
  SCREEN_CLOCK_A,
  SCREEN_ADMIN_A,
  SCREEN_INFORMATION_A,
  SCREEN_LOGO
};

const uint8_t MENU_COUNT = sizeof(menuScreens) / sizeof(menuScreens[0]);
int8_t menuIndex = 0;

bool screenDirty = true;

// --- PROTOTYPE FUNGSI UNTUK MENCEGAH ERROR KOMPILASI ---
void drawScreen(GFXcanvas1 &c, ScreenId screen);
void slideAnimation(ScreenId from, ScreenId to, bool reverse);
int8_t findMenuIndex(ScreenId screen);
// -------------------------------------------------------

// Boot logo is also available as a menu item.
bool bootSplashActive = true;
unsigned long bootSplashStarted = 0;
const unsigned long BOOT_SPLASH_DURATION = 1200;

// ================= BUTTONS ==============================

bool lastUp = HIGH;
bool lastDown = HIGH;
bool lastOK = HIGH;
bool lastBack = HIGH;

unsigned long lastDebounce = 0;
const unsigned long DEBOUNCE_DELAY = 150;

// ================= BUZZER ================================

bool buzzerActive = false;
bool buzzerState = false;
int beepRemaining = 0;
unsigned long buzzerTimer = 0;

const unsigned long BEEP_ON_TIME = 100;
const unsigned long BEEP_OFF_TIME = 100;

void startBeep(bool success) {
  if (buzzerActive) return;

  beepRemaining = success ? 1 : 2;
  buzzerActive = true;
  buzzerState = true;

  digitalWrite(BUZZER_PIN, BUZZER_ON);
  buzzerTimer = millis();
}

void updateBuzzer() {
  if (!buzzerActive) return;

  unsigned long now = millis();
  unsigned long interval = buzzerState ? BEEP_ON_TIME : BEEP_OFF_TIME;
  if (now - buzzerTimer < interval) return;

  buzzerTimer = now;

  if (buzzerState) {
    digitalWrite(BUZZER_PIN, BUZZER_OFF);
    buzzerState = false;
    beepRemaining--;

    if (beepRemaining <= 0) {
      buzzerActive = false;
    }
  } else {
    digitalWrite(BUZZER_PIN, BUZZER_ON);
    buzzerState = true;
  }
}

// ================= AUTHENTICATION / SESSION ==============

bool waitingFingerRemoval = false;
unsigned long lastFingerprintScan = 0;
const unsigned long FINGERPRINT_INTERVAL = 120;

uint16_t matchedID = 0;
uint16_t confidence = 0;

String activeOperatorName = "";
bool sessionActive = false;
bool adminModeActive = false;

// ================= ADMIN MODE V1.7 ======================
// Ganti kedua password sebelum firmware digunakan.
// AP password minimal 8 karakter; admin password digunakan HTTP Basic Auth.
static const char* ADMIN_AP_SSID = "A.T.O.M-ADMIN";
static const char* ADMIN_AP_PASSWORD = "ATOM-ADMIN";
static const char* ADMIN_WEB_USER = "admin";
static const char* ADMIN_WEB_PASSWORD = "atom-admin";

static const uint16_t MAX_FINGERPRINT_ID = 127;
static const uint16_t FIRST_NEW_OPERATOR_ID = 4; // ID 1-3 dipertahankan dari baseline V1.6.
static const unsigned long ADMIN_IDLE_TIMEOUT = 10UL * 60UL * 1000UL;
static const unsigned long ENROLL_TIMEOUT = 45000UL;
static const unsigned long ENROLL_POLL_INTERVAL = 120UL;

WebServer adminServer(80);
Preferences operatorPrefs;
bool preferencesOK = false;
bool adminServerStarted = false;
bool adminRoutesRegistered = false;
unsigned long adminLastActivity = 0;

String operatorKey(uint16_t id) {
  char key[8];
  snprintf(key, sizeof(key), "op%03u", (unsigned)id);
  return String(key);
}

String jsonEscape(const String &input) {
  String out;
  out.reserve(input.length() + 8);
  for (size_t i = 0; i < input.length(); i++) {
    char c = input[i];
    if (c == '"' || c == '\\') { out += '\\'; out += c; }
    else if (c == '\n') out += "\\n";
    else if (c == '\r') out += "\\r";
    else if (c == '\t') out += "\\t";
    else if ((uint8_t)c >= 0x20) out += c;
  }
  return out;
}

String getOperatorName(uint16_t id) {
  if (preferencesOK) {
    String saved = operatorPrefs.getString(operatorKey(id).c_str(), "");
    if (saved.length()) return saved;
  }
  char fallback[24];
  snprintf(fallback, sizeof(fallback), "OPERATOR %02u", (unsigned)id);
  return String(fallback);
}

void seedLegacyOperatorNames() {
  if (!preferencesOK) return;
  if (!operatorPrefs.getString("op001", "").length()) operatorPrefs.putString("op001", "OPERATOR 01");
  if (!operatorPrefs.getString("op002", "").length()) operatorPrefs.putString("op002", "OPERATOR 02");
  if (!operatorPrefs.getString("op003", "").length()) operatorPrefs.putString("op003", "OPERATOR 03");
}

bool requireAdminAuth() {
  if (!adminModeActive) {
    adminServer.send(403, "application/json", "{\"ok\":false,\"error\":\"Admin mode is inactive\"}");
    return false;
  }
  if (!adminServer.authenticate(ADMIN_WEB_USER, ADMIN_WEB_PASSWORD)) {
    adminServer.requestAuthentication(BASIC_AUTH, "A.T.O.M. ADMIN", "Authentication required");
    return false;
  }
  adminLastActivity = millis();
  return true;
}

// ---------- Fingerprint enrollment state machine ----------
enum EnrollState : uint8_t {
  ENROLL_IDLE = 0,
  ENROLL_WAIT_FIRST,
  ENROLL_WAIT_REMOVE,
  ENROLL_WAIT_SECOND,
  ENROLL_DONE,
  ENROLL_FAILED
};

EnrollState enrollState = ENROLL_IDLE;
uint16_t enrollTargetId = 0;
String enrollTargetName = "";
String enrollMessage = "Idle";
unsigned long enrollStartedAt = 0;
unsigned long enrollLastPoll = 0;

const char* enrollStateText() {
  switch (enrollState) {
    case ENROLL_WAIT_FIRST: return "Place finger (1/2)";
    case ENROLL_WAIT_REMOVE: return "Remove finger";
    case ENROLL_WAIT_SECOND: return "Place same finger (2/2)";
    case ENROLL_DONE: return "Completed";
    case ENROLL_FAILED: return "Failed";
    default: return "Idle";
  }
}

void failEnrollment(const String &reason) {
  enrollState = ENROLL_FAILED;
  enrollMessage = reason;
  Serial.print("[ADMIN][ENROLL] FAILED: "); Serial.println(reason);
}

void updateEnrollment() {
  if (enrollState != ENROLL_WAIT_FIRST && enrollState != ENROLL_WAIT_REMOVE && enrollState != ENROLL_WAIT_SECOND) return;

  unsigned long now = millis();
  if (now - enrollStartedAt >= ENROLL_TIMEOUT) {
    failEnrollment("Enrollment timeout");
    return;
  }
  if (now - enrollLastPoll < ENROLL_POLL_INTERVAL) return;
  enrollLastPoll = now;

  uint8_t result = finger.getImage();
  if (enrollState == ENROLL_WAIT_FIRST) {
    if (result == FINGERPRINT_NOFINGER) return;
    if (result != FINGERPRINT_OK) { failEnrollment("Image capture 1 failed: " + String(result)); return; }
    result = finger.image2Tz(1);
    if (result != FINGERPRINT_OK) { failEnrollment("Image conversion 1 failed: " + String(result)); return; }
    enrollState = ENROLL_WAIT_REMOVE;
    enrollMessage = "Remove finger completely";
    Serial.println("[ADMIN][ENROLL] First image captured");
    return;
  }

  if (enrollState == ENROLL_WAIT_REMOVE) {
    if (result == FINGERPRINT_NOFINGER) {
      enrollState = ENROLL_WAIT_SECOND;
      enrollMessage = "Place the same finger again";
      Serial.println("[ADMIN][ENROLL] Finger removed; waiting second scan");
    } else if (result != FINGERPRINT_OK && result != FINGERPRINT_NOFINGER) {
      failEnrollment("Finger removal check failed: " + String(result));
    }
    return;
  }

  if (enrollState == ENROLL_WAIT_SECOND) {
    if (result == FINGERPRINT_NOFINGER) return;
    if (result != FINGERPRINT_OK) { failEnrollment("Image capture 2 failed: " + String(result)); return; }
    result = finger.image2Tz(2);
    if (result != FINGERPRINT_OK) { failEnrollment("Image conversion 2 failed: " + String(result)); return; }
    result = finger.createModel();
    if (result != FINGERPRINT_OK) { failEnrollment("Fingerprint mismatch or model creation failed: " + String(result)); return; }
    result = finger.storeModel(enrollTargetId);
    if (result != FINGERPRINT_OK) { failEnrollment("Sensor storage failed: " + String(result)); return; }

    if (!preferencesOK) {
      finger.deleteModel(enrollTargetId);
      failEnrollment("NVS unavailable; template rolled back");
      return;
    }
    operatorPrefs.putString(operatorKey(enrollTargetId).c_str(), enrollTargetName);
    enrollState = ENROLL_DONE;
    enrollMessage = "Operator enrolled successfully";
    finger.getTemplateCount();
    Serial.printf("[ADMIN][ENROLL] Stored ID %u (%s)\n", (unsigned)enrollTargetId, enrollTargetName.c_str());
  }
}

const char ADMIN_HTML[] PROGMEM = R"PC200HTML(
<!doctype html><html lang="en"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1"><title>A.T.O.M. Admin</title>
<style>body{font-family:Arial,sans-serif;background:#101820;color:#edf2f4;margin:0;padding:18px}main{max-width:760px;margin:auto}h1{font-size:22px}section{background:#1c2b36;padding:16px;border-radius:10px;margin:14px 0}input,button{padding:10px;margin:5px 3px;border-radius:6px;border:1px solid #647783}input{max-width:95%;background:#f7f7f7}button{background:#d9a441;color:#111;font-weight:bold;cursor:pointer}.muted{color:#aab7c0;font-size:13px}.operator{display:flex;gap:8px;align-items:center;justify-content:space-between;border-top:1px solid #40515c;padding:9px 0}.operator span{overflow-wrap:anywhere}.danger{background:#c94c4c;color:white}#message{white-space:pre-wrap}</style></head><body><main>
<h1>A.T.O.M. — ADMIN</h1><p class="muted">Access, Tracking &amp; Operation Monitoring • Connect to A.T.O.M-ADMIN and open 192.168.4.1</p>
<section><h2>System status</h2><div id="status">Loading…</div><button onclick="refreshStatus()">Refresh status</button></section>
<section><h2>Enroll operator</h2><p class="muted">New IDs start at 4. IDs 1–3 are reserved for existing V1.6 templates. Use only IDs confirmed empty in the sensor.</p><form id="enrollForm"><input id="opid" type="number" min="4" max="127" placeholder="Fingerprint ID (4–127)" required><input id="opname" maxlength="20" placeholder="Operator name" required><button type="submit">Start enrollment</button></form><p id="enrollStatus">Idle</p><button id="cancelEnroll" class="danger" onclick="cancelEnrollment()" hidden>Cancel enrollment</button></section>
<section><h2>Operators</h2><div id="operators">Loading…</div><button onclick="loadOperators()">Refresh operators</button></section>
<p id="message" class="muted"></p></main><script>
const $=id=>document.getElementById(id);async function api(path,method='GET',data=null){let o={method,headers:{}};if(data){o.headers['Content-Type']='application/x-www-form-urlencoded';o.body=new URLSearchParams(data)}let r=await fetch(path,o);let j=await r.json();if(!r.ok||j.ok===false)throw Error(j.error||'Request failed');return j}
async function refreshStatus(){try{let s=await api('/api/status');$('status').textContent=`Admin: ${s.admin?'ACTIVE':'INACTIVE'} | Fingerprint: ${s.fingerprint?'READY':'ERROR'} | Templates: ${s.templates} | AP: ${s.ip}`}catch(e){$('status').textContent=e.message}}
async function loadOperators(){try{let j=await api('/api/operators');$('operators').innerHTML='';if(!j.operators.length){$('operators').textContent='No operator metadata';return}j.operators.forEach(o=>{let d=document.createElement('div');d.className='operator';let label=document.createElement('span');label.textContent=`ID ${o.id} — ${o.name}`;let edit=document.createElement('button');edit.textContent='Rename';edit.onclick=async()=>{let n=prompt('New operator name',o.name);if(n&&n.trim()){try{await api('/api/operator/rename','POST',{id:o.id,name:n.trim()});await loadOperators()}catch(e){alert(e.message)}}};let del=document.createElement('button');del.className='danger';del.textContent='Delete';del.onclick=async()=>{if(confirm(`Delete fingerprint ID ${o.id}?`)){try{await api('/api/operator/delete','POST',{id:o.id});await loadOperators();await refreshStatus()}catch(e){alert(e.message)}}};d.append(label,edit,del);$('operators').append(d)})}catch(e){$('operators').textContent=e.message}}
$('enrollForm').onsubmit=async e=>{e.preventDefault();try{let j=await api('/api/enroll/start','POST',{id:$('opid').value,name:$('opname').value.trim()});$('enrollStatus').textContent=j.message;$('cancelEnroll').hidden=false;pollEnroll()}catch(e){alert(e.message)}};
let poller=null;async function pollEnroll(){if(poller)clearInterval(poller);poller=setInterval(async()=>{try{let j=await api('/api/enroll/status');$('enrollStatus').textContent=j.state+' — '+j.message;if(j.state==='Completed'||j.state==='Failed'||j.state==='Idle'){clearInterval(poller);poller=null;$('cancelEnroll').hidden=true;loadOperators();refreshStatus()}}catch(e){$('enrollStatus').textContent=e.message;clearInterval(poller);poller=null}},900)}
async function cancelEnrollment(){try{await api('/api/enroll/cancel','POST',{});$('enrollStatus').textContent='Cancelled';$('cancelEnroll').hidden=true;if(poller){clearInterval(poller);poller=null}}catch(e){alert(e.message)}}
refreshStatus();loadOperators();</script></body></html>
)PC200HTML";

void registerAdminRoutes() {
  adminServer.on("/", HTTP_GET, []() {
    if (!requireAdminAuth()) return;
    adminServer.send_P(200, "text/html", ADMIN_HTML);
  });

  adminServer.on("/api/status", HTTP_GET, []() {
    if (!requireAdminAuth()) return;
    if (fingerOK) finger.getTemplateCount();
    String json = "{\"ok\":true,\"admin\":true,\"fingerprint\":" + String(fingerOK ? "true" : "false") +
      ",\"templates\":" + String(fingerOK ? finger.templateCount : 0) +
      ",\"ip\":\"" + WiFi.softAPIP().toString() + "\"}";
    adminServer.send(200, "application/json", json);
  });

  adminServer.on("/api/operators", HTTP_GET, []() {
    if (!requireAdminAuth()) return;
    String json = "{\"ok\":true,\"operators\":[";
    bool first = true;
    if (preferencesOK) {
      for (uint16_t id = 1; id <= MAX_FINGERPRINT_ID; id++) {
        String name = operatorPrefs.getString(operatorKey(id).c_str(), "");
        if (!name.length()) continue;
        if (!first) json += ",";
        first = false;
        json += "{\"id\":" + String(id) + ",\"name\":\"" + jsonEscape(name) + "\"}";
      }
    }
    json += "]}";
    adminServer.send(200, "application/json", json);
  });

  adminServer.on("/api/enroll/start", HTTP_POST, []() {
    if (!requireAdminAuth()) return;
    uint16_t id = (uint16_t)adminServer.arg("id").toInt();
    String name = adminServer.arg("name"); name.trim();
    if (!fingerOK || !preferencesOK) { adminServer.send(503, "application/json", "{\"ok\":false,\"error\":\"Fingerprint sensor or NVS unavailable\"}"); return; }
    if (id < FIRST_NEW_OPERATOR_ID || id > MAX_FINGERPRINT_ID) { adminServer.send(400, "application/json", "{\"ok\":false,\"error\":\"Use an ID from 4 to 127\"}"); return; }
    if (!name.length() || name.length() > 20) { adminServer.send(400, "application/json", "{\"ok\":false,\"error\":\"Name must contain 1–20 characters\"}"); return; }
    if (enrollState == ENROLL_WAIT_FIRST || enrollState == ENROLL_WAIT_REMOVE || enrollState == ENROLL_WAIT_SECOND) { adminServer.send(409, "application/json", "{\"ok\":false,\"error\":\"Enrollment already running\"}"); return; }
    if (operatorPrefs.getString(operatorKey(id).c_str(), "").length()) { adminServer.send(409, "application/json", "{\"ok\":false,\"error\":\"ID already has operator metadata; delete it first\"}"); return; }
    enrollTargetId = id; enrollTargetName = name; enrollStartedAt = millis(); enrollLastPoll = 0;
    enrollState = ENROLL_WAIT_FIRST; enrollMessage = "Place finger (1/2)";
    adminServer.send(200, "application/json", "{\"ok\":true,\"message\":\"Enrollment started\"}");
    Serial.printf("[ADMIN][ENROLL] Started ID %u\n", (unsigned)id);
  });

  adminServer.on("/api/enroll/status", HTTP_GET, []() {
    if (!requireAdminAuth()) return;
    String json = "{\"ok\":true,\"state\":\"" + String(enrollStateText()) + "\",\"message\":\"" + jsonEscape(enrollMessage) + "\"}";
    adminServer.send(200, "application/json", json);
  });

  adminServer.on("/api/enroll/cancel", HTTP_POST, []() {
    if (!requireAdminAuth()) return;
    if (enrollState == ENROLL_WAIT_FIRST || enrollState == ENROLL_WAIT_REMOVE || enrollState == ENROLL_WAIT_SECOND) {
      enrollState = ENROLL_IDLE; enrollMessage = "Cancelled";
    }
    adminServer.send(200, "application/json", "{\"ok\":true}");
  });

  adminServer.on("/api/operator/rename", HTTP_POST, []() {
    if (!requireAdminAuth()) return;
    uint16_t id = (uint16_t)adminServer.arg("id").toInt();
    String name = adminServer.arg("name"); name.trim();
    if (!preferencesOK || id < 1 || id > MAX_FINGERPRINT_ID || !operatorPrefs.getString(operatorKey(id).c_str(), "").length()) {
      adminServer.send(404, "application/json", "{\"ok\":false,\"error\":\"Operator not found\"}"); return;
    }
    if (!name.length() || name.length() > 20) { adminServer.send(400, "application/json", "{\"ok\":false,\"error\":\"Name must contain 1–20 characters\"}"); return; }
    operatorPrefs.putString(operatorKey(id).c_str(), name);
    adminServer.send(200, "application/json", "{\"ok\":true}");
  });

  adminServer.on("/api/operator/delete", HTTP_POST, []() {
    if (!requireAdminAuth()) return;
    uint16_t id = (uint16_t)adminServer.arg("id").toInt();
    if (!fingerOK || !preferencesOK || id < 1 || id > MAX_FINGERPRINT_ID) { adminServer.send(400, "application/json", "{\"ok\":false,\"error\":\"Invalid ID or device unavailable\"}"); return; }
    uint8_t result = finger.deleteModel(id);
    if (result != FINGERPRINT_OK) {
      adminServer.send(409, "application/json", (String("{\"ok\":false,\"error\":\"Sensor delete failed: ") + result + "\"}").c_str()); return;
    }
    operatorPrefs.remove(operatorKey(id).c_str());
    adminServer.send(200, "application/json", "{\"ok\":true}");
    Serial.printf("[ADMIN] Deleted fingerprint ID %u\n", (unsigned)id);
  });

  adminServer.onNotFound([]() {
    if (!requireAdminAuth()) return;
    adminServer.send(404, "application/json", "{\"ok\":false,\"error\":\"Not found\"}");
  });
}

bool startAdminAccessPoint() {
  if (!fingerOK) Serial.println("[ADMIN] WARNING: fingerprint sensor is not ready");
  WiFi.mode(WIFI_AP);
  IPAddress localIP(192, 168, 4, 1);
  IPAddress gateway(192, 168, 4, 1);
  IPAddress subnet(255, 255, 255, 0);
  if (!WiFi.softAPConfig(localIP, gateway, subnet)) {
    Serial.println("[ADMIN] SoftAP IP configuration failed");
    WiFi.mode(WIFI_OFF);
    return false;
  }
  if (!WiFi.softAP(ADMIN_AP_SSID, ADMIN_AP_PASSWORD, 6, false, 2)) {
    Serial.println("[ADMIN] SoftAP start failed");
    WiFi.mode(WIFI_OFF);
    return false;
  }
  if (!adminRoutesRegistered) {
    registerAdminRoutes();
    adminRoutesRegistered = true;
  }
  adminServer.begin();
  adminServerStarted = true;
  adminLastActivity = millis();
  Serial.println("[ADMIN] SoftAP started");
  Serial.print("[ADMIN] SSID: "); Serial.println(ADMIN_AP_SSID);
  Serial.print("[ADMIN] IP: "); Serial.println(WiFi.softAPIP());
  return true;
}

void stopAdminAccessPoint() {
  if (enrollState == ENROLL_WAIT_FIRST || enrollState == ENROLL_WAIT_REMOVE || enrollState == ENROLL_WAIT_SECOND) {
    enrollState = ENROLL_IDLE;
    enrollMessage = "Cancelled because Admin Mode closed";
  }
  if (adminServerStarted) { adminServer.stop(); adminServerStarted = false; }
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_OFF);
  adminModeActive = false;
  Serial.println("[ADMIN] SoftAP stopped; Admin Mode inactive");
}

void updateAdminMode() {
  if (!adminModeActive) return;
  if (adminServerStarted) adminServer.handleClient();

  static EnrollState lastRenderedEnrollState = ENROLL_IDLE;
  static String lastRenderedEnrollMessage = "Idle";
  static EnrollState previousEnrollState = ENROLL_IDLE;
  static unsigned long terminalStateStartedAt = 0;
  const unsigned long ENROLL_RESULT_DISPLAY_DURATION = 4000UL;

  updateEnrollment();

  // Pertahankan layar hasil (success/fail) selama 4 detik,
  // lalu kembalikan state dan OLED ke tampilan Admin Mode idle.
  if (enrollState == ENROLL_DONE || enrollState == ENROLL_FAILED) {
    if (previousEnrollState != enrollState || terminalStateStartedAt == 0) {
      terminalStateStartedAt = millis();
    }

    if (millis() - terminalStateStartedAt >= ENROLL_RESULT_DISPLAY_DURATION) {
      Serial.println("[ADMIN][ENROLL] Result display timeout; returning to idle");
      enrollState = ENROLL_IDLE;
      enrollMessage = "Idle";
      terminalStateStartedAt = 0;
    }
  } else {
    terminalStateStartedAt = 0;
  }

  previousEnrollState = enrollState;

  if (
    currentScreen == SCREEN_ADMIN_B &&
    (lastRenderedEnrollState != enrollState || lastRenderedEnrollMessage != enrollMessage)
  ) {
    lastRenderedEnrollState = enrollState;
    lastRenderedEnrollMessage = enrollMessage;
    screenDirty = true;
  }

  if (millis() - adminLastActivity >= ADMIN_IDLE_TIMEOUT) {
    Serial.println("[ADMIN] Inactivity timeout; closing Admin Mode");
    stopAdminAccessPoint();
    slideAnimation(currentScreen, SCREEN_MAIN_A, true);
    menuIndex = 0;
  }
}



unsigned long deniedScreenStarted = 0;
const unsigned long DENIED_SCREEN_DURATION = 3000;


// ================= DATE / TEXT HELPERS ===================

const char* monthShort(uint8_t month) {
  static const char* months[] = {
    "JAN", "FEB", "MAR", "APR", "MAY", "JUN",
    "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"
  };
  if (month < 1 || month > 12) return "---";
  return months[month - 1];
}

const char* monthLong(uint8_t month) {
  static const char* months[] = {
    "JANUARY", "FEBRUARY", "MARCH", "APRIL",
    "MAY", "JUNE", "JULY", "AUGUST",
    "SEPTEMBER", "OCTOBER", "NOVEMBER", "DECEMBER"
  };
  if (month < 1 || month > 12) return "UNKNOWN";
  return months[month - 1];
}

void drawCenteredText(GFXcanvas1 &c, const char* text, int16_t topY, uint8_t textSize = 1) {
  c.setTextSize(textSize);
  c.setTextColor(SSD1306_WHITE);

  int16_t x1, y1;
  uint16_t w, h;
  c.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);

  int16_t x = (SCREEN_WIDTH - (int16_t)w) / 2 - x1;
  int16_t y = topY - y1;

  c.setCursor(x, y);
  c.print(text);
}

void drawCenteredTextAtY(GFXcanvas1 &c, const char* text, int16_t centerY, uint8_t textSize = 1) {
  c.setTextSize(textSize);
  int16_t x1, y1;
  uint16_t w, h;
  c.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  int16_t topY = centerY - (int16_t)h / 2;
  drawCenteredText(c, text, topY, textSize);
}

void drawCenteredHeader(GFXcanvas1 &c, const char* text) {
  drawCenteredText(c, text, 2, 1);
}

void makeHeaderDateTime(char* buffer, size_t length) {
  if (!rtcOK) {
    snprintf(buffer, length, "RTC NOT AVAILABLE");
    return;
  }

  DateTime now = rtc.now();
  snprintf(buffer, length, "%02u %s %04u %02u:%02u",
    (unsigned)now.day(), monthShort(now.month()), (unsigned)now.year(),
    (unsigned)now.hour(), (unsigned)now.minute()
  );
}

void makeClockDate(char* buffer, size_t length) {
  if (!rtcOK) {
    snprintf(buffer, length, "RTC NOT AVAILABLE");
    return;
  }

  DateTime now = rtc.now();
  snprintf(buffer, length, "%02u %s %04u",
    (unsigned)now.day(), monthLong(now.month()), (unsigned)now.year()
  );
}

// ================= SCREEN RENDERING ======================

void drawMainScreenA(GFXcanvas1 &c) {
  char header[32];
  makeHeaderDateTime(header, sizeof(header));
  drawCenteredHeader(c, header);

  drawCenteredTextAtY(c, "IGNITION STATUS : ON", 31, 1);
  drawCenteredTextAtY(c, "PLACE YOUR FINGER", 45, 1);
}

void drawMainScreenB(GFXcanvas1 &c) {
  char header[32];
  makeHeaderDateTime(header, sizeof(header));
  drawCenteredHeader(c, header);

  const char* line1 = sessionActive ? activeOperatorName.c_str() : "UNKNOWN FINGER";
  const char* line2 = sessionActive ? "ACCESS GRANTED" : "ACCESS DENIED";

  drawCenteredTextAtY(c, line1, 31, 1);
  drawCenteredTextAtY(c, line2, 45, 1);
}

void drawClockScreenA(GFXcanvas1 &c) {
  c.drawBitmap((SCREEN_WIDTH - ICON_WIDTH) / 2, 16, clock_icon, ICON_WIDTH, ICON_HEIGHT, SSD1306_WHITE);
  drawCenteredTextAtY(c, "SYSTEM CLOCK", 45, 1);
}

void drawClockScreenB(GFXcanvas1 &c) {
  drawCenteredHeader(c, "SYSTEM CLOCK");

  if (!rtcOK) {
    drawCenteredTextAtY(c, "RTC NOT AVAILABLE", 38, 1);
    return;
  }

  DateTime now = rtc.now();
  char timeBuffer[12];
  char dateBuffer[28];

  snprintf(timeBuffer, sizeof(timeBuffer), "%02u:%02u:%02u", (unsigned)now.hour(), (unsigned)now.minute(), (unsigned)now.second());
  makeClockDate(dateBuffer, sizeof(dateBuffer));

  drawCenteredTextAtY(c, timeBuffer, 32, 2);
  drawCenteredTextAtY(c, dateBuffer, 49, 1);
}

void drawInformationScreenA(GFXcanvas1 &c) {
  c.drawBitmap((SCREEN_WIDTH - INFO_ICON_WIDTH) / 2, 16, information_icon, INFO_ICON_WIDTH, INFO_ICON_HEIGHT, SSD1306_WHITE);
  drawCenteredTextAtY(c, "INFORMATION", 45, 1);
}

void drawInformationScreenB(GFXcanvas1 &c) {
  drawCenteredHeader(c, "INFORMATION");
  drawCenteredTextAtY(c, "ATOMIC FIRMWARE", 25, 1);
  drawCenteredTextAtY(c, "V1.7.5", 39, 1);
  drawCenteredTextAtY(c, "192.168.4.1", 53, 1);
}

void drawAdminScreenA(GFXcanvas1 &c) {
  c.drawBitmap((SCREEN_WIDTH - ICON_WIDTH) / 2, 16, shield_icon, ICON_WIDTH, ICON_HEIGHT, SSD1306_WHITE);
  drawCenteredTextAtY(c, "ADMIN MODE", 45, 1);
}

void drawAdminScreenB(GFXcanvas1 &c) {
  drawCenteredHeader(c, "ADMIN MODE");

  // Tampilan idle dipertahankan persis seperti V1.7.
  if (enrollState == ENROLL_IDLE) {
    c.drawBitmap((SCREEN_WIDTH - ICON_WIDTH) / 2, 19, shield_icon, ICON_WIDTH, ICON_HEIGHT, SSD1306_WHITE);
    drawCenteredTextAtY(c, "ADMIN SESSION", 47, 1);
    drawCenteredTextAtY(c, "IS RUNNING!", 57, 1);
    return;
  }

  const uint8_t* statusIcon = fingerprint_icon;
  if (enrollState == ENROLL_FAILED) statusIcon = fail_icon;
  else if (enrollState == ENROLL_DONE) statusIcon = success_icon;

  c.drawBitmap((SCREEN_WIDTH - ICON_WIDTH) / 2, 13, statusIcon, ICON_WIDTH, ICON_HEIGHT, SSD1306_WHITE);

  // Pesan dinamis yang sama dengan field "message" pada Web Admin.
  String remaining = enrollMessage;
  const uint8_t maxCharsPerLine = 21;
  const int16_t lineCenters[] = {42, 51, 60};

  for (uint8_t line = 0; line < 3 && remaining.length(); line++) {
    remaining.trim();
    if (!remaining.length()) break;

    String currentLine;
    if (remaining.length() <= maxCharsPerLine) {
      currentLine = remaining;
      remaining = "";
    } else {
      int splitAt = remaining.lastIndexOf(' ', maxCharsPerLine);
      if (splitAt <= 0) splitAt = maxCharsPerLine;
      currentLine = remaining.substring(0, splitAt);
      remaining = remaining.substring(splitAt);
      remaining.trim();
    }

    if (line == 2 && remaining.length()) {
      while (currentLine.length() > 18) currentLine.remove(currentLine.length() - 1);
      currentLine += "...";
      remaining = "";
    }

    drawCenteredTextAtY(c, currentLine.c_str(), lineCenters[line], 1);
  }
}

void drawLogoScreen(GFXcanvas1 &c) {
  c.drawBitmap(0, 0, logo_bitmap, LOGO_WIDTH, LOGO_HEIGHT, SSD1306_WHITE);
}

void drawScreen(GFXcanvas1 &c, ScreenId screen) {
  c.fillScreen(SSD1306_BLACK);
  c.setTextColor(SSD1306_WHITE);
  c.setTextSize(1);

  switch (screen) {
    case SCREEN_MAIN_A:  drawMainScreenA(c); break;
    case SCREEN_MAIN_B:  drawMainScreenB(c); break;
    case SCREEN_CLOCK_A: drawClockScreenA(c); break;
    case SCREEN_CLOCK_B: drawClockScreenB(c); break;
    case SCREEN_ADMIN_A: drawAdminScreenA(c); break;
    case SCREEN_ADMIN_B: drawAdminScreenB(c); break;
    case SCREEN_INFORMATION_A: drawInformationScreenA(c); break;
    case SCREEN_INFORMATION_B: drawInformationScreenB(c); break;
    case SCREEN_LOGO:    drawLogoScreen(c); break;
  }
}

void showCurrentScreen() {
  if (!oledOK) return;

  drawScreen(canvasNew, currentScreen);
  display.clearDisplay();
  display.drawBitmap(0, 0, canvasNew.getBuffer(), SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);
  display.display();
  screenDirty = false;
}

// ================= V1.4 SCROLL ANIMATION =================

void slideAnimation(ScreenId from, ScreenId to, bool reverse) {
  if (!oledOK) {
    currentScreen = to;
    screenDirty = true;
    return;
  }

  drawScreen(canvasOld, from);
  drawScreen(canvasNew, to);

  for (int offset = 0; offset <= SCREEN_HEIGHT; offset += 4) {
    display.clearDisplay();

    if (!reverse) {
      display.drawBitmap(0, -offset, canvasOld.getBuffer(), SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);
      display.drawBitmap(0, SCREEN_HEIGHT - offset, canvasNew.getBuffer(), SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);
    } else {
      display.drawBitmap(0, offset, canvasOld.getBuffer(), SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);
      display.drawBitmap(0, offset - SCREEN_HEIGHT, canvasNew.getBuffer(), SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);
    }

    display.display();
    delay(15);
  }

  currentScreen = to;
  screenDirty = false;
}

// ================= MENU NAVIGATION ======================

int8_t findMenuIndex(ScreenId screen) {
  for (uint8_t i = 0; i < MENU_COUNT; i++) {
    if (menuScreens[i] == screen) return (int8_t)i;
  }
  return -1;
}

void navigateMenu(int8_t direction) {
  int8_t currentIndex = findMenuIndex(currentScreen);
  if (currentIndex < 0) return;

  int8_t newIndex = currentIndex + direction;

  if (newIndex < 0) newIndex = MENU_COUNT - 1;
  if (newIndex >= MENU_COUNT) newIndex = 0;

  if (newIndex == currentIndex) return;

  ScreenId target = menuScreens[newIndex];
  slideAnimation(currentScreen, target, direction < 0);
  menuIndex = newIndex;

  Serial.print("MENU INDEX: ");
  Serial.println(menuIndex);
}

void returnToMainMenu(bool reverse = true) {
  if (adminModeActive) stopAdminAccessPoint();

  if (currentScreen != SCREEN_MAIN_A) {
    slideAnimation(currentScreen, SCREEN_MAIN_A, reverse);
  } else {
    screenDirty = true;
  }

  menuIndex = 0;
}

// ================= SCREEN ACTIONS ========================


void handleOK() {
  Serial.println("BUTTON OK");

  switch (currentScreen) {
    case SCREEN_CLOCK_A:
      slideAnimation(SCREEN_CLOCK_A, SCREEN_CLOCK_B, false);
      break;

    case SCREEN_ADMIN_A:
      if (startAdminAccessPoint()) {
        adminModeActive = true;
        Serial.println("ADMIN MODE ACTIVE");
        slideAnimation(SCREEN_ADMIN_A, SCREEN_ADMIN_B, false);
      } else {
        Serial.println("[ADMIN] Failed to start access point");
      }
      break;

    case SCREEN_INFORMATION_A:
      slideAnimation(SCREEN_INFORMATION_A, SCREEN_INFORMATION_B, false);
      break;

    case SCREEN_LOGO:
    case SCREEN_MAIN_A:
      // Tidak ada bunyi saat tombol OK ditekan.
      break;

    case SCREEN_CLOCK_B:
    case SCREEN_ADMIN_B:
    case SCREEN_MAIN_B:
      break;
  }
}


void handleBack() {
  Serial.println("BUTTON BACK");

  switch (currentScreen) {
    case SCREEN_MAIN_B:
      if (sessionActive) Serial.println("SESSION CLOSED BY USER");
      sessionActive = false;
      activeOperatorName = "";
      returnToMainMenu(true);
      break;
    case SCREEN_CLOCK_A:
    case SCREEN_CLOCK_B:
    case SCREEN_ADMIN_A:
    case SCREEN_ADMIN_B:
    case SCREEN_INFORMATION_A:
    case SCREEN_LOGO:
      if (currentScreen == SCREEN_ADMIN_B || currentScreen == SCREEN_ADMIN_A) {
        if (adminModeActive) stopAdminAccessPoint();
        Serial.println("ADMIN MODE INACTIVE");
      }
      returnToMainMenu(true);
      break;
    case SCREEN_INFORMATION_B:
      slideAnimation(SCREEN_INFORMATION_B, SCREEN_INFORMATION_A, true);
      break;
    case SCREEN_MAIN_A:
      break;
  }
}

// ================= BUTTON DETECTION ======================

bool pressed(int pin, bool &lastState) {
  bool now = digitalRead(pin);

  if (lastState == HIGH && now == LOW && millis() - lastDebounce > DEBOUNCE_DELAY) {
    lastDebounce = millis();
    lastState = now;
    return true;
  }
  lastState = now;
  return false;
}

void updateButtons() {
  if (bootSplashActive) return;

  if (pressed(BTN_UP, lastUp)) { Serial.println("BUTTON UP"); navigateMenu(-1); }
  if (pressed(BTN_DOWN, lastDown)) { Serial.println("BUTTON DOWN"); navigateMenu(1); }
  if (pressed(BTN_OK, lastOK)) handleOK();
  if (pressed(BTN_BACK, lastBack)) handleBack();
}

// ================= FINGERPRINT SCANNER ===================

void showAuthenticationResult(bool granted) {
  if (granted) {
    sessionActive = true;
    deniedScreenStarted = 0;
    ledGranted();
    startBeep(true);

    Serial.println("[AUTH] ACCESS GRANTED");
    Serial.print("[AUTH] ID: ");
    Serial.println(matchedID);
    Serial.print("[AUTH] OPERATOR: ");
    Serial.println(activeOperatorName);
    Serial.print("[AUTH] CONFIDENCE: ");
    Serial.println(confidence);

  } else {
    sessionActive = false;
    activeOperatorName = "";

    ledDenied();
    startBeep(false);

    Serial.println("[AUTH] ACCESS DENIED");
  }

  // Jalankan animasi sampai selesai
  slideAnimation(SCREEN_MAIN_A, SCREEN_MAIN_B, false);

  // Mulai timer setelah animasi selesai
  if (!granted) {
    deniedScreenStarted = millis();
    Serial.println("[AUTH] Denied screen timer started");
  }
}

void updateFingerprint() {
  if (!fingerOK) {
    ledSystemFailure();
    return;
  }

  if (currentScreen != SCREEN_MAIN_A || adminModeActive) return;

  unsigned long now = millis();
  if (now - lastFingerprintScan < FINGERPRINT_INTERVAL) return;

  lastFingerprintScan = now;

  uint32_t t0 = millis();
  uint8_t p = finger.getImage();
  uint32_t elapsed = millis() - t0;

  if (p != FINGERPRINT_NOFINGER) {
    Serial.printf("[FP] getImage=%lu ms | status=%u\n", (unsigned long)elapsed, (unsigned int)p);
  }

  if (p == FINGERPRINT_NOFINGER) {
    if (waitingFingerRemoval) {
      waitingFingerRemoval = false;
      Serial.println("[FP] Finger removed; scanner ready");
      ledWaiting();
    }
    return;
  }

  if (waitingFingerRemoval) return;

  if (p != FINGERPRINT_OK) {
    Serial.print("[FP] GET IMAGE ERROR: "); Serial.println(p);
    waitingFingerRemoval = true;
    ledSystemFailure();
    startBeep(false);
    return;
  }

  Serial.println("[FP] Fingerprint captured");

  t0 = millis();
  p = finger.image2Tz();
  Serial.printf("[FP] image2Tz=%lu ms | status=%u\n", (unsigned long)(millis() - t0), (unsigned int)p);

  if (p != FINGERPRINT_OK) {
    Serial.print("[FP] IMAGE2TZ ERROR: "); Serial.println(p);
    waitingFingerRemoval = true;
    ledDenied();
    startBeep(false);
    return;
  }

  t0 = millis();
  p = finger.fingerFastSearch();
  Serial.printf("[FP] fingerFastSearch=%lu ms | status=%u\n", (unsigned long)(millis() - t0), (unsigned int)p);

  waitingFingerRemoval = true;

  if (p == FINGERPRINT_OK) {
    matchedID = finger.fingerID;
    confidence = finger.confidence;
    activeOperatorName = getOperatorName(matchedID);
    showAuthenticationResult(true);
  } else {
    if (p != FINGERPRINT_NOTFOUND) {
      Serial.print("[FP] SEARCH ERROR: "); Serial.println(p);
    }
    showAuthenticationResult(false);
  }
}

// ================= DEVICE INITIALIZATION =================

void initializeDevices() {
  Serial.println("Initializing peripherals...");

  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.setClock(100000);

  oledOK = display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS);
  Serial.println(oledOK ? "OLED OK" : "OLED ERROR");

  rtcOK = rtc.begin();
  Serial.println(rtcOK ? "RTC OK" : "RTC ERROR");

  if (rtcOK && rtc.lostPower()) {
    Serial.println("WARNING: RTC lost power; set clock if required.");
  }

  FPSerial.begin(57600, SERIAL_8N1, FP_RX, FP_TX);

  finger.begin(57600);
  fingerOK = finger.verifyPassword();
  Serial.println(fingerOK ? "FINGERPRINT OK" : "FINGERPRINT ERROR");
}

// ================= SETUP =================================

void setup() {
  Serial.begin(115200);
  delay(300);

  Serial.println();
  Serial.println("========================================");
  Serial.println("A.T.O.M. - Access, Tracking & Operation Monitoring");
  Serial.println("ADMIN MODE V1.7.5");
  Serial.println("========================================");

  rgbLed.begin();
  rgbLed.setBrightness(40);
  rgbLed.clear();
  rgbLed.show();

  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_OK, INPUT_PULLUP);
  pinMode(BTN_BACK, INPUT_PULLUP);

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, BUZZER_OFF);

  initializeDevices();

  preferencesOK = operatorPrefs.begin("pc200ops", false);
  Serial.println(preferencesOK ? "OPERATOR NVS OK" : "OPERATOR NVS ERROR");
  if (preferencesOK) seedLegacyOperatorNames();

  if (fingerOK) ledWaiting();
  else ledSystemFailure();

  Serial.println("----------------------------------------");
  Serial.println("SYSTEM READY");
  Serial.println("BOOT LOGO");
  Serial.println("----------------------------------------");

  currentScreen = SCREEN_LOGO;
  bootSplashStarted = millis();

  if (oledOK) showCurrentScreen();
}

// ================= MAIN LOOP =============================

void loop() {
  unsigned long now = millis();

  if (bootSplashActive && now - bootSplashStarted >= BOOT_SPLASH_DURATION) {
    bootSplashActive = false;
    currentScreen = SCREEN_MAIN_A;
    menuIndex = 0;
    screenDirty = true;
    Serial.println("[UI] MAIN MENU");
  }

  updateButtons();
  if (adminModeActive) updateAdminMode();
  else updateFingerprint();
  updateBuzzer();

  if (
    currentScreen == SCREEN_MAIN_B &&
    !sessionActive &&
    deniedScreenStarted != 0 &&
    millis() - deniedScreenStarted >= DENIED_SCREEN_DURATION
  ) {
    deniedScreenStarted = 0;
    Serial.println("[AUTH] Denied screen timeout");
    slideAnimation(SCREEN_MAIN_B, SCREEN_MAIN_A, true);
    menuIndex = 0;
  }

  static unsigned long lastClockRefresh = 0;
  if (
    (currentScreen == SCREEN_MAIN_A || currentScreen == SCREEN_MAIN_B || currentScreen == SCREEN_CLOCK_B) &&
    now - lastClockRefresh >= 1000
  ) {
    lastClockRefresh = now;
    screenDirty = true;
  }

  if (screenDirty) showCurrentScreen();
}
