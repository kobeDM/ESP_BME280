// ESP32 + BME280 sensor post program (serial version)
// S.Higashino
// thanks to Ref. https://www.mgo-tec.com/blog-entry-bosch-driver-bme280-m5stack-esp32.html/3

#include <Wire.h>
#include <Adafruit_BME280.h> // BME280 driver https://github.com/adafruit/Adafruit_BME280_Library

// data post setting
#define SENSOR_ID "ESP32-KUBEAM-01"

//I2C pin setting
const int sda = 27;
const int scl = 26;

static const uint8_t BME280_ADDR   = 0x76; //default value for BME280

Adafruit_BME280 bme; // I2C

void setup() {
  Serial.begin(115200);
  Wire.begin( sda, scl, 100000 );
  BME280Init();
}

void loop() {
  BME280dataGet();
  delay(1000);
}

void BME280Init(){
  while(!bme.begin(BME280_ADDR)){
    Serial.println("Could not find BME280 sensor!");
    delay(5000);
  }
}

void BME280dataGet(){
  float temperature = bme.readTemperature();
  float pressure = bme.readPressure() / 100.0F;
  float humidity = bme.readHumidity();

  String pres_str = "";
  String temp_str = "";
  String hum_str = "";
  char temp_c[10], hum_c[10], pres_c[10];
 
  if( pressure == 0 ){
    pres_str = "??";
  }else{ //sprintf Arduino-ESP32 only
    sprintf( pres_c, "%6.1f", pressure ); 
    pres_str = String(pres_c);
  }
 
  if( temperature == -100 ){
    temp_str = "??";
  }else{
    sprintf( temp_c, "%4.1f", temperature ); 
    temp_str = String(temp_c);
  }
 
  if( humidity == -1 ){
    hum_str = "??";
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
  Serial.print( "\n" );
}
