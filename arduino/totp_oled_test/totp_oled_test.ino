#include "TOTP.h"
#include "swRTC.h"
#include <U8g2lib.h>
#include <Wire.h>

// OLED PREP
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

void u8g2_prepare() {
  u8g2.setFont(u8g2_font_profont11_tf);
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  //u8g2.setFontDirection(0);
}

// TOTP PREP
uint8_t hmacKey[] = "12345678901234567890";
TOTP totp = TOTP(hmacKey, 20, 30);
swRTC rtc;
char code[7];


void setup() {
  u8g2.begin();
  u8g2_prepare();

  Serial.begin(9600);

  rtc.stopRTC();
  rtc.setDate(16, 10, 2023);
  rtc.setTime(21, 54, 0);
  rtc.startRTC();

  u8g2.clearBuffer();
  u8g2.drawStr(0, 0, "INIT");
  u8g2.sendBuffer();
}

void loop() {
  long GMT = rtc.getTimestamp();
  char* newCode = totp.getCode(GMT);

  Serial.print("UNIX: ");
  Serial.println(GMT);
  Serial.print("CODE: ");
  Serial.println(newCode);
  Serial.println("=======================");

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_profont11_tf);
  u8g2.drawStr(0, 0, "2FA CODE:");

  u8g2.drawStr(113, 10, String(30-(int)(GMT % 30)).c_str());

  u8g2.setFont(u8g2_font_fur20_tn);
  u8g2.drawStr(0, 10, newCode);
  u8g2.sendBuffer();

  delay(500);
}
