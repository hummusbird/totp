#include <I2C_RTC.h>
#include "TOTP.h"
#include <U8g2lib.h>
#include <Wire.h>

// BUTTON PREP
const int buttonPin = 7;
int pageNum = 0;
int buttonState;
int lastState;

// RTC PREP
static DS1307 RTC;

// OLED PREP
// some OLEDs do not like working with other devices on the i2c bus, so we have to connect the display to pins A2 and A3 and run software i2c instead.
U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, /* clock=*/A3, /* data=*/A2, /* reset=*/U8X8_PIN_NONE);
int updateDisplay;

void u8g2_prepare() {
  u8g2.setFont(u8g2_font_profont11_tf);
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
}

// TOTP PREP
TOTP totp = TOTP("12345678901234567890", 20, 30);
char code[7];

void setup() {
  RTC.begin();
  Serial.begin(9600);
  pinMode(buttonPin, INPUT);
  u8g2.begin();
  u8g2_prepare();
}

void loop() {
  int buttonState = digitalRead(buttonPin);

  if (buttonState == HIGH && lastState != buttonState) { // update display if the button has been pressed
    pageNum++;
    updateDisplay = 1;
    if (pageNum > 5) { pageNum = 0; }
  }

  lastState = buttonState;

  long TIME = RTC.getEpoch() - 3390;  // offset to adjust for BST timezone
  char* newCode = totp.getCode(TIME);

  if(strcmp(code, newCode) != 0) { // update display if there's a new TOTP code
    strcpy(code, newCode);
    updateDisplay = 1;
  }  

  // Serial.print("UNIX: ");
  // Serial.println(TIME);
  // Serial.print("CODE: ");
  // Serial.println(newCode);
  // Serial.println("=======================");

  // u8g2.drawStr(115, 10, String(30 - (int)(TIME % 30)).c_str());

  if (updateDisplay == 1) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_profont11_tf);

    char pages[2];
    sprintf(pages, "%d", pageNum);
    u8g2.drawStr(115, 20, pages);

    u8g2.drawStr(0, 0, "2FA CODE:");
    u8g2.setFont(u8g2_font_fur20_tn);
    u8g2.drawStr(0, 10, newCode);
    u8g2.sendBuffer();
  }

  updateDisplay = 0;
}
