// Required libraries:
//  - Adafruit Unified Sensor (required for DHT)
//  - PubSubClient (for MQTT)

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

#define MQTT_HOST "*"
#define MQTT_PORT 8883
#define MQTT_USER "*"
#define MQTT_PASS "*"
#define MQTT_HOST_SHA1 "*"
#define MQTT_CLIENT "MotionSensor1"
#define MQTT_TOPIC "sensors/motion1/state" 

#define WIFI_SSID "*"
#define WIFI_PASS "*"

WiFiClientSecure espClient;
PubSubClient client(espClient); 
int sensorState = 0;
int oldSensorState = 0;

#define REPORT_DELAY_SECS 10

void setup() {
  
  Serial.begin(115200);
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);


  WiFi.begin(WIFI_SSID, WIFI_PASS);
  WiFi.mode(WIFI_STA); 

  while ( WiFi.status() != WL_CONNECTED ) {
//    digitalWrite(D5, HIGH);
//    delay ( 250 );
//    digitalWrite(D5, LOW);
//    delay ( 250 );
    delay(500);
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
      client.publish(MQTT_TOPIC, "on", true);
    } else {
      digitalWrite(D5, LOW);
      client.publish(MQTT_TOPIC, "off", true);
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
