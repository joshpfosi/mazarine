/*
  Web client
 
 This sketch connects to a website (http://www.google.com)
 using a WiFi shield.
 
 This example is written for a network using WPA encryption. For 
 WEP or WPA, change the Wifi.begin() call accordingly.
 
 This example is written for a network using WPA encryption. For 
 WEP or WPA, change the Wifi.begin() call accordingly.
 
 Circuit:
 * WiFi shield attached
 
 created 13 July 2010
 by dlf (Metodo2 srl)
 modified 31 May 2012
 by Tom Igoe
 */


#include <SPI.h>
#include <WiFi.h>

char ssid[] = "tufts-guest"; //  your network SSID (name) 
//char pass[] = "";
int keyIndex = 0;            // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;
char server[] = "swarmbot-status.herokuapp.com";

// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):
WiFiClient client;

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600); 
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  
  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present"); 
    // don't continue:
    while(true);
  } 
  
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) { 
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:    
    status = WiFi.begin(ssid);
  
    // wait 10 seconds for connection:
    delay(1000);
  } 
  Serial.println("Connected to wifi");
  
  Serial.println("\nStarting connection to server..."); 
  post("Test1155", "false", "true", "true", "FOLLOWING RED");
}

void loop() {
  // if there are incoming bytes available 
  // from the server, read them and print them:
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting from server.");
    client.stop();

    // do nothing forevermore:
    while(true);
  }
}

void post(const char *content, const char *bot, const char *stat, const char *toxic, const char *state) {
    // if you get a connection, report back via serial:
  if (client.connect(server, 80)) {
    Serial.println("connected to server");
    
    // Make a HTTP request:
    char buf[1000];
    sprintf(buf, "{\"todo\": {\"content\": \"%s\", \"bot\": %s, \"status\": %s, \"toxic\": %s, \"state\": \"%s\"}}\r\n", content, bot, stat, toxic, state);
    String data(buf);
   
    // send the HTTP PUT request:
    client.print("POST /todos HTTP/1.1\r\n");
    client.print("Host: swarmbot-status.herokuapp.com\r\n");
    client.print("Host: swarmbot-status.herokuapp.com\r\n");
    client.print("Accept: application/json\r\n");
    client.print("Content-Type: application/json\r\n");
    client.print("Content-Length: ");
    client.print(data.length());
    client.print("\r\n");
    client.print("Cache-Control: no-cache\r\n");
    
    //last pieces of the HTTP PUT request:
    client.print("\r\n");
    client.print("{\"todo\": {\"content\": \"");
    client.print(content);
    client.print("\", \"bot\": ");
    client.print(bot);
    client.print(", \"status\": ");
    client.print(stat);
    client.print(", \"toxic\": ");
    client.print(toxic);
    client.print(", \"state\": \"");
    client.print(state);
    client.print("\"}}\r\n");
  } 
}
