/* ROBOT GUARDEE (by The Roboteur)
 *   
 * Website: www.roboteur.me
 * Facebook: facebook.com/TheRoboteur
 * Instagram: instagram.com/the_roboteur
 * YouTube: bitly.com/RoboteurTV  
 * GitHub: github.com/roboteur
 *  
 */

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

char* mySsid = "ANYUSERID";
char* password = "ANYPASSWORD";

bool ota_time_trigger = true;
uint16_t ota_time_consumed = 0;
int state_current = 0;

float Temperature;
float Humidity;

uint8_t pin_led = 2;
int pinInA0 = A0;
int Light;   

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
byte hf[8]= {B00000000,
             B00110110,
             B01000110,
             B01000000,
             B01000000,
             B01000110,
             B00110110,
             B00000000};
// neutral face
byte nf[8]= {B00000000,
             B00100100,
             B01000100,
             B01000000,
             B01000000,
             B01000100,
             B00100100,
             B00000000};
// sad face
byte sf[8]= {B00000000,
             B01000100,
             B00100100,
             B00100000,
             B00100000,
             B00100100,
             B01000100,
             B00000000};

byte bf[8]= {B00000000,
             B00100110,
             B01000110,
             B01000000,
             B01000000,
             B01000110,
             B00100110,
             B00000000};
             

void setup() {
                        
  Serial.begin(115200); 
  Serial.println("Starting ...");
  
  pinMode(pin_led, OUTPUT);
  
  WiFi.mode(WIFI_STA); /* Station Point Only */
  // WiFi.mode(WIFI_AP_STA); /* Both */
  // WiFi.mode(WIFI_AP); /* Access Point Only */  

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
    server.send(200, "text/plain", "1] ip/sad 2] ip/smile 3] ip/neutral 4] ip/ota 5] ip/reset 6] ip/data");
    delay(1000);
  });

  server.on("/reset", [](){
    server.send(200, "text/plain", "Resetting...");
    delay(1000);
    ESP.restart();    
  });

  server.on("/", handle_OnConnect);
  server.on("/toggle",toggleLED);   
  
  server.on("/smile",handle_Smile);  
  server.on("/sad",handle_Sad);
  server.on("/neutral",handle_Neutral);
  server.on("/blink",handle_Blink);
  
  WiFi.softAPConfig(ip, gateway, subnet);
  WiFi.softAP(mySsid, password);  

  lc.shutdown(0,false);
  // Set brightness to a medium value
  lc.setIntensity(0,8);
  // Clear the display
  lc.clearDisplay(0);  

  server.begin();
  dht.begin();
}

void loop() {
  
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  if(ota_time_trigger)
  {
    while(ota_time_consumed < 25000)
    {
      ArduinoOTA.handle();
      ota_time_consumed = millis();
      delay(10);
    }
    ota_time_trigger = false;
  }
    
  server.handleClient();

  Serial.println(" ");
  Serial.print("Temperature: ");
  Serial.println(Temperature);
  Serial.print("Humidity: ");
  Serial.println(Humidity);
  Serial.print("Light: ");
  Serial.print("Sound Provision: ");
 
  state_machine_serial();
  
}

String SendHTML(float Temperaturestat,float Humiditystat, float Lightstat) {
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
  ptr +=" hex</p>";
  ptr +="<p><a href=\"/\"><button>REFRESH</button></a></p>";
  ptr +="</div>\n";
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
  
  }
  
void toggleLED() {
  digitalWrite(pin_led,!digitalRead(pin_led));
  server.send(204,"");
  
}

void handle_OnConnect() {
  Temperature = dht.readTemperature(); // Gets the values of the temperature
  Humidity = dht.readHumidity(); // Gets the values of the humidity   
  int Light = analogRead(A0);

  if (Light > 900)  {
    handle_Sad();
    }
  else  {
    handle_Smile();
    }
    
  server.send(200, "text/html", SendHTML(Temperature,Humidity, Light)); 
  
  }
  
void handle_Smile()  {
  state_current = 1;
  Temperature = dht.readTemperature(); // Gets the values of the temperature
  Humidity = dht.readHumidity(); // Gets the values of the humidity   
  int Light = analogRead(A0);

  server.send(200, "text/html", SendHTML(Temperature,Humidity, Light)); 
   
  }

void handle_Sad()  {
  state_current = 2;
  Temperature = dht.readTemperature(); // Gets the values of the temperature
  Humidity = dht.readHumidity(); // Gets the values of the humidity   
  int Light = analogRead(A0);

  server.send(200, "text/html", SendHTML(Temperature,Humidity, Light)); 
   
  }

void handle_Neutral()  {
  state_current = 3;
  Temperature = dht.readTemperature(); // Gets the values of the temperature
  Humidity = dht.readHumidity(); // Gets the values of the humidity   
  int Light = analogRead(A0);

  server.send(200, "text/html", SendHTML(Temperature,Humidity, Light)); 
   
  }

void handle_Blink()  {
  state_current = 3;
  delay(1000);
  state_current = 4;
  delay(1000);
  Temperature = dht.readTemperature(); // Gets the values of the temperature
  Humidity = dht.readHumidity(); // Gets the values of the humidity   
  int Light = analogRead(A0);

  server.send(200, "text/html", SendHTML(Temperature,Humidity, Light)); 
   
  }

void state_machine_serial() {
 
  // state_previous = state_current; delay(1000);
    
 switch (state_current) {    
  // void drawFaces(){
    
 // Display sad face
   case 2: 
    lc.setRow(0,0,sf[0]);
    lc.setRow(0,1,sf[1]);
    lc.setRow(0,2,sf[2]);
    lc.setRow(0,3,sf[3]);
    lc.setRow(0,4,sf[4]);
    lc.setRow(0,5,sf[5]);
    lc.setRow(0,6,sf[6]);
    lc.setRow(0,7,sf[7]);
    delay(delaytime);   
    break; 

   case 3: // Display neutral face
    lc.setRow(0,0,nf[0]);
    lc.setRow(0,1,nf[1]);
    lc.setRow(0,2,nf[2]);
    lc.setRow(0,3,nf[3]);
    lc.setRow(0,4,nf[4]);
    lc.setRow(0,5,nf[5]);
    lc.setRow(0,6,nf[6]);
    lc.setRow(0,7,nf[7]);
    delay(delaytime);
    break;
 
   case 1: // Display happy face
    lc.setRow(0,0,hf[0]);
    lc.setRow(0,1,hf[1]);
    lc.setRow(0,2,hf[2]);
    lc.setRow(0,3,hf[3]);
    lc.setRow(0,4,hf[4]);
    lc.setRow(0,5,hf[5]);
    lc.setRow(0,6,hf[6]);
    lc.setRow(0,7,hf[7]);
    delay(delaytime);
    break;

   case 4: // Display happy face
    lc.setRow(0,0,bf[0]);
    lc.setRow(0,1,bf[1]);
    lc.setRow(0,2,bf[2]);
    lc.setRow(0,3,bf[3]);
    lc.setRow(0,4,bf[4]);
    lc.setRow(0,5,bf[5]);
    lc.setRow(0,6,bf[6]);
    lc.setRow(0,7,bf[7]);
    delay(delaytime);
    break;
    
  }
}
