#include <I2C_RTC.h>
#include "TOTP.h"
#include <U8g2lib.h>
#include <Wire.h>

// RTC PREP
static DS1307 RTC;

// OLED PREP
// some OLEDs do not like working with other devices on the i2c bus, so we have to connect the display to pins A2 and A3 and run software i2c instead.
U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, /* clock=*/A3, /* data=*/A2, /* reset=*/U8X8_PIN_NONE);

void u8g2_prepare() {
  u8g2.setFont(u8g2_font_profont11_tf);
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
}

void drawCountdownBars(long TIME) {
  if (30 - (int)(TIME % 30) > 22) { u8g2.drawBox(110, 17, 18, 3); };
  if (30 - (int)(TIME % 30) > 15) { u8g2.drawBox(110, 23, 18, 3); };
  if (30 - (int)(TIME % 30) > 7) { u8g2.drawBox(110, 29, 18, 3); };
}

// TOTP PREP
static const int totp_count = 6;
char totp_names[totp_count][10] = { "12345",
                                    "09876",
                                    "555555",
                                    "666666",
                                    "777777",
                                    "ABCDEF" };

TOTP totps[totp_count] = { TOTP("12345678901234567890", 20, 30),
                           TOTP("09876543210987654321", 20, 30),
                           TOTP("55555555555555555555", 20, 30),
                           TOTP("66666666666666666666", 20, 30),
                           TOTP("77777777777777777777", 20, 30),
                           TOTP("ABCDEFGHIJABCDEFGHIJ", 20, 30) };
char code[7];

// BUTTON PREP
const int CWPin = 3;
const int CCWPin = 2;
int pageNum = 0;
int buttonState;  // current button state
int lastState;    // last button state

long lastMillis;

void setup() {
  RTC.begin();
  Serial.begin(9600);
  pinMode(CWPin, INPUT_PULLUP);
  pinMode(CCWPin, INPUT_PULLUP);
  u8g2.begin();
  u8g2_prepare();
}

void loop() {
  int updateDisplay = 0;
  int updateBar = 0;
  int buttonState = digitalRead(CWPin);

  if (buttonState == HIGH && lastState != buttonState) {  // update display if the button has been pressed
    if (digitalRead(CCWPin) == HIGH) {
      pageNum--;
    } else {
      pageNum++;
    }

    updateDisplay = 1;
    if (pageNum > totp_count - 1) { pageNum = 0; }
    if (pageNum < 0) { pageNum = totp_count - 1; }
  }

  lastState = buttonState;

  long TIME = RTC.getEpoch() - 3390;  // offset to adjust for BST timezone
  char* newCode = totps[pageNum].getCode(TIME);

  if (strcmp(code, newCode) != 0) {  // update display if there's a new TOTP code
    strcpy(code, newCode);
    updateDisplay = 1;
  }

  int barInt = 30 - (int)(TIME % 30);
  if (barInt == 22 || barInt == 15 || barInt == 7) {  // update display if new countdown bar should be shown
    updateBar = 1;
  }

  // Serial.print("UNIX: ");
  // Serial.println(TIME);
  // Serial.print("CODE: ");
  // Serial.println(newCode);
  // Serial.println("=======================");

  // u8g2.drawStr(115, 10, String(30 - (int)(TIME % 30)).c_str());

  if (updateDisplay == 1) {
    u8g2.clearBuffer();
    u8g2.setFontMode(1);
    u8g2.setFont(u8g2_font_spleen16x32_mn);  // this font is only digits, to save on memory
    u8g2.drawStr(0, 5, newCode);             // draw large TOTP code

    u8g2.setFont(u8g2_font_profont11_tf);
    char pages[6];
    snprintf(pages, sizeof(pages), "%d%s%d", pageNum + 1, "/", totp_count);
    u8g2.drawStr(110, 0, pages);              // draw page counter
    u8g2.drawStr(0, 0, totp_names[pageNum]);  // draw TOTP name

    drawCountdownBars(TIME);

    u8g2.sendBuffer();
  }

  if (updateBar == 1) {
    // draw countdown boxes
    u8g2.setDrawColor(0);
    u8g2.drawBox(110, 17, 18, 15);
    u8g2.setDrawColor(1);
    drawCountdownBars(TIME);
    u8g2.updateDisplayArea(13, 2, 3, 3);
  }

  updateDisplay = 0;
  updateBar = 0;
}
