#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>

// Objeto Servo
Servo myservo;
int pos = 0;
int servoPin = 13;

// Update these with values suitable for your network.
// variables para control de led y alarmas

String stringData;
char charData[1024];

// variables para tracking
String esp_mac;
int count;

// WiFi
const char* ssid = "******";
const char* password = "*********";

// MQTT
const char *mqtt_broker = "**********";
const char *topic_alarm = "robot/brazo";
const char *mqtt_username = "qwerty";
const char *mqtt_password = "qwerty";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  myservo.attach(servoPin);
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("Connnected to the WiFi network");
  reconnected();
}

void reconnected() {
  // connecting to mqqt broker
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  while(!client.connected()) {
    String client_id = "ESP-";
    client_id += String(WiFi.macAddress());
    Serial.println(client_id);
    int str_len = client_id.length() + 1;
    char char_client[str_len];
    client_id.toCharArray(char_client, str_len);
    if (client.connect(char_client, mqtt_username, mqtt_password)) {
      Serial.println("Public emqx mqtt broker connected");
    } else {
      Serial.println("failed with state");
      Serial.print(client.state());
      delay(2000);
    }
  }
  // publish and suscribe
  client.subscribe(topic_alarm);
  Serial.println("cuando termine de reactivarse");
}

void loop() {
  if(!client.connected()) {
    Serial.println("MQTT disconnected");
    reconnected();
  }
  client.loop();
  delay(15);
  esp_mac = WiFi.macAddress();
  if (WiFi.status() != WL_CONNECTED) {
    //Serial.println("no conectado");
    ESP.restart();
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  String message;
  for (int i = 0; i < length; i++) {
    message = message + (char) payload[i];
  }
  Serial.print(message);
  myservo.write(message.toInt());
//   if (message == "true") { digitalWrite(LED, HIGH); }   // Value servomotor
  Serial.println();
  Serial.println("-------------------------");
}