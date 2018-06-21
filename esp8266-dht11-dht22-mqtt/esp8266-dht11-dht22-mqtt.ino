// Required libraries:
//  - Adafruit Unified Sensor (required for DHT)
//  - DHT Sensor Library
//  - NTPClient
//  - PubSubClient (for MQTT)

#include <DHT.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <PubSubClient.h>

#define MQTT_HOST "*"
#define MQTT_PORT 1883
#define MQTT_USER "*"
#define MQTT_PASS "*"
#define MQTT_CLIENT "ESP8266-dht11-dht22"
#define MQTT_TOPIC_H1 "sensors/hum3" 
#define MQTT_TOPIC_T1 "sensors/temp3"
#define MQTT_TOPIC_H2 "sensors/hum4" 
#define MQTT_TOPIC_T2 "sensors/temp4"

#define WIFI_SSID "*"
#define WIFI_PASS "*"

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);

WiFiClient espClient;
PubSubClient client(espClient); 

#define REPORT_DELAY_SECS 10

DHT dht11(D2, DHT11); //DHT11 wired to D2
DHT dht22(D3, DHT22); //DHT22 wired to D3

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

  timeClient.begin();

  client.setServer(MQTT_HOST, MQTT_PORT); 
  
  dht11.begin();
  dht22.begin();
}

void loop() {

  delay(REPORT_DELAY_SECS * 1000);
  timeClient.update();

  if (!client.connected()) {
    mqttConnect();
  }
  client.loop(); 

  float h1 = dht11.readHumidity();
  float t1 = dht11.readTemperature();
  if (isnan(h1) || isnan(t1)) {
    Serial.println("Failed to read from DHT11 sensor!");
    return;
  }
  log("DHT11 Humidity:    ", h1);
  log("DHT11 Temperature: ", t1);

  client.publish(MQTT_TOPIC_H1, String(h1).c_str(), true);
  client.publish(MQTT_TOPIC_T1, String(t1).c_str(), true); 
  
  float h2 = dht22.readHumidity();
  float t2 = dht22.readTemperature();
  if (isnan(h2) || isnan(t2)) {
    Serial.println("Failed to read from DHT22 sensor!");
    return;
  }
  log("DHT22 Humidity:    ", h2);
  log("DHT22 Temperature: ", t2);

  client.publish(MQTT_TOPIC_H2, String(h2).c_str(), true);
  client.publish(MQTT_TOPIC_T2, String(t2).c_str(), true); 

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

void mqttConnect() {

  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(MQTT_CLIENT, MQTT_USER, MQTT_PASS)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
