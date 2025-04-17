#include <ArduinoMqttClient.h>
#include <WiFiNINA.h>
#include "secrets.h"
#include <HCSR04.h>

/////// Store confidential access details in secrets.h
char ssid[] = "ssid";        // Wireless network identifier
char pass[] = "pass";    // Network security credential (WPA/WEP)
float Distance = 0.0;  // Stores measured proximity value

// Configure ultrasonic ranging module (Trigger:13, Echo:12)
UltraSonicDistanceSensor distanceSensor(13, 12);  

// Network communication handlers
WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

// MQTT service configuration
const char broker[] = "broker.emqx.io";
int        port     = 1883;
const char waveTopic[]  = "SIT210/waveS";  // Channel for wave gestures
const char patTopic[]  = "SIT210/pat";     // Channel for pat gestures

// Timing control for sensor polling (8 second cycle)
const long interval = 8000;
unsigned long previousMillis = 0;

int count = 0;  // Event counter

void setup() {
  // Establish serial monitor connection
  Serial.begin(9600);
  while (!Serial) {
    ; // Pause until serial interface activates (USB-specific)
  }

  // Initiate wireless network handshake
  Serial.print("Establishing connection with network: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // Connection retry mechanism
    Serial.print(".");
    delay(5000);
  }

  Serial.println("Network access confirmed");
  Serial.println();

  // Broker connection procedure
  Serial.print("Initializing MQTT broker link: ");
  Serial.println(broker);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("Broker connection unsuccessful. Error: ");
    Serial.println(mqttClient.connectError());

    while (1);  // Halt on critical failure
  }

  Serial.println("Broker communication established!");
  Serial.println();
}

void loop() {
  // Call poll() regularly to maintain MQTT connection
  mqttClient.poll();

  unsigned long currentMillis = millis();

  // Execute at defined intervals
  if (currentMillis - previousMillis >= interval) {
    // Record last activation timestamp
    previousMillis = currentMillis;
    
    Serial.println("Checking the distance");

    Distance = distanceSensor.measureDistanceCm();

    // Close-range detection
    if(Distance < 20.00) {
      Serial.println("Wave motion identified");
      Serial.print("Publishing message to the Topic: ");
      Serial.println(waveTopic);
      Serial.println("IsuruWave");

      // Publish detection event
      mqttClient.beginMessage(waveTopic);
      mqttClient.print("IsuruWave");
      mqttClient.endMessage();

      Serial.println();

    } 
    // Mid-range detection
    else if (Distance < 40) {
      Serial.println("Pat interaction detected");
      Serial.print("Publishing message to the Topic: ");
      Serial.println(patTopic);
      Serial.println("IsuruPat");

      // Publish detection event
      mqttClient.beginMessage(patTopic);
      mqttClient.print("IsuruPat");
      mqttClient.endMessage();

      Serial.println();
    }
  }
}