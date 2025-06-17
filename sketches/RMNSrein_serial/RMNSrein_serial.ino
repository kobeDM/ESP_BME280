/**
"RMNSrein" ADS51115+BME280 I2C reader serial output only
K. Miuchi 2024 July
**/

#include <Wire.h>
#include <ADS1X15.h> //RobTillaart/ADS1X15
#include <Adafruit_BME280.h> // BME280 driver https://github.com/adafruit/Adafruit_BME280_Library

// data post setting
#define SENSOR_ID "RMNS-12" // name your sensor ID

//I2C pin setting
const int scl = 26; //fixed by RMNS harware
const int sda = 27; //fixed by RMNS harware

//I2C address
static const uint8_t BME280_ADDR   = 0x76; //default value for BME280
static const uint8_t ADS_ADDR   = 0x48; // fixed by RMNSrein harware (ADRR to GND)

//modules
ADS1115 ADS(ADS_ADDR);
Adafruit_BME280 bme; // I2C

// serial
const int baud  = 115200;

//moinitor
const int interval =1000; //ms

void setup() {
  Serial.begin(baud);
  Wire.begin( sda, scl, 100000 );
  BME280Init();  
  }

void loop() {
  GetBME();  
  GetADS(); 
  delay(interval);
}


void ADSInit(){
  Serial.println(__FILE__);
  Serial.print("ADS1X15_LIB_VERSION: ");
  Serial.println(ADS1X15_LIB_VERSION);
  ADS.begin();
  //Wire.begin( sda, scl);
}

void BME280Init(){
  while(!bme.begin(BME280_ADDR)){
    Serial.println("Could not find BME280 sensor!");
    delay(5000);
  }
}

void GetADS(){
  String adc0_str = "";
  String adc1_str = "";
  String adc2_str = "";
  String adc3_str = "";
  char adc0_c[10], adc1_c[10], adc2_c[10], adc3_c[10]; 

  ADS.setGain(0);
  float f = ADS.toVoltage(1);  // voltage factor
  float adc0 = ADS.readADC(0) * f;
  float adc1 = ADS.readADC(1) * f;
  float adc2 = ADS.readADC(2) * f;
  float adc3 = ADS.readADC(3) * f;
  sprintf( adc0_c, "%2.3f", adc0 ); 
  adc0_str = String(adc0_c);
  sprintf( adc1_c, "%2.3f", adc1 ); 
  adc1_str = String(adc1_c);
  sprintf( adc2_c, "%2.3f", adc2 ); 
  adc2_str = String(adc2_c);
  sprintf( adc3_c, "%2.3f", adc3 ); 
  adc3_str = String(adc3_c);
  Serial.print( adc0_str );
  Serial.print( "\t" );
  Serial.print( adc1_str );
  Serial.print( "\t" );
  Serial.print( adc2_str );
  Serial.print( "\t" );
  Serial.print( adc3_str );
  Serial.print( "\n" );
}

void GetBME(){  
  String pres_str = "";
  String temp_str = "";
  String hum_str = "";
  char temp_c[10], hum_c[10], pres_c[10];
  float temperature = bme.readTemperature();
  float pressure = bme.readPressure() / 100.0F;
  float humidity = bme.readHumidity();
  
  if( pressure == 0 ){
    pres_str = "0";
  }else{ //sprintf Arduino-ESP32 only
    sprintf( pres_c, "%6.1f", pressure ); 
    pres_str = String(pres_c);
  }
 
  if( temperature == -100 ){
    temp_str = "-100";
  }else{
    sprintf( temp_c, "%4.1f", temperature ); 
    temp_str = String(temp_c);
  }

  if( humidity == -1 ){
    hum_str = "-1";
  }else{
    sprintf( hum_c, "%5.1f", humidity ); 
    hum_str = String(hum_c);
  }
  Serial.print( SENSOR_ID"\t");
  Serial.print( pres_str);
  Serial.print( "\t" );
  Serial.print( temp_str );
  Serial.print( "\t" );
  Serial.print( hum_str );
  Serial.print( "\t" );
}
