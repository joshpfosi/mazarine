/* wifi_control.c
 *
 * Connects to an open WiFi network and makes POST requests
 * 
 * The bot speed is held in a static global to avoid the overhead of structs
 */

#include "Arduino.h"
#include <SPI.h>
#include <WiFi.h>
#include "wifi_control.h"

static const char server[]       = "maz-site.herokuapp.com/";
static const int yourarduinodata = 999;
static char ssid[]               = "yourNetwork";     // the name of your network
static int status                = WL_IDLE_STATUS;    // the Wifi radio's status
static String yourdata;
static WiFiClient client;

void connectWifi(void) {
  // Attempt to connect to wifi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid);
    // Wait 10 seconds for connection
    delay(10000);
  }
}

void printWifiStatus(void) {
  // Print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // Print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // Print the received signal strength
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

// This method makes a HTTP connection to the server and POSTs data
void postData(char* post_msg, boolean bot, boolean status) {
  // If there's a successful connection, send the HTTP POST request
  if (client.connect(server, 80)) {
    Serial.println("connecting...");

    char buf[100];
    sprintf(buf, "{\"todo\": {\"content\": %s, \"bot\": %d, \"status\": %d}}", post_msg, bot, status);
    yourdata = buf;
    client.println(yourdata); 
  } 
  else {
    // If you couldn't make a connection:
    Serial.println("Connection failed");
    Serial.println("Disconnecting.");
    client.stop();
  }
}
