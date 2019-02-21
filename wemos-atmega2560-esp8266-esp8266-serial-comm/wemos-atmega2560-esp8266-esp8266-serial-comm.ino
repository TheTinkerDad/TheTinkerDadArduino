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

JsonObject& getJson(const char * url, int bufferSize) {
  
  // get data over rest - repeat this for whatever endpoints you want to fetch and display
  http.begin(url);
  http.GET();
  String payload = http.getString();
  DynamicJsonBuffer jsonBuffer(bufferSize);
  return jsonBuffer.parseObject(payload);
}

char* getUpTime(const char * url) {
  
  JsonObject& root = getJson(url, 200);
  if (!root.success()) {
    return ">Service unavailable!<";
  }

  int uptime_secs_t = root["data"][0][1];
  int uptime_hours = uptime_secs_t / 60 / 60;
  int uptime_mins = (uptime_secs_t - uptime_hours * 60 * 60) / 60;
  int uptime_secs = (uptime_secs_t - uptime_hours * 60 * 60 - uptime_mins * 60);
  
  char * buf = (char*)malloc(sizeof(char) * 32);
  sprintf(buf, ">Uptime: %0d:%0d:%0d<", uptime_hours, uptime_mins, uptime_secs);
  return buf;
}

char* getCPUUsage(const char * url) {
  
  JsonObject& root = getJson(url, 455);
  if (!root.success()) {
    return ">Service unavailable!<";
  }

  char float_tmp[7];
  JsonArray& data_0 = root["data"][0];
  dtostrf((float)data_0[4] + (float)data_0[5] + (float)data_0[6] + (float)data_0[7] + (float)data_0[8] + (float)data_0[9], 3, 2, float_tmp);
  
  char * buf = (char*)malloc(sizeof(char) * 32);
  
  strcpy(buf, ">CPU: ");
  strcat(buf, float_tmp);
  strcat(buf, "%<");

  return buf;
}

char *labels[4] = {"RAM Free:   ", "RAM Used:   ", "RAM Cached: ", "RAM Buffers:"};
char* getRAMUsage(const char * url) {
  
  JsonObject& root = getJson(url, 455);
  if (!root.success()) {
    return ">Service unavailable!<";
  }

  JsonArray& data_0 = root["data"][0];

  char * buf = (char*)malloc(sizeof(char) * 256);
  strcpy(buf, "");
  for (int i = 1; i <= 4; i++) {
    char float_tmp[8];
    float ram = data_0[i];  
    dtostrf(ram, 4, 2, float_tmp);
    strcat(buf, ">");
    strcat(buf, labels[i-1]);
    strcat(buf, " ");
    strcat(buf, float_tmp);
    strcat(buf, "%<\n");
  }

  return buf;
}


void loop() {

  connectIfNeeded();

  Serial.println(getUpTime(NETDATA_URL_UPTIME));
  Serial.println(getCPUUsage(NETDATA_URL_CPU));
  Serial.println(getRAMUsage(NETDATA_URL_RAM));
  
  delay(2000);
}
