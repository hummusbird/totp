#include "TOTP.h"
#include "swRTC.h"

uint8_t hmacKey[] = "12345678901234567890";

TOTP totp = TOTP(hmacKey, 20, 30);
swRTC rtc;
char code[7];

void setup() {
  Serial.begin(9600);
  rtc.stopRTC();

  rtc.setDate(16, 10, 2023);
  rtc.setTime(21, 54, 0);

  rtc.startRTC();
}

void loop() {
  long GMT = rtc.getTimestamp();
  char* newCode = totp.getCode(GMT);
  Serial.print("UNIX: ");
  Serial.println(GMT);
  Serial.print("CODE: ");
  Serial.println(newCode);
  Serial.println("=======================");

  if (strcmp(code, newCode) != 0) {
    strcpy(code, newCode);
    Serial.println(code);
  }

  delay(1000);
}