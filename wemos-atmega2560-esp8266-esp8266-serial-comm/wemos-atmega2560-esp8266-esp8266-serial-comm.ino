#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#include "settings.h"

int incomingByte = 0;
WiFiClient espClient;
HTTPClient http;
 
void setup() {
 
  Serial.begin(9600);
  connectIfNeeded();
}

void connectIfNeeded() {

  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.println("Connecting to WiFi..");
    }
    Serial.println("Connected to the WiFi network");
  }
}

JsonObject& getJson(const char * url) {
  
  // get data over rest - repeat this for whatever endpoints you want to fetch and display
  http.begin(url);
  http.GET();
  String payload = http.getString();
  DynamicJsonBuffer jsonBuffer(200);
  return jsonBuffer.parseObject(payload);
}

char * getUpTime(const char * url) {
  
  JsonObject& root = getJson(url);
  if (!root.success()) {
    return ">Service unavailable!<";
  }

  int uptime_secs_t = root["data"][0][1];
  int uptime_hours = uptime_secs_t / 60 / 60;
  int uptime_mins = (uptime_secs_t - uptime_hours * 60 * 60) / 60;
  int uptime_secs = (uptime_secs_t - uptime_hours * 60 * 60 - uptime_mins * 60);
  
  char buf[256];
  sprintf(buf, ">Uptime: %0d:%0d:%0d<", uptime_hours, uptime_mins, uptime_secs);
  return buf;
}

char * getCPUUsage(const char * url) {
  
  JsonObject& root = getJson(url);
  if (!root.success()) {
    return ">Service unavailable!<";
  }

  // TODO: Parse and return!
  float value = 0.0;
  
  char buf[32];
  sprintf(buf, ">CPU: %00d%%<", value);
  return buf;
}


void loop() {

  connectIfNeeded();

  Serial.println(getUpTime(NETDATA_URL_UPTIME));
  //Serial.println(getCPUUsage(NETDATA_URL_CPU));
  
  delay(1000);
}
