#include <SPI.h>
#include <WiFi.h>
#include "wifi_control.h"

void setup() {
  Serial.begin(9600);
  connectWifi();
  printWifiStatus();
}

void loop() {
  postData("Good", true, true);
}


