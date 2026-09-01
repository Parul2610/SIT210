#include <WiFiNINA.h>
#include <PubSubClient.h>

// Wi-Fi details
char ssid[] = "Parul";
char pass[] = "aadvik2605";

// MQTT details
const char* mqtt_server = "broker.emqx.io";
const int mqtt_port = 1883;

// Pin numbers
const int trig_Pin = 7;
const int echo_Pin = 8;

const int Hallway_Led = 2;
const int Bathroom_led = 3;

// Wi-Fi and MQTT
WiFiClient wifiClient;
PubSubClient client(wifiClient);


// This runs when a message is received
void callback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Message received on topic: ");
  Serial.println(topic);

  Serial.print("Message: ");

  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }

  Serial.println();


  // Wave turns both LEDs on
  if (strcmp(topic, "ES/Wave") == 0) {

    digitalWrite(Hallway_Led, HIGH);
    digitalWrite(Bathroom_led, HIGH);

    Serial.println("Both LEDs ON");
  }


  // Pat turns both LEDs off
  if (strcmp(topic, "ES/Pat") == 0) {

    digitalWrite(Hallway_Led, LOW);
    digitalWrite(Bathroom_led, LOW);

    Serial.println("Both LEDs OFF");
  }
}


// Setup starts here
void setup() {

  Serial.begin(9600);

  // Set LED pins
  pinMode(Hallway_Led, OUTPUT);
  pinMode(Bathroom_led, OUTPUT);

  digitalWrite(Hallway_Led, LOW);
  digitalWrite(Bathroom_led, LOW);

  // Set sensor pins
  pinMode(trig_Pin, OUTPUT);
  pinMode(echo_Pin, INPUT);


  // Connect to Wi-Fi
  Serial.print("Connecting to Wi-Fi");

  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    delay(2000);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Wi-Fi connected!");


  // Connect to MQTT
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  connectMQTT();
}


// Main program
void loop() {

  if (!client.connected()) {
    connectMQTT();
  }

  client.loop();


  // Send signal from sensor
  digitalWrite(trig_Pin, LOW);
  delayMicroseconds(2);

  digitalWrite(trig_Pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_Pin, LOW);


  // Read the echo
  long duration = pulseIn(echo_Pin, HIGH, 30000);


  // Check if sensor got no signal
  if (duration == 0) {
    Serial.println("No echo received");
    delay(500);
    return;
  }


  // Calculate distance
  float distance = duration * 0.0343 / 2;

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");


  // Detect a wave
  if (distance >= 10 && distance <= 30) {

    Serial.println("Wave detected!");

    client.publish("ES/Wave", "Parul");

    delay(2000);
  }


  // Detect a pat
  if (distance > 0 && distance < 10) {

    Serial.println("Pat detected!");

    client.publish("ES/Pat", "Parul");

    delay(2000);
  }

  delay(200);
}


// Connect to MQTT
void connectMQTT() {

  Serial.print("Connecting to MQTT...");

  while (!client.connected()) {

    String clientID = "ArduinoNano33IoT-";
    clientID += String(random(0xffff), HEX);


    if (client.connect(clientID.c_str())) {

      Serial.println("connected!");

      // Subscribe to both topics
      client.subscribe("ES/Wave");
      client.subscribe("ES/Pat");

    } 
    else {

      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" trying again...");

      delay(2000);
    }
  }
}
