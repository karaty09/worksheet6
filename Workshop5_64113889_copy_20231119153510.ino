#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <ArduinoJson.h>

const char* ssid = "IoTC605";
const char* password = "ccsadmin";
const char* mqtt_server = "192.168.0.119";
const char* mqtt_topic = "dht11"; 
const int ledPin = D6;
float temp;
float hum;
DHT dht11(D4, DHT11);

WiFiClient espClient;
PubSubClient client(espClient);

void ReadDHT11(){
  temp = dht11.readTemperature();
  hum = dht11.readHumidity();
}

void setup_wifi() {

  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  if (String(topic) == "led") {
    if (message == "on") {
      digitalWrite(ledPin, HIGH);
    } else if (message == "off") {
      digitalWrite(ledPin, LOW);
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe("led");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  pinMode(ledPin, OUTPUT);     
  Serial.begin(115200);
  dht11.begin();
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  ReadDHT11();

  IPAddress localIP = WiFi.localIP();
  StaticJsonDocument<200> jsonDocument;
  jsonDocument["humidity"] = hum;
  jsonDocument["temperature"] = temp;

  String jsonStr;
  serializeJson(jsonDocument, jsonStr);

  client.publish(mqtt_topic, jsonStr.c_str());
  Serial.println(hum);
  Serial.println(temp);
  delay(5000);
}