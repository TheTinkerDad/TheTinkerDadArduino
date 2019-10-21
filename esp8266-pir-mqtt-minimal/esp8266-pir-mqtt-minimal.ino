#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "settings.h"

#define DEVICE_ID "camera-01"

WiFiClient espClient;
PubSubClient client(espClient); 
int sensorState = 0;
int oldSensorState = 0;

void setup() {
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  WiFi.mode(WIFI_STA); 
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
  }
  client.setServer(MQTT_HOST, MQTT_PORT); 
  mqttConnect();
  attachInterrupt(digitalPinToInterrupt(D2), IntCallback, CHANGE);
}

void IntCallback() {
  if (digitalRead(D2) == HIGH) {
    sensorState = 1;
  } else {
    sensorState = 0;
  }
}

void loop() {
  client.loop(); 
  if (sensorState != oldSensorState) {
    if (sensorState) {
      detachInterrupt(digitalPinToInterrupt(D2));
      mqttConnect();    
      client.publish(MQTT_TOPIC, DEVICE_ID, true);
      attachInterrupt(digitalPinToInterrupt(D2), IntCallback, CHANGE);
    }
    oldSensorState = sensorState;
  }
  delay(100);
}

void mqttConnect() {

  while (!client.connected()) {
    if (!client.connect(MQTT_CLIENT)) {
      Serial.print("MQTT Connection Failed, code=");
      Serial.print(client.state());
      Serial.println(" retrying in 5 seconds...");
      delay(5000);
    }
  }
}
