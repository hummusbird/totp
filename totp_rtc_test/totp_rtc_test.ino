#include <I2C_RTC.h>
#include "TOTP.h"
#include <U8g2lib.h>
#include <Wire.h>

// RTC PREP
static DS1307 RTC;

// OLED PREP
U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ A3, /* data=*/ A2, /* reset=*/ U8X8_PIN_NONE);

void u8g2_prepare() {
  u8g2.setFont(u8g2_font_profont11_tf);
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
}

// TOTP PREP
uint8_t hmacKey[] = "12345678901234567890";
TOTP totp = TOTP(hmacKey, 20, 30);
char code[7];

void setup() {
  RTC.begin();
  Serial.begin(9600);

  u8g2.begin();
  u8g2_prepare();
}

void loop() {
  long TIME = RTC.getEpoch() - 3390;  // offset to adjust for BST timezone
  char* newCode = totp.getCode(TIME);

  Serial.print("UNIX: ");
  Serial.println(TIME);
  Serial.print("CODE: ");
  Serial.println(newCode);
  Serial.println("=======================");

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_profont11_tf);
  u8g2.drawStr(0, 0, "2FA CODE:");

  u8g2.drawStr(113, 10, String(30 - (int)(TIME % 30)).c_str());

  u8g2.setFont(u8g2_font_fur20_tn);
  u8g2.drawStr(0, 10, newCode);
  u8g2.sendBuffer();

  delay(1000);
}
