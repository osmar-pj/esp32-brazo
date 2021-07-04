#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Servo.h>

// Objeto Servo
Servo myservo;
int pos = 0;

// Update these with values suitable for your network.
// variables para control de led y alarmas
int IN_PRESENCE_1 = 4;    // D2
int IN_DOOR_1 = 5;        // D1
//int IN_PRESENCE_2 = 14;
//int IN_DOOR_2 = 12;
int thisState_1 = 0;
int lastState_1 = 0;
int thisState_2 = 0;
int lastState_2 = 0;
String stringData;
char charData[1024];

// variables para tracking
String esp_mac;
int count;

// WiFi
const char* ssid = "CLARO LIZ - 2.4ghz";
const char* password = "liz.jove73031792";

// MQTT
const char *mqtt_broker = "143.198.128.180";
const char *topic_alarm = "peru/arequipa/hunter/palomino_florez/piso_1";
const char *mqtt_username = "admin";
const char *mqtt_password = "public";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {

  pinMode(IN_DOOR_1, INPUT);
  pinMode(IN_PRESENCE_1, INPUT);
  myservo.attach(13);
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
  //client.subscribe(topic_alarm_presence);
  Serial.println("cuando termine de reactivarse");
}

void loop() {
  if(!client.connected()) {
    Serial.println("MQTT disconnected");
    reconnected();
  }
  client.loop();
  myservo.write(50);
  delay(15);
  esp_mac = WiFi.macAddress();
  thisState_1 = digitalRead(IN_PRESENCE_1);
  delay(100);
  thisState_2 = digitalRead(IN_DOOR_1);
  delay(300);
  char alarm[300];
  StaticJsonDocument<100> doc;
  if (thisState_1 != lastState_1 || thisState_2 != lastState_2) {
    lastState_1 = thisState_1;
    lastState_2 = thisState_2;
    doc["alarm"]["esp"] = esp_mac;
    doc["alarm"]["presence"] = String(thisState_1);
    doc["alarm"]["door"] = String(thisState_2);
    size_t m = serializeJson(doc, alarm);
    serializeJson(doc, Serial);
    client.publish(topic_alarm, alarm);
    delay(1000);
  }
  if (WiFi.status() != WL_CONNECTED) {
    //Serial.println("no conectado");
    ESP.restart();
  }
}
