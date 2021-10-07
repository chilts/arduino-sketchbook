// From : https://www.emqx.com/en/blog/esp8266-connects-to-the-public-mqtt-broker

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Wifi (create a 'wifi-credentials.h' file)
// const char *ssid = "..."; // Enter your WiFi name
// const char *password = "...";  // Enter WiFi password
#include "wifi-credentials.h"

// MQTT Broker (create a 'mqtt-credentials.h' file)
// const char *mqtt_broker = "mqtt.example.com";
// const char *topic = "...";
// const char *mqtt_username = "...";
// const char *mqtt_password = "...";
// const int mqtt_port = 1883;
#include "mqtt-credentials.h"

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  // Set software serial baud to 115200;
  Serial.begin(115200);
  // connecting to a WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
  //connecting to a mqtt broker
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  while (!client.connected()) {
      String client_id = "esp8266-client-";
      client_id += String(WiFi.macAddress());
      Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
      if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
          Serial.println("Public emqx mqtt broker connected");
      } else {
          Serial.print("failed with state ");
          Serial.print(client.state());
          delay(2000);
      }
  }
  // publish and subscribe
  client.publish(topic, "hello emqx");
  client.subscribe(topic);
}

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
      Serial.print((char) payload[i]);
  }
  Serial.println();
  Serial.println("-----------------------");
}

void loop() {
  client.loop();
}
