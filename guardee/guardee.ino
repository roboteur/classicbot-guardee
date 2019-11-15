

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>   

#include <DHT.h>
#define DHTPIN D1                          // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11                      // DHT 11

#include "LedControl.h"
#include "binary.h"

char* password = "X9425TE9";
char* mySsid = "ZTEH108N_3C01E";

bool ota_flag = true;
uint16_t time_elapsed = 0;

float Temperature;
float Humidity;

uint8_t pin_led = 2;
int pinInA0 = A0;   

DHT dht(DHTPIN, DHTTYPE);
ESP8266WebServer server;

/* AT AP MODE */
IPAddress ip(192,168,18,190);
IPAddress gateway(192,168,11,0);
IPAddress subnet(255,255,255,0);

/*
 DIN connects to pin 5
 CLK connects to pin 7
 CS connects to pin 6
*/

LedControl lc=LedControl(D5,D7,D6,1); 

// delay time between faces
unsigned long delaytime=1000;

// happy face
byte hf[8]= {B00111100,
             B01000010,
             B10010101,
             B10100001,
             B10100001,
             B10010101,
             B01000010,
             B00111100};
// neutral face
byte nf[8]={B00111100, B01000010,B10100101,B10000001,B10111101,B10000001,B01000010,B00111100};
// sad face
byte sf[8]= {B00111100,B01000010,B10100101,B10000001,B10011001,B10100101,B01000010,B00111100};

void setup() {
                        
  Serial.begin(115200); 
  Serial.println("Starting ...");
  
  pinMode(pin_led, OUTPUT);

  WiFi.mode(WIFI_AP); /* Access Point Only */  
  // WiFi.mode(WIFI_STA); /* Station Point Only */
  // WiFi.mode(WIFI_AP_STA); /* Both */

  /* 
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Restarting...");
    delay(5000);
    ESP.restart();
  }

  */
  
  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
    type = "sketch";
  } else { // U_SPIFFS
      type = "filesystem";
    }

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/ota", [](){
    server.send(200, "text/plain", "Upload the firmware.");
    delay(1000);
    ESP.restart();    
  });  
  
  server.on("/list", [](){
    server.send(200, "text/plain", "1] ip/blinkFast 2] ip/blinkSlow 3] ip/on 4] ip/off 5] ip/ota 6] ip/reset 7] ip/data");
    delay(1000);
  });

  server.on("/reset", [](){
    server.send(200, "text/plain", "Resetting...");
    delay(1000);
    ESP.restart();    
  });

  server.on("/", handle_OnConnect);
  server.on("/toggle",toggleLED);   

  server.begin();

  dht.begin();

  WiFi.softAPConfig(ip, gateway, subnet);
  WiFi.softAP(mySsid, password);

  

  lc.shutdown(0,false);
  // Set brightness to a medium value
  lc.setIntensity(0,8);
  // Clear the display
  lc.clearDisplay(0);  
}

void loop() {
  
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  // OTA FLAG WORKS ONLY AT ROUTE IP/ota Unclear of this snippet.
  if(ota_flag)
  {
    while(time_elapsed < 25000)
    {
      ArduinoOTA.handle();
      time_elapsed = millis();
      delay(10);
    }
    ota_flag = false;
  }

  // Sound Sensor
  if(D3 == HIGH) {
      digitalWrite(pin_led,!digitalRead(pin_led));
    }
    
  server.handleClient();

  Serial.println(" ");
  Serial.print("Temperature: ");
  Serial.println(Temperature);
  Serial.print("Humidity: ");
  Serial.println(Humidity);
  Serial.print("Light: ");
  // Serial.println(valueInA1);
  Serial.print("Sound Provision: ");
 //  Serial.println(valueInA2);

  drawFaces();
}

void toggleLED()
{
  digitalWrite(pin_led,!digitalRead(pin_led));
  server.send(204,"");
}

void handle_OnConnect() {

  Temperature = dht.readTemperature(); // Gets the values of the temperature
  Humidity = dht.readHumidity(); // Gets the values of the humidity 
  
  int Light = analogRead(A0);

  // digitalWrite(pinOut02, HIGH);                  // Y1
  // digitalWrite(pinOut03, LOW);
  // digitalWrite(pinOut04, LOW);
  // delay(250);
  // int Light = analogRead(pinInA0);             // Sensing voltage input in pin A0 and converting to integer values (0 - 1023)
    // Serial.print("A1 = ");                       // Label the ouput 
    // Serial.println (valueInA1);
    // delay(750);

  // digitalWrite(pinOut02, LOW);                   // Y2
  // digitalWrite(pinOut03, HIGH);
  // digitalWrite(pinOut04, LOW);
  // delay(250);
    // valueInA2 = analogRead(pinInA0);             // Sensing voltage input in pin A0 and converting to integer values (0 - 1023)
    // Serial.print("A2 = ");                       // Label the ouput 
    // Serial.println (valueInA2);
    // delay(750);   

  /* Temporary random mutation */
  // Temperature = 28;
  // Humidity = 80;
  // int Light = 567;
  // valueInA2 = 567;
   
  // server.send(200, "text/html", SendHTML(Temperature,Humidity, Light, valueInA2)); 
  server.send(200, "text/html", SendHTML(Temperature,Humidity, Light)); 
  
  }

String SendHTML(float Temperaturestat,float Humiditystat, float Lightstat){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>GUARDEE</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\n";
  ptr +="p {font-size: 24px;color: #444444;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<div id=\"webpage\">\n";
  ptr +="<h1>GUARDEE</h1>\n";
  ptr +="<p>Temperature: ";
  ptr +=(int)Temperaturestat;
  ptr +=" C</p>";
  ptr +="<p>Humidity: ";
  ptr +=(int)Humiditystat;
  ptr +=" %</p>";
  ptr +="<p>Light: ";
  ptr +=(int) Lightstat;
  //  qptr +=" hex</p>";
  // ptr +="<p>Sound Provision: ";
  // ptr +=(int) AnalogValuestat;
  ptr +=" hex</p>";
  
  ptr +="</div>\n";
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
  }

  void drawFaces(){
 /* // Display sad face
  lc.setRow(0,0,sf[0]);
  lc.setRow(0,1,sf[1]);
  lc.setRow(0,2,sf[2]);
  lc.setRow(0,3,sf[3]);
  lc.setRow(0,4,sf[4]);
  lc.setRow(0,5,sf[5]);
  lc.setRow(0,6,sf[6]);
  lc.setRow(0,7,sf[7]);
  delay(delaytime);

 */

  /* 
  // Display neutral face
  lc.setRow(0,0,nf[0]);
  lc.setRow(0,1,nf[1]);
  lc.setRow(0,2,nf[2]);
  lc.setRow(0,3,nf[3]);
  lc.setRow(0,4,nf[4]);
  lc.setRow(0,5,nf[5]);
  lc.setRow(0,6,nf[6]);
  lc.setRow(0,7,nf[7]);
  delay(delaytime);

  */
  
  // Display happy face
  lc.setRow(0,0,hf[0]);
  lc.setRow(0,1,hf[1]);
  lc.setRow(0,2,hf[2]);
  lc.setRow(0,3,hf[3]);
  lc.setRow(0,4,hf[4]);
  lc.setRow(0,5,hf[5]);
  lc.setRow(0,6,hf[6]);
  lc.setRow(0,7,hf[7]);
  delay(delaytime);
}
