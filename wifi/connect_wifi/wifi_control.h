/* wifi_control.h
 * This file exposes the wifi communication interface to implement wifi post requests
 */

#include <SPI.h>
#include <WiFi.h>
#include <Arduino.h>

// function declarations
void connectWifi(void);
void printWifiStatus(void);
void postData(char* post_msg, boolean bot, boolean status);
