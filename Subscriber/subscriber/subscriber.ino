#include <WiFiNINA.h>
#include <PubSubClient.h>
#include "Secrets.h"
#include "functions.h"

// WiFi and MQTT Configuration
const char* WIFI_SSID = SECRET_SSID;     // Replace with your WiFi SSID
const char* WIFI_PASS = SECRET_PASS;     // Replace with your WiFi Password
const char* MQTT_BROKER = BROKER;
const int MQTT_PORT = PORT;
const char* TOPIC_WAVE = "SIT210/wave";  // Topic for publishing wave detection
const char* TOPIC_PAT = "SIT210/pat";    // Topic for publishing pat detection

// LED Pin Definition
const int LED_PIN = 6;  // Pin where the LED is connected

// WiFi and MQTT Clients
WiFiClient espClient;
PubSubClient mqttClient(espClient);

void setup() {
  // Start Serial communication for debugging
  Serial.begin(115200);

  // Initialize WiFi and connect to network
  initializeWiFi();
  
  // Set MQTT server and callback function
  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
  mqttClient.setCallback(mqttCallback);

  // Set the LED pin as an output
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  // Ensure MQTT client is connected to the broker
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  
  mqttClient.loop();  // Handle MQTT messages
}

// Function to initialize WiFi connection
void initializeWiFi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  
  // Wait until connected to the WiFi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi!");
}

// Function to reconnect to MQTT broker
void reconnectMQTT() {
  while (!mqttClient.connected()) {
    Serial.println("Attempting MQTT connection...");
    if (mqttClient.connect("Subscriber_Client")) {  // Connect to MQTT broker with a client ID
      Serial.println("Connected to MQTT broker.");
      mqttClient.subscribe(TOPIC_WAVE);  // Subscribe to the wave topic
      mqttClient.subscribe(TOPIC_PAT);   // Subscribe to the pat topic
    } else {
      Serial.print("Failed, rc=");
      Serial.print(mqttClient.state());
      delay(2000);  // Retry after a delay if the connection fails
    }
  }
}

// Callback function triggered when a message is received
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.println(topic);

  // Print the message payload
  Serial.print("Message: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // If the message is from the wave topic, blink the LED for a wave
  if (strcmp(topic, TOPIC_WAVE) == 0) {
    Serial.println("Wave detected, blinking LED.");
    blinkLED(3, 500, 500);  // Blink the LED 3 times (wave action)
  }

  // If the message is from the pat topic, blink the LED for a pat
  else if (strcmp(topic, TOPIC_PAT) == 0) {
    Serial.println("Pat detected, blinking LED.");
    blinkLED(5, 300, 300);  // Blink the LED 5 times (pat action)
  }
}

// Function to blink the LED a specified number of times
void blinkLED(int count, int onTime, int offTime) {
  for (int i = 0; i < count; i++) {
    digitalWrite(LED_PIN, HIGH);  // Turn LED on
    delay(onTime);                // Wait for the specified on-time
    digitalWrite(LED_PIN, LOW);   // Turn LED off
    delay(offTime);               // Wait for the specified off-time
  }
}
