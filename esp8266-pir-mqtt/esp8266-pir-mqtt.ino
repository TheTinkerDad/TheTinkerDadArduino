// Required libraries:
//  - Adafruit Unified Sensor (required for DHT)
//  - PubSubClient (for MQTT)

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include "settings.h"

WiFiClient espClient;
PubSubClient client(espClient); 
int sensorState = 0;
int oldSensorState = 0;

#define REPORT_DELAY_SECS 10

void setup() {
  
  Serial.begin(115200);
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  WiFi.mode(WIFI_STA); 

  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }

  client.setServer(MQTT_HOST, MQTT_PORT); 
  mqttConnect();

  pinMode(D5, OUTPUT);
  digitalWrite(D5, LOW);
  attachInterrupt(digitalPinToInterrupt(D2), IntCallback, CHANGE);
}

void IntCallback()
{
  if (digitalRead(D2) == HIGH) {
    sensorState = 1;
  } else {
    sensorState = 0;
  }
}

void loop() {
  
  client.loop(); 
  if (sensorState != oldSensorState) {
    detachInterrupt(digitalPinToInterrupt(D2));
    mqttConnect();    
    if (sensorState) {
      digitalWrite(D5, HIGH);
      client.publish(MQTT_TOPIC, "1", true);
    } else {
      digitalWrite(D5, LOW);
      client.publish(MQTT_TOPIC, "0", true);
    }
    oldSensorState = sensorState;
    attachInterrupt(digitalPinToInterrupt(D2), IntCallback, CHANGE);
  }
}

void mqttConnect() {

  while (!client.connected()) {
    Serial.print("Attempting MQTT connection... ");
    if (client.connect(MQTT_CLIENT, MQTT_USER, MQTT_PASS)) {
      Serial.println("connected!");
    } else {
      Serial.print("failed, code=");
      Serial.print(client.state());
      Serial.println(" retrying in 5 seconds...");
      delay(5000);
    }
  }
}
