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
    WiFi.mode(WIFI_AP_STA);
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

void getUpTime(const char * url, char * buf) {
  
  JsonObject& root = getJson(url, 200);
  if (!root.success()) {
    return;
  }

  long uptime_secs_t = root["data"][0][1];
  int uptime_hours = uptime_secs_t / 60 / 60;
  int uptime_mins = (uptime_secs_t - uptime_hours * 60 * 60) / 60;
  int uptime_secs = (uptime_secs_t - uptime_hours * 60 * 60 - uptime_mins * 60);
  
  sprintf(buf, "Uptime:       %02d:%02d:%02d", uptime_hours, uptime_mins, uptime_secs);
}

void getCPUUsage(const char * url, char * buf) {
  
  JsonObject& root = getJson(url, 455);
  if (!root.success()) {
    return;
  }

  char float_tmp[7];
  JsonArray& data_0 = root["data"][0];
  dtostrf((float)data_0[4] + (float)data_0[5] + (float)data_0[6] + (float)data_0[7] + (float)data_0[8] + (float)data_0[9], 3, 2, float_tmp);
  
  strcpy(buf, "CPU Usage:    ");
  strcat(buf, float_tmp);
  strcat(buf, "%  ");
}

char *labels[4] = {
  "RAM Free:    ", 
  "RAM Used:    ", 
  "RAM Cached:  ", 
  "RAM Buffers: "
};
void getRAMUsage(const char * url, char * buf) {
  
  JsonObject& root = getJson(url, 455);
  if (!root.success()) {
    return;
  }

  JsonArray& data_0 = root["data"][0];

  strcpy(buf, "");
  for (int i = 1; i <= 4; i++) {
    char float_tmp[9];
    float ram = data_0[i];  
    dtostrf(ram, 6, 2, float_tmp);
    strcat(buf, labels[i-1]);
    strcat(buf, " ");
    strcat(buf, float_tmp);
    strcat(buf, "   \n");
  }
}

char * tmp;
void loop() {

  connectIfNeeded();

  char * buf = (char*)malloc(sizeof(char) * 256);
  
  strcpy(buf, ">");
  
  tmp = (char*)malloc(sizeof(char) * 32);  
  getUpTime(NETDATA_URL_UPTIME, tmp);
  strcat(buf, tmp);
  free(tmp);
  
  strcat(buf, "\n");
  
  tmp = (char*)malloc(sizeof(char) * 32);  
  getCPUUsage(NETDATA_URL_CPU, tmp);
  strcat(buf, tmp);
  free(tmp);
  
  strcat(buf, "\n");
  
  tmp = (char*)malloc(sizeof(char) * 128);
  getRAMUsage(NETDATA_URL_RAM, tmp);
  strcat(buf, tmp);
  free(tmp);
  
  strcat(buf, "<");

  if (strlen(buf) < 16) {
    WiFi.disconnect();
  } else {
    int i = 0;
    while (i < strlen(buf)) {
      i+=16;
      Serial.write(&buf[i-16], (i > strlen(buf) ? strlen(buf) % 16 : 16));
      delay(200);
    }
  }

  free(buf);
  
  delay(1000);
}
