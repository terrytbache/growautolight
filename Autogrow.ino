// Version 20210717.2117

// REQUIRES the following Arduino libraries:
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN_0 2     // Digital pin connected to the 1st DHT sensor 
#define DHTPIN_1 3     // Digital pin connected to the 2nd DHT sensor 
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.

#define DHTTYPE    DHT11     // DHT 11


// See guide for details on sensor wiring and usage:
//   https://learn.adafruit.com/dht/overview

DHT_Unified dht_0(DHTPIN_0, DHTTYPE);
DHT_Unified dht_1(DHTPIN_1, DHTTYPE);

uint32_t delayMS;
int Soil_Wet_1 = 0;
int Soil_Wet_2 = 0;
int h, m, s, lightControl, feedpumpControl, stirpumpControl, lightTest, feedTest, stirTest = 0;
unsigned long syncMillis, nowMillis, elapsedMillis, elapsedSecs;

void setup() {
  pinMode(3, INPUT); //stir pump test button
  pinMode(4, INPUT); //feed pump test button
  pinMode(5, INPUT); //lights test button
  pinMode(11, OUTPUT); //relay 3 - stir pump
  pinMode(12, OUTPUT); //relay 1 - lights
  pinMode(13, OUTPUT); //relay 2 - feed pump
  Serial.begin(9600);
  // Initialize 1st DHT device.
  dht_0.begin();
  // Initialize 2nd DHT device.
  dht_1.begin();
  sensor_t sensor;
  dht_0.temperature().getSensor(&sensor);

  // Delay between readings
  delayMS = ((sensor.min_delay / 1000)); // one reading per second
}

void loop() {
  // Delay between measurements.
  delay(delayMS);

/*
*  Test buttons
*/
  
  lightTest = digitalRead(5);
  feedTest = digitalRead(4);
  stirTest = digitalRead(3);
  
/*
 * Read everything from the serial buffer
 * This is the current time 
 */

nowMillis = millis(); //current millis
elapsedMillis = nowMillis - syncMillis; //elapsed millis since the last sync event
elapsedSecs = (elapsedMillis / 1000);


  while (Serial.available() > 0)
 {
    h = Serial.read();
    m = Serial.read();
    s = Serial.read();
    syncMillis = millis();
    
  }

    Serial.print(h);
  Serial.print(",");
    Serial.print(m);
  Serial.print(",");
    Serial.print(s);
  Serial.print(",");

  /*
 * Get temperature event and print its value.
 * 1st temp/humid Sensor
 */
  
  sensors_event_t event_0;
  dht_0.temperature().getEvent(&event_0);
  
  if (isnan(event_0.temperature)) {
 //   Serial.println(F("Error reading temperature!"));
  }
  else {

    Serial.print(event_0.temperature);
    Serial.print(",");
  }
  // Get humidity event and print its value.
  dht_0.humidity().getEvent(&event_0);

  if (isnan(event_0.relative_humidity)) {
 //   Serial.println(F("Error reading humidity!"));
  }
  else {
    Serial.print(event_0.relative_humidity);
    Serial.print(",");
  }

/*
 * 2nd temp/humid Sensor//
 */
  sensors_event_t event_1;
  dht_1.temperature().getEvent(&event_1);

  if (isnan(event_1.temperature)) {
 //   Serial.println(F("Error reading temperature!"));
  }
  else {

    Serial.print(event_1.temperature);
    Serial.print(",");
  }
  // Get humidity event and print its value.
  dht_1.humidity().getEvent(&event_1);

  if (isnan(event_1.relative_humidity)) {
 //   Serial.println(F("Error reading humidity!"));
  }
  else {
    Serial.print(event_1.relative_humidity);
    Serial.print(",");
  }

/*
 * Read both of the soil sensors and print to serial for logging
 */

Soil_Wet_1 = analogRead(2);
Soil_Wet_2 = analogRead(3);
  Serial.print(Soil_Wet_1);
  Serial.print(",");
  Serial.print(Soil_Wet_2);
  Serial.print(",");

/*
 * Relay controls
 */

/*
 * Keep the lights on from 5AM to 11PM  
 */

 if ((h >= 8 && h < 20)||(lightTest==1)){
  lightControl = 1;
  digitalWrite(12,HIGH);
}
  else {
    lightControl = 0;
    digitalWrite(12,LOW);
  }
   Serial.print(lightTest);
   Serial.print(lightControl);


/*
 * Run the feed pump for 5 seconds at 7am  
 */

  if (feedTest==1){
  feedpumpControl = 1;
  digitalWrite(13,HIGH);
}
  else {
    feedpumpControl = 0;
    digitalWrite(13,LOW);
  }
  Serial.print(feedTest);
  Serial.print(feedpumpControl);
  
  
/*
 * Run the stir pump for 30 seconds at 630am  
 */
 
  if (stirTest==1){
  stirpumpControl = 1;
  digitalWrite(11,HIGH);
}
  else {
    stirpumpControl = 0;
    digitalWrite(11,LOW);
  }
  Serial.print(stirTest);
  Serial.println(stirpumpControl);
  
}

// Change Log
// 07172021 - Changed the pump control to button only
//          - Moved the button inputs from 6,7,8 to 3,4,5 - thought they were an issue, turns out my code had a "=" not a "==" :-(
 
  
