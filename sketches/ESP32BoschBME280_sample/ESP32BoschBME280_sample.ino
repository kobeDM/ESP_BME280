// ESP32 + BME280 sensor post program
// H.Ishiura
// thanks to Ref. https://www.mgo-tec.com/blog-entry-bosch-driver-bme280-m5stack-esp32.html/3

#include <Wire.h>
//#include <WiFi.h>
#include <ESPmDNS.h>
//#include <WiFiUdp.h>
#include <HTTPClient.h>

// required by sensor 
#include "bme280.h" //BOSCH純正ドライバ

// data post setting
#define SENSOR_ID "ESP32-miraclue-01"
#define DBTAG "/sensor.log" // specify tag 
#define FLUENTD_IP "10.37.0.222"
#define FLUENTD_PORT 8888

// wifi setting
const char* ssid = "pproomg";
const char* password = "kobeppphysics";

//I2C pin setting
const int sda = 27;
const int scl = 26;
 
struct bme280_dev dev;
struct bme280_data comp_data;


void setup() {
  Serial.begin(115200);
//  WiFiSetup();
  BoschBME280init();
}

void loop() {
  BoschBME280dataGet();
  delay(1000);
}

void WiFiSetup() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}


void BoschBME280init(){
  Wire.begin( sda, scl, 400000 );
  dev.dev_id = BME280_I2C_ADDR_PRIM; //0x76
  //id.dev_addr = BME280_I2C_ADDR_PRIM;


  dev.intf = BME280_I2C_INTF; //I2C interface
  dev.read = user_i2c_read; //
  dev.write = user_i2c_write; //
  dev.delay_ms = user_delay_ms; //
  
  int8_t rslt = bme280_init( &dev );
  Serial.printf( "bme280_init rslt=%d\r\n", rslt );
 
  dev.settings.osr_h = BME280_OVERSAMPLING_1X;
  dev.settings.osr_p = BME280_OVERSAMPLING_16X;
  dev.settings.osr_t = BME280_OVERSAMPLING_2X;
  dev.settings.filter = BME280_FILTER_COEFF_16;
  dev.settings.standby_time = BME280_STANDBY_TIME_62_5_MS; //62.5ms
 
  uint8_t settings_sel;
  settings_sel = BME280_OSR_PRESS_SEL;
  settings_sel |= BME280_OSR_TEMP_SEL;
  settings_sel |= BME280_OSR_HUM_SEL;
  settings_sel |= BME280_STANDBY_SEL;
  settings_sel |= BME280_FILTER_SEL;
  rslt = bme280_set_sensor_settings( settings_sel, &dev );
 // Serial.printf( "sensor settings rslt=%d\r\n",rslt );
  rslt = bme280_set_sensor_mode(BME280_NORMAL_MODE, &dev );
 // Serial.printf( "sensor mode rslt=%d\r\n", rslt );
}

void BoschBME280dataGet(){
  dev.delay_ms( 1000 ); // every 1 sec data taking
  int8_t rslt = bme280_get_sensor_data( BME280_ALL, &comp_data, &dev );
  //Serial.printf( "bme280_get_sensor_data rslt=%d\r\n", rslt );
  print_sensor_data( &comp_data );
}
//******************************
void user_delay_ms( uint32_t period ){
  delay( period );
}
//******************************
int8_t user_i2c_read( uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len ){
  int8_t rslt_read = 0; /* Return 0 for Success, non-zero for failure */
 
  Wire.beginTransmission( dev_id );
  Wire.write( reg_addr );
  Wire.endTransmission();
  uint8_t req_from_ret = Wire.requestFrom( dev_id, (uint8_t)len, true );
 
  if( req_from_ret == 0 ){
    //Wire.reset(); 
    //Serial.println("@@@@@@@@@@@@@@@@ Wire Reset! @@@@@@@@@@@@@@");
    rslt_read = 1;
  }else{
    for( int i = 0; i < len; i++ ){
      reg_data[i] = Wire.read();
    }
    rslt_read = 0;
  }
 
  return rslt_read;
}
//******************************
int8_t user_i2c_write( uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len ){
  int8_t rslt_write = 0; /* Return 0 for Success, non-zero for failure */
 
  Wire.beginTransmission( dev_id );
  Wire.write( reg_addr );
  for( int i = 0; i < len; i++ ){
    Wire.write( reg_data[i] );
  }
  rslt_write = Wire.endTransmission();
 
  return rslt_write;
}
//******************************


void print_sensor_data(struct bme280_data *comp_data){
  //BME280_FLOAT_ENABLE
 
  float temperature = (float)comp_data->temperature;
  float pressure = (float)comp_data->pressure/100.0;
  float humidity = (float)comp_data->humidity;
  if( temperature > 100 || temperature < -9 ) temperature = -100;
  if( pressure > 2000 || pressure < 700 ) pressure = 0;
  if( humidity > 100 || humidity < 0 ) humidity = -1;
  char temp_c[10], hum_c[10], pres_c[10];
 
  String pres_str = "";
  String temp_str = "";
  String hum_str = "";
 
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

//  postToFluentd(temperature, humidity, pressure);
}

bool postToFluentd(float ftemp, float fhumid, float fpress){
  HTTPClient http;

  http.begin(FLUENTD_IP, FLUENTD_PORT, DBTAG);
  delay(1000);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  
  char temp_c[10], hum_c[10], pres_c[10];
  //ftemp = 50.5;
  sprintf(temp_c, "%2.1f ℃", ftemp);
  sprintf(hum_c, "%3.1f ％", fhumid);
  sprintf(pres_c, "%4.1f hPa", fpress);
  
  char jsonStr[200];
  sprintf(jsonStr, "{\"sensor_id\": \"%s\", \"temp\": %2.1f,  \"humi\": %3.1f, \"pres\": %4.1f}", SENSOR_ID, ftemp, fhumid, fpress);
  
  Serial.println(jsonStr);
  int code = http.POST(jsonStr); // POST data to na22
  //int code = HTTP_CODE_OK;
  http.end();

  if (code != HTTP_CODE_OK) {
//    Serial.println("NOT HTTP_CODE_OK");
    return false;
  }
  //Serial.println("HTTP_CODE_OK");
  return true;
}
