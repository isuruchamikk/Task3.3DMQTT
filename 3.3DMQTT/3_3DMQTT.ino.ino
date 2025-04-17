#include <ArduinoMqttClient.h>
#include <WiFiNINA.h>
#include "secrets.h"
#include <HCSR04.h>

/////// Store confidential credentials in the secrets.h file
char ssid[] = "ssid";        // Name of your WiFi network
char pass[] = "pass";    // WiFi security key (for WPA/WEP)
float Distance = 0.0;

UltraSonicDistanceSensor distanceSensor(13, 12);  // Configure ultrasonic sensor with trigger (13) and echo (12) pins

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "broker.emqx.io";
int        port     = 1883;
const char Topic[]  = "SIT210/waves";

// Duration between sensor checks (in milliseconds)
const long interval = 8000;
unsigned long previousMillis = 0;

int count = 0;

void setup() {
  // Start serial communication at 9600 baud rate
  Serial.begin(9600);
  while (!Serial) {
    ; // Pause execution until serial monitor is ready (for USB-only boards)
  }

  // Establish connection with the wireless network
  Serial.print("Trying to connect to WiFi network: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // Retry connection if unsuccessful
    Serial.print(".");
    delay(5000);
  }

  Serial.println("Network connection established");
  Serial.println();

  // Initialize MQTT broker connection
  Serial.print("Connecting to message broker: ");
  Serial.println(broker);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("Broker connection error: ");
    Serial.println(mqttClient.connectError());

    while (1);
  }

  Serial.println("Broker connection successful!");
  Serial.println();
}

void loop() {
  // Maintain active connection with the broker
  mqttClient.poll();

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // Update timestamp of last sensor reading
    previousMillis = currentMillis;
    
    Serial.println("Measuring current distance");

    Distance = distanceSensor.measureDistanceCm();

    if(Distance < 20.00) {

      Serial.println("Wave motion identified");
      Serial.print("Publishing message to the Topic: ");
      Serial.println(Topic);
      Serial.println("Isuru");

      // Transmit message using MQTT protocol
      mqttClient.beginMessage(Topic);
      mqttClient.print("Isuru");
      mqttClient.endMessage();

      Serial.println();

    

    }
  }
}