
// Screen dimensions
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 128

// You can use any (4 or) 5 pins 
#define SCLK_PIN 2
//#define MOSI_PIN 3
#define DC_PIN   4
#define CS_PIN   5
#define RST_PIN  6

// Color definitions
#define BLACK           0x0000
#define BLUE            0x001F
#define RED             0xF800
#define GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0  
#define WHITE           0xFFFF

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include <SPI.h>
#include "Adafruit_VEML7700.h"
#include "RTClib.h"
#include <SD.h> // the SD card h file, native to arduino

Adafruit_VEML7700 veml = Adafruit_VEML7700();

// Option 1: use any pins but a little slower
Adafruit_SSD1351 tft = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, CS_PIN, DC_PIN, 3, SCLK_PIN, RST_PIN);  

// Option 2: must use the hardware SPI pins 
// (for UNO thats sclk = 13 and sid = 11) and pin 10 must be 
// an output. This is much faster - also required if you want
// to use the microSD card (see the image drawing example)
//Adafruit_SSD1351 tft = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, CS_PIN, DC_PIN, RST_PIN);

float p = 3.1415926;
File myFile;
RTC_DS3231 rtc;
int grow_light_status = 0;
unsigned long previousMillis = 0;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
long ticker = 5000; // milliseconds between data points

void setup () {
  pinMode(7,OUTPUT);
  pinMode(10,OUTPUT);
  Serial.begin(9600);
  tft.begin();
  uint16_t time = millis();

#ifndef ESP8266
  while (!Serial); // wait for serial port to connect. Needed for native USB
#endif

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  // When time needs to be re-set on a previously configured device, the
  // following line sets the RTC to the date & time this sketch was compiled
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));

  if (!veml.begin()) {
    Serial.println("Lux Sensor not found");
    while (1);
  }
  Serial.println("Sensor found");

  veml.setGain(VEML7700_GAIN_1);
  veml.setIntegrationTime(VEML7700_IT_100MS);

  Serial.print(F("Gain: "));
  switch (veml.getGain()) {
    case VEML7700_GAIN_1: Serial.println("1"); break;
    case VEML7700_GAIN_2: Serial.println("2"); break;
    case VEML7700_GAIN_1_4: Serial.println("1/4"); break;
    case VEML7700_GAIN_1_8: Serial.println("1/8"); break;
  }

  Serial.print(F("Integration Time (ms): "));
  switch (veml.getIntegrationTime()) {
    case VEML7700_IT_25MS: Serial.println("25"); break;
    case VEML7700_IT_50MS: Serial.println("50"); break;
    case VEML7700_IT_100MS: Serial.println("100"); break;
    case VEML7700_IT_200MS: Serial.println("200"); break;
    case VEML7700_IT_400MS: Serial.println("400"); break;
    case VEML7700_IT_800MS: Serial.println("800"); break;
  }

  //veml.powerSaveEnable(true);
  //veml.setPowerSaveMode(VEML7700_POWERSAVE_MODE4);

  veml.setLowThreshold(10000);
  veml.setHighThreshold(20000);
  veml.interruptEnable(true);
  
  //setup the SD card
  if (!SD.begin(10)){
    Serial.println("initialization failed!");
    return;
  }

}
void loop () {

  DateTime now = rtc.now();
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= ticker){
    previousMillis = currentMillis; // remember the time
  
//code to turn on the grow light if the seconds of the minute are between 0 and 10
  if ((now.second()>0) && (now.second()<10)){
     grow_light_status = 1;
     digitalWrite(7, HIGH);
  }
  else{
    grow_light_status = 0;
    digitalWrite(7, LOW);
  }
  
  Serial.print("Lux: "); Serial.println(veml.readLux());
  Serial.print("White: "); Serial.println(veml.readWhite());
  Serial.print("Raw ALS: "); Serial.println(veml.readALS());

//For the SD card...

myFile = SD.open("luxdata.txt", FILE_WRITE);
if (myFile) {
    Serial.print("Writing to luxdata.txt...");
    myFile.print(now.year(), DEC);
    myFile.print('/');
    if(now.month()<10){
       myFile.print('0');
    }
    myFile.print(now.month(), DEC);
    myFile.print('/');
    if(now.day()<10){
       myFile.print('0');
    }
    myFile.print(now.day(), DEC);
    myFile.print(F(" "));
    if(now.hour()<10){
       myFile.print('0');
    }
    myFile.print(now.hour(), DEC);
    myFile.print(':');
    if(now.minute()<10){
       myFile.print('0');
    }
    myFile.print(now.minute(), DEC);
    myFile.print(':');
    if(now.second()<10){
       myFile.print('0');
    }
    myFile.print(now.second(), DEC);
    myFile.print(F(" "));
    myFile.print(veml.readLux());
    myFile.print(F(" "));
    myFile.print(veml.readWhite());
    myFile.print(F(" "));
    myFile.print(veml.readALS());
    myFile.print(F(" "));
    myFile.print(grow_light_status);
    myFile.println();
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening luxdata.txt");
  }
//end of SD card

  uint16_t irq = veml.interruptStatus();
  if (irq & VEML7700_INTERRUPT_LOW) {
    Serial.println("** Low threshold"); 
  }
  if (irq & VEML7700_INTERRUPT_HIGH) {
    Serial.println("** High threshold"); 
  }

    //tft.fillScreen(BLACK);
    tft.fillRect(0,0,128,128,BLACK);
    tft.setTextColor(WHITE);
    tft.setTextSize(2);
    tft.setCursor(0, 0);
    tft.print("Grow Light"); 
    
    if (grow_light_status == 1){
        tft.setTextSize(3);
        tft.setTextColor(YELLOW);
        tft.print("ON");
    }
    else {
      tft.setTextColor(BLUE);
      tft.setTextSize(3);
      tft.print("OFF");
    }
    tft.setTextColor(WHITE);
    tft.setTextSize(1);
    tft.setCursor(0, 80);
    tft.print("Lux: "); tft.println(veml.readLux());
    tft.setCursor(0, 90);
    tft.print("White: "); tft.println(veml.readWhite());
    tft.setCursor(0, 100);
    tft.print("Raw ALS: "); tft.println(veml.readALS());
    tft.setCursor(0, 110);
    tft.setTextColor(WHITE);
    tft.setTextSize(1);
    tft.print("Temp: ");
    tft.print(rtc.getTemperature());
    tft.println(" C");
    tft.println();
    tft.setCursor(0, 120);
    tft.setTextColor(WHITE);
    tft.setTextSize(1);
    tft.print(now.hour(), DEC);
    tft.print(':');   
    if(now.minute()<10){
       tft.print('0');
    }
    tft.print(now.minute(), DEC);
    tft.print(':');
    if(now.second()<10){
       tft.print('0');
    }
    tft.print(now.second(), DEC);
    tft.println();
    delay(500);
}
}
