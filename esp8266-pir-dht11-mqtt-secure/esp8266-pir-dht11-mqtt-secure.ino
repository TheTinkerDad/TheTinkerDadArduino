// Required libraries:
//  - Adafruit Unified Sensor (required for DHT)
//  - PubSubClient (for MQTT)

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <DHT.h>

#include "settings.h"

DHT dht11(D3, DHT11);
WiFiClientSecure espClient;
PubSubClient client(espClient); 
int sensorState = 0;
int oldSensorState = 0;

#define REPORT_DELAY_SECS 10

void setup() {
  
  Serial.begin(115200);
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);

  pinMode(D5, OUTPUT);
  digitalWrite(D5, LOW);

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  WiFi.mode(WIFI_STA); 

  while ( WiFi.status() != WL_CONNECTED ) {
    digitalWrite(D5, HIGH);
    delay ( 250 );
    digitalWrite(D5, LOW);
    delay ( 250 );
    Serial.print ( "." );
  }

  Serial.println("OK.");
  Serial.print("Client IP: ");
  Serial.println(WiFi.localIP());

  espClient.connect(MQTT_HOST, MQTT_PORT);

  if (espClient.verify(MQTT_HOST_SHA1, MQTT_HOST)) {
    Serial.println("Established secure connection.");
  } else {
    Serial.println("Couldn't verify MQTT broker fingerprint! Server compromised?");
  }

  client.setServer(MQTT_HOST, MQTT_PORT); 
  mqttConnect();

  attachInterrupt(digitalPinToInterrupt(D2), IntCallback, CHANGE);

  dht11.begin(); 
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
      client.publish(MQTT_TOPIC_M, "on", true);
    } else {
      digitalWrite(D5, LOW);
      client.publish(MQTT_TOPIC_M, "off", true);
    }
    oldSensorState = sensorState;
    attachInterrupt(digitalPinToInterrupt(D2), IntCallback, CHANGE);
  } else {
    if (!(millis() % (REPORT_DELAY_SECS * 1000))) {
      float h1 = dht11.readHumidity();
      float t1 = dht11.readTemperature();
      if (isnan(h1) || isnan(t1)) {
        Serial.println("Failed to read from DHT11 sensor!");
        return;
      }   
      Serial.print("Humidity: ");
      Serial.println(h1);
      client.publish(MQTT_TOPIC_H, String(h1).c_str(), true);
      Serial.print("Temperature: ");
      Serial.println(t1);
      client.publish(MQTT_TOPIC_T, String(t1).c_str(), true);
    }
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
