/**
ADS51115+BME280 I2C reader
K. Miuchi 2024 July
**/

#include <Arduino.h>
#include <Wire.h>
#include <ADS1X15.h> //RobTillaart/ADS1X15
#include <Adafruit_BME280.h> //BME280 driver https://github.com/adafruit/Adafruit_BME280_Library
#include "EspEasySerialCommand.h"



// data post
#define SENSOR_ID "RMNS-14" // name your sensor ID


//I2C pin
const int scl = 26; //RMNS
const int sda = 27; //RMNS

//I2C address
static const uint8_t BME280_ADDR   = 0x76;
static const uint8_t ADS_ADDR   = 0x48;

//modules
ADS1115 ADS(ADS_ADDR); //ADC
Adafruit_BME280 bme; //BME280
EspEasySerialCommand command(Serial);

// serial
const int baud  = 115200;

//moinitor
const int interval =1000; //ms

//ADC
float f;


void POWER_ON(EspEasySerialCommand::command_t command) {
  //Serial.print("ON");
  dacWrite(25,255);//3.3V/255
}

void POWER_OFF(EspEasySerialCommand::command_t command) {
  //Serial.print("ON");
  dacWrite(25,0);//3.3V/255
}


void setup() {
  
    Serial.begin(115200);
    Serial.begin(baud);
    Wire.begin( sda, scl, 400000 );
    BME280Init();  
    ADS.begin();
    
    dacWrite(25,0);
    ADS.setGain(0);
    f = ADS.toVoltage(1);  // voltage factor
    
    command.addCommand("ON", POWER_ON);  
    command.addCommand("OFF", POWER_OFF);
}

void loop() {
    //sensor.clearFields();
    
    command.task();
    Serial.print(SENSOR_ID);Serial.print('\t');
    Serial.print(bme.readPressure()/ 100.0F);Serial.print('\t');
    Serial.print(bme.readTemperature());Serial.print('\t');
    Serial.print(bme.readHumidity());Serial.print('\t');    
    Serial.print(ADS.readADC(0) * f, 3);Serial.print('\t');
    Serial.print(ADS.readADC(1) * f, 3);Serial.print('\t');
    Serial.print(ADS.readADC(2) * f, 3);Serial.print('\t');
    Serial.print(ADS.readADC(3) * f, 3);Serial.print('\t');
    Serial.println();
    delay(interval);
}




void ADSInit(){
    Serial.println(__FILE__);
    Serial.print("ADS1X15_LIB_VERSION: ");
    Serial.println(ADS1X15_LIB_VERSION);
    //Wire.begin( sda, scl);
}
void BME280Init(){
  while(!bme.begin(BME280_ADDR)){
    Serial.println("Could not find BME280 sensor!");
    delay(5000);
  }
}
