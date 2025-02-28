#include "Arduino_LED_Matrix.h"
#include "WiFiS3.h"
#include "RTC.h"
#include <NTPClient.h>
#include <ESP_Mail_Client.h>
#define relay 2

ArduinoLEDMatrix matrix;

char ssid[] = "Mi 10T Lite";
char pass[] = "password";   
int keyIndex = 0;      // your network key index number (needed only for WEP)

String output = "off";
String header;

// Current time
unsigned long currentTime = millis();
unsigned long previousTime = 0;
// Define timeout time in milliseconds 
const long timeoutTime = 2000;


int status = WL_IDLE_STATUS;
WiFiServer server(80);

long umiditate = 0;
// timp
RTCTime startTime, currentT;
unsigned long unixTime;
unsigned long unixTimeLastCheck = 0;
unsigned long minutesSinceLastCheck;
unsigned long minMinutesBetweenCheck = 1;

WiFiUDP Udp; //ne permite sa primim si sa trimitem pachete prin UDP
NTPClient timeClient(Udp);

unsigned long timeZoneOffsetHours;
bool verify;

void setRTCtime(){
  RTC.begin();
  timeClient.begin();
  timeClient.update();

  timeZoneOffsetHours = +2; // Set timezone offset
  unixTime = timeClient.getEpochTime() + (timeZoneOffsetHours * 3600); 
  RTCTime timeToSet = RTCTime(unixTime);
  RTC.setTime(timeToSet);

  RTC.getTime(currentT); 

}

void setup() {
  Serial.begin(9600); // initialize serial communication
  matrix.begin();
  pinMode(relay, OUTPUT); // set the Relay pin mode
  digitalWrite(relay, HIGH);

  setRTCtime();

  pinMode(A0, INPUT);


  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Network named: ");
    Serial.println(ssid);

    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(10000);
  }
  server.begin(); // start the web server on port 80
  printWifiStatus(); // print the status
}


void loop() {
  webServer();
  verify_humidity();
  umiditate = analogRead(A0);
  RTC.getTime(currentT);
  unsigned long minutes = currentT.getMinutes();
  unixTime = currentT.getUnixTime();
  minutesSinceLastCheck = (unixTime - unixTimeLastCheck)/60;

  if (minutesSinceLastCheck >= minMinutesBetweenCheck){
    verify_humidity();
    verify = true;
    unixTimeLastCheck = unixTime;
    Serial.print("Au trecut 2 minute");
    Serial.println(unixTime);
  } 
  else 
    verify = false;
}

void webServer() {
  WiFiClient client = server.available();   // Listen for incoming clients
  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // we read and print it
        Serial.write(c);                    
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code 
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // turns the relay on and off
            if (header.indexOf("GET /on") >= 0) {
              output = "on";
              digitalWrite(relay, LOW);
            } else if (header.indexOf("GET /off") >= 0) {
              output = "off";
              digitalWrite(relay, HIGH);
            }

            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".on { background-color: #FF0000; border: 5px; color: white; padding: 16px 40px; border-radius: 20px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".off {background-color: #000000;border: 5px; color: white; padding: 16px 40px; border-radius: 20px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}</style></head>");


            // Web Page Heading
            client.println("<body><h1>Ingrijitor Flori</h1>");
            client.println("<p>Relay " + output + "</p>");
            if (output == "off") {
              client.println("<p><a href=\"/on\"><button class=\"off\">OFF</button></a></p>");
            } else {
              client.println("<p><a href=\"/off\"><button class=\"on\">ON</button></a></p>");
            }

            client.println("</body></html>");

            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if we got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if we got anything else but a carriage return character,
          currentLine += c;      // we add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}

void verify_humidity(){
  umiditate = analogRead(A0);
  umiditate = map(umiditate, 950, 350, 0, 100);
  Serial.print("Umiditatea este:");
  Serial.println(umiditate);
  while(umiditate < 20 && verify)
  {
    umiditate = analogRead(A0);
    umiditate = map(umiditate, 950, 350, 0, 100);
    Serial.println("Mai mic de 20");
    digitalWrite(relay, LOW);
    delay(1000);
    digitalWrite(relay, HIGH);
  }
  setRTCtime();
}

void printWifiStatus() {
  // print the SSID of the network we're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  Serial.print("Now open this URL on your browser --> http://");
  Serial.println(ip);
}