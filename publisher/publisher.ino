#include <WiFiNINA.h>
#include <PubSubClient.h>
#include "Secrets.h"
#include "functions.h"

// WiFi and MQTT Configuration
const char* WIFI_SSID = SECRET_SSID;     
const char* WIFI_PASS = SECRET_PASS; 
const char* MQTT_BROKER = BROKER;
const int MQTT_PORT = PORT;
const char* TOPIC_WAVE = "SIT210/wave";  // Topic for publishing wave detection
const char* TOPIC_PAT = "SIT210/pat";

// Ultrasonic Sensor Pins
const int trigPin = 9;
const int echoPin = 10;
long duration;
int distance;

// WiFi and MQTT Clients
WiFiClient espClient;
PubSubClient mqttClient(espClient);


// Setup function (runs once)
void setup() {
  // Start Serial communication for debugging
  Serial.begin(115200);

  // Initialize WiFi and connect to network
  initializeWiFi();
  
  // Set MQTT server
  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);

  // Set up the ultrasonic sensor pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

// Loop function (runs repeatedly)
void loop() {
  // Ensure MQTT client is connected to the broker
  waitForMQTTConnection();
  
  mqttClient.loop();  // Handle MQTT messages
  
  // Read distance from the ultrasonic sensor
  distance = readUltrasonicSensor();

  // If an object is detected within 30 cm, publish a message
  if (distance < 30 && distance >= 10) {
    Serial.println("Wave detected, publishing message.");
    publishMessage(TOPIC_WAVE, "Sudhanshu");  // Publish message containing your name
    delay(2000);  // Wait to avoid multiple detections
  }

  // If an object is detected within 10 cm, it's considered a pat, publish a message
  else if (distance < 10) {
    Serial.println("Pat detected, publishing message.");
    publishMessage(TOPIC_PAT, "Sudhanshu");  // Publish message containing your name
    delay(2000);  // Wait to avoid multiple detections
  }
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

// Function to reconnect to the MQTT broker
void reconnectMQTT() {
  while (!mqttClient.connected()) {
    Serial.println("Attempting MQTT connection...");
    if (mqttClient.connect("Publisher_Client")) {  // Connect to MQTT broker with a client ID
      Serial.println("Connected to MQTT broker.");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(mqttClient.state());
      delay(2000);  // Retry after a delay if the connection fails
    }
  }
}

// Function to wait for a connection to the MQTT broker
void waitForMQTTConnection() {
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
}

// Function to read the distance from the ultrasonic sensor
int readUltrasonicSensor() {
  // Send a 10µs pulse to the ultrasonic sensor to trigger a measurement
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Measure the pulse duration
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate and return the distance in centimeters
  return duration * 0.034 / 2;
}

// Function to publish an MQTT message to a specific topic
void publishMessage(const char* topic, const char* message) {
  mqttClient.publish(topic, message);
}
