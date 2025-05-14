/**
"RMNSrein" ADS51115+BME280 I2C reader
K. Miuchi 2024 July
**/

#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <ADS1X15.h> //RobTillaart/ADS1X15
#include <Adafruit_BME280.h> // BME280 driver https://github.com/adafruit/Adafruit_BME280_Library
#include <InfluxDbClient.h> // InfluxDB libraries https://github.com/tobiasschuerg/InfluxDB-Client-for-Arduino/

// data post setting
#define SENSOR_ID "RMNS-XX" // name your sensor ID

// InfluxDB server url
#define INFLUXDB_URL "http://10.37.0.227:8086"
#define INFLUXDB_DB_NAME "RMNS" // influxdb database name
Point sensor("RMNS"); // influxdb measurement name

// wifi setting
const char* ssid = "pproomg";
const char* password = "kobeppphysics";

//I2C pin setting
const int scl = 26; //fixed by RMNS harware
const int sda = 27; //fixed by RMNS harware

//I2C address
static const uint8_t BME280_ADDR   = 0x76; //default value for BME280
static const uint8_t ADS_ADDR   = 0x48; // fixed by RMNSrein harware (ADRR to GND)

//modules
ADS1115 ADS(ADS_ADDR);
Adafruit_BME280 bme; // I2C
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_DB_NAME); // influxdb client

// serial
const int baud  = 115200;

//moinitor
const int interval =1000; //ms

void setup() {
  Serial.begin(baud);
  WiFiInit();
  Wire.begin( sda, scl, 400000 );
  BME280Init();  
  InfluxServerInit();
}

void loop() {
  sensor.clearFields();
  GetADS(); 
  GetBME();
  Post(); 
  delay(interval);
}

void InfluxServerInit(){
  sensor.addTag("sensor_id", SENSOR_ID);
  client.setConnectionParamsV1(INFLUXDB_URL, INFLUXDB_DB_NAME) ;
}

String wifiStatusToString(wl_status_t status) {
  switch (status) {
    case WL_IDLE_STATUS:     return "IDLE";
    case WL_NO_SSID_AVAIL:   return "SSID Not Available";
    case WL_SCAN_COMPLETED:  return "Scan Completed";
    case WL_CONNECTED:       return "Connected";
    case WL_CONNECT_FAILED:  return "Connect Failed";
    case WL_CONNECTION_LOST: return "Connection Lost";
    case WL_DISCONNECTED:    return "Disconnected";
    default:                 return "Unknown";
  }
}

void WiFiInit() {
  Serial.println(WiFi.macAddress());
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    Serial.println(wifiStatusToString(WiFi.status()));
    delay(5000);
    ESP.restart();
  }
  Serial.println("WiFi Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
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
  ADS.setGain(0);
  float f = ADS.toVoltage(1);  // voltage factor
  float adc0 = ADS.readADC(0) * f;
  float adc1 = ADS.readADC(1) * f;
  float adc2 = ADS.readADC(2) * f;
  float adc3 = ADS.readADC(3) * f;
  sensor.addField("ADC_0", adc0); 
  sensor.addField("ADC_1", adc1);
  sensor.addField("ADC_2", adc2);
  sensor.addField("ADC_3", adc3);
}

void GetBME(){  
  sensor.addField("temp", bme.readTemperature());
  sensor.addField("pres", bme.readPressure() / 100.0F);
  sensor.addField("humi", bme.readHumidity());
}

void Post(){
  Serial.println(client.pointToLineProtocol(sensor));
  //Write point
  if (!client.writePoint(sensor)) {
    Serial.print("InfluxDB write failed: ");
   Serial.println(client.getLastErrorMessage());
  }
}
