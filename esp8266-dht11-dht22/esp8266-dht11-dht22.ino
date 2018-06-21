// Required libraries:
//  - Adafruit Unified Sensor (required for DHT)
//  - DHT Sensor Library
//  - NTPClient

#include <DHT.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const char *ssid     = "*";
const char *password = "*";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);

#define REPORT_DELAY_SECS 10

DHT dht11(D2, DHT11); //DHT11 wired to D2
DHT dht22(D3, DHT22); //DHT22 wired to D3

void setup() {
  
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }

  timeClient.begin();
  dht11.begin();
  dht22.begin();
}

void loop() {

  delay(REPORT_DELAY_SECS * 1000);
  timeClient.update();

  float h1 = dht11.readHumidity();
  float t1 = dht11.readTemperature();
  if (isnan(h1) || isnan(t1)) {
    Serial.println("Failed to read from DHT11 sensor!");
    return;
  }
  log("DHT11 Humidity:    ", h1);
  log("DHT11 Temperature: ", t1);

  float h2 = dht22.readHumidity();
  float t2 = dht22.readTemperature();
  if (isnan(h2) || isnan(t2)) {
    Serial.println("Failed to read from DHT22 sensor!");
    return;
  }
  log("DHT22 Humidity:    ", h2);
  log("DHT22 Temperature: ", t2);

  log("------------------------", NULL);
}

void log(char * msg, float value) {

  Serial.print(timeClient.getFormattedTime());
  Serial.print(" ");
  Serial.print(msg);
  if (value != NULL) {
    Serial.print(value);
  }
  Serial.println();
}

