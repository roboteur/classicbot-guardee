# Robot Guardee
* Easy to build desktop companion robot
* Most affordable and teaches sensor concepts and programming
* Build Your Own For less than $20 (excluding battery)

## Features
* Smartphone Controlled
* Over-The-Air Update
* Code Design Pattern Included

## How It Works 
* Here's the link >> https://youtu.be/Y-y1LgC-yeM

## Quick Start
1. Setup The ESP8266
    * Download Arduino IDE: https://www.arduino.cc/en/Main/Software
    * Open Arduino IDE and click: File -> Preferences -> Additional Boards Manager --> Paste this     link: https://arduino.esp8266.com/stable/package_esp8266com_index.json
    * Setup the board: Go to Tools -> Board -> Board Manager -> search ESP8266 Community, then click install
    * Select the ESP8266 board: Tools -> Board -> NodeMCU 1.0 (ESP12E Module)
    * Select the PORT: Tools -> PORT -> USB0 (or anything that works)
    * Upload the Code: Sketch -> Upload (There should be no error. If there is, it is mostly about user permissions. Message me if you can't go through this.)
2. Upload the code guardee.ino
3. Find the IP Address from the Arduino IDE Serial Monitor. 
4. Copy and paste the IP Address in the browser and start from there

## Bill Of Materials
1. Wemos D1 Mini (ESP8266) x 1  = $5.00 approx.
2. 8x8 LED Matrix x 1  = $2.00 approx.
3. 18650 Battery Holder x 2 slots = $1.00 approx.
4. Temperature and Humidity Sensor = $2.00 approx.
5. Light Dependent Resistor = $ 1.00 approx
6. Solid Wires #22 x 3ft. = $1.00 approx.
7. USB Cable x 1 = $2.00 approx.
8. Half-sized breadboard x 1 = $2.00 approx.
9. Breadboard Powersupply x 1 = $2.00 approx
10. Long-legged Headers x 3 = $2.00 approx
11. 18650 Battery x 2 = Depends on you. 

## Robot Construction
![Robot Construction](/robot-guardee-01.png)

## Wiring 
* 8X8 LED Matrix
   * VCC connect to 5V Breadboard Powersupply
   * GND connect to GND Breadboard Powersupply
   * DIN connect to D5 pin of Wemos D1 Mini (ESP8266)
   * CS connect to D6 pin of Wemos D1 Mini (ESP8266)
   * CLK connect to D7 pin of Wemos D1 Mini (ESP8266)
   
* Light Dependent Resistor (LDR) or Photo Sensor
   * VCC connect to 5V Breadboard Powersupply
   * GND connect to GND Breadboard Powersupply
   * A0 connect to A0 of Wemos D1 Mini (ESP8266)
   
* Temperature and Humidity Sensor
   * VCC connect to 5V Breadboard Powersupply
   * GND connect to GND Breadboard Powersupply
   * OUT connect to D1 pin of Wemos D1 Mini (ESP8266
   
## Demo Video
* Here's the link --> https://youtu.be/-PQkA54bA8s

## Additional Notes
* Bug: If for some reason you cannot compile this code in Arduino IDE version 1.8.10 or up, revert to version 1.8.9.
* Bug: AP STA modes causes confusion with OTA.

## Feel Free To Direct Message
* Facebook: https://facebook.com/TheRoboteur
* Instagram: https://instagram.com/the_roboteur
