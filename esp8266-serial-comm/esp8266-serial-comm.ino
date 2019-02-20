#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#include "settings.h"

int incomingByte = 0;
WiFiClient espClient;
PubSubClient client(espClient);
HTTPClient http;
 
void setup() {
 
  Serial.begin(9600);

  client.setServer(MQTT_HOST, MQTT_PORT);
  client.setCallback(callback);

  connectIfNeeded();

  client.subscribe(MQTT_TOPIC); 
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
  
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect("ESP8266Client", MQTT_USER, MQTT_PASS )) {
      Serial.println(" Connected");  
    } else {
      Serial.print(" Failed with state ");
      Serial.println(client.state());
      delay(2000);
    }
  }
}
 
void callback(char* topic, byte* payload, unsigned int length) {
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void loop() {

  connectIfNeeded();

  client.loop();

  char buffer[READ_BUF_SIZE];
  if ((incomingByte = Serial.available()) > 0) {
    memset(buffer, 0, READ_BUF_SIZE);
    Serial.readBytesUntil('\n', buffer, incomingByte);
    client.publish(MQTT_TOPIC, buffer);
  }

  // get data over rest
  http.begin(NETDATA_URL);
  http.GET();
  String payload = http.getString();
  DynamicJsonBuffer  jsonBuffer(200);
  JsonObject& root = jsonBuffer.parseObject(payload);
  if (!root.success()) {
    Serial.println("parseObject() failed");
    return;
  }

  int uptime_secs_t = root["data"][0][1];
  int uptime_hours = uptime_secs_t / 60 / 60;
  int uptime_mins = (uptime_secs_t - uptime_hours * 60 * 60) / 60;
  int uptime_secs = (uptime_secs_t - uptime_hours * 60 * 60 - uptime_mins * 60);
  
  char buf[256];
  sprintf(buf, "Uptime: %0d:%0d:%0d", uptime_hours, uptime_mins, uptime_secs);
  Serial.println(buf);
  
  delay(1000);
}
