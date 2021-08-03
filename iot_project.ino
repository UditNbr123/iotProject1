//libraries for oled display
#include <Wire.h>
#include "SSD1306.h" 
SSD1306  display(0x3c, 5, 4); 

//Button details
#define btnPin 16

//network details
#include <ESP8266WiFi.h>
#define mainSsid "Connect"
#define mainPass "Fast45678"

//server info
#define CSE_IP "192.168.18.25"
#define CSE_PORT 8080
#define HTTPS false
#define OM2M_ORIGIN "admin:admin"
#define OM2M_MN "/~/mn-cse/mn-name/"
#define OM2M_MN_1 "/~/in-cse/in-name/"
#define OM2M_AE "Weather"
#define OM2M_DATA_CONT "DATA"
#define LISTENER_PORT 8000
WiFiServer listener(LISTENER_PORT);
HTTPClient http;

//WiFiserver object
WiFiServer my_server(80);
WiFiClient client;

// DHT11
#include "DHT.h"
#define DHTPIN D3  
#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE);
int localHum = 0;
int localTemp = 0;

void setup() 
{
  Serial.begin(9600);
  delay(100);
  
  //Setup the sensor
  dht.begin();
  //Connect to the wifi
  Serial.println("Connecting to");
  Serial.println(mainSsid);
  WiFi.begin(mainSsid,mainPass);

  while(WiFi.status()!=WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi Connected");

  listener.begin(LISTENER_PORT);
  Serial.println("ESP listener started");
  Serial.println("IP Address is "+WiFi.localIP());
  delay(500);
  display.init(); // Initialising the UI will init the display too.
  display.flipScreenVertically();
}

void loop() 
{
  getDHT();
  display.clear();
  drawDHT(); 
  display.display();
  if(buttonPressed(btnPin))
  {
      String data = "[" + String(localTemp) + ", " + String(localHum) + "]";
      String server="http://" + String() + CSE_IP + ":" + String() + CSE_PORT + String()+OM2M_MN_1;
      http.begin(server + String() +OM2M_AE_2 + "/" + OM2M_DATA_CONT + "/");
      http.addHeader("X-M2M-Origin", OM2M_ORIGIN);
      http.addHeader("Content-Type", "application/json;ty=4");
      http.addHeader("Content-Length", "100");
  
      String req_data = String() + "{\"m2m:cin\": {"

        + "\"con\": \"" + data + "\","

        + "\"cnf\": \"text\""

        + "}}";
      int code = http.POST(req_data);
      http.end();
      Serial.println(code); 
  }
  delay (2000);
}

void getDHT()
{
  float tempIni = localTemp;
  float humIni = localHum;
  localTemp = dht.readTemperature();
  localHum = dht.readHumidity();
  if (isnan(localHum) || isnan(localTemp))   // Check if any reads failed and exit early (to try again).
  {
    Serial.println("Failed to read from DHT sensor!");
    localTemp = tempIni;
    localHum = humIni;
    return;
  }
}

//Draw Indoor Page

void drawDHT() 
{
  int x=0;
  int y=0;
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0 + x, 5 + y, "Hum");
  
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(43 + x, y, "INDOOR");

  display.setFont(ArialMT_Plain_24);
  String hum = String(localHum) + "%";
  display.drawString(0 + x, 15 + y, hum);
  int humWidth = display.getStringWidth(hum);

  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(95 + x, 5 + y, "Temp");

  display.setFont(ArialMT_Plain_24);
  String temp = String(localTemp) + "°C";
  display.drawString(70 + x, 15 + y, temp);
  int tempWidth = display.getStringWidth(temp);
}
//button pressed
int buttonPressed(uint8_t button) 
{
  static uint16_t lastStates = 0;
  uint8_t state = digitalRead(button);
  if (state != ((lastStates >> button) & 1)) {
    lastStates ^= 1 << button;
    return state == HIGH;
  }
  return false;
}
