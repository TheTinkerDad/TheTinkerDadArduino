#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define READ_BUF_SIZE 256

int incomingByte = 0;
const char* ssid = "*";
const char* password =  "*";
const char* mqttServer = "*";
const int mqttPort = 1883;
const char* mqttUser = "*";
const char* mqttPassword = "*";
 
WiFiClient espClient;
PubSubClient client(espClient);
 
void setup() {
 
  Serial.begin(9600);

  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  connectIfNeeded();

  client.subscribe("test/hello"); 
}

void connectIfNeeded() {

  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.println("Connecting to WiFi..");
    }
    Serial.println("Connected to the WiFi network");
  }
  
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {
      Serial.println("connected");  
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
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
    client.publish("test/hello", buffer);
  }
  delay(100);
}

