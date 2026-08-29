#include <WiFiNINA.h>          // it provides Wi-Fi functions for the Nano 33 IoT
#include <PubSubClient.h>      // it provides MQTT communication
#include <Wire.h>              // this enables I2C communication
#include <BH1750.h>            //  this library is  for the BH1750 light sensor


// Wi-Fi configuration
#define WIFI_SSID "Parul"
#define WIFI_PASSWORD "aadvik2605"


// HiveMQ Cloud configuration
#define MQTT_SERVER "974977aac2d241c0a11474b272695f80.s1.eu.hivemq.cloud"
#define MQTT_PORT 8883// Port 8883 is used for secure MQTT communication.

#define MQTT_USER "Parul"
#define MQTT_PASSWORD "Parul2605"

// MQTT topic used to send terrarium light information
#define MQTT_TOPIC "parul/sunlight"


// Light threshold
// If the measured lux is greater than this value,the system considers the terrarium to be receiving sunlight.

const float SUNLIGHT_THRESHOLD = 500.0;



WiFiSSLClient secureClient;
PubSubClient mqttClient(secureClient);
BH1750 lightMeter;


// Connecting the Arduino to the Wi-Fi network
void connectWiFi()
{
    Serial.println("Connecting to Wi-Fi...");

    while (WiFi.status() != WL_CONNECTED)
    {
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        delay(2000);

        Serial.print(".");
    }

    Serial.println();
    Serial.println("Wi-Fi connection successful!");

    // Display the IP address assigned to the Arduino
    Serial.print("Arduino IP address: ");
    Serial.println(WiFi.localIP());
}



// Connecting the Arduino to the HiveMQ MQTT broker
void connectMQTT()
{
    while (!mqttClient.connected())
    {
        Serial.println("Connecting to HiveMQ MQTT broker...");


        String clientID = "TerrariumArduino-";
        clientID += String(random(1000, 9999));

        if (mqttClient.connect(clientID.c_str(), MQTT_USER, MQTT_PASSWORD))
        {
            Serial.println("Connected to HiveMQ!");
        }
        else
        {
           
            Serial.print("MQTT connection failed. State: ");
            Serial.println(mqttClient.state());

            Serial.println("Trying again in 5 seconds...");
            delay(5000);
        }
    }
}


void setup()
{
    Serial.begin(115200);
    delay(2000);

    Serial.println();
    Serial.println("");
    Serial.println("Terrarium Light Monitoring System");
    Serial.println("");


    Wire.begin();


    if (lightMeter.begin())
    {
        Serial.println("BH1750 initialised successfully.");
    }
    else
    {
        // Stop the program if the sensor cannot be detected
        Serial.println("ERROR: BH1750 sensor not detected.");

        while (true)
        {
            delay(1000);
        }
    }
    connectWiFi();
    mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
    connectMQTT();

    Serial.println("System ready.");
}

void loop()
{
    
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("Wi-Fi connection lost.");
        connectWiFi();
    }
    if (!mqttClient.connected())
    {
        connectMQTT();
    }

    mqttClient.loop();

    float lux = lightMeter.readLightLevel();

    if (lux < 0)
    {
        Serial.println("Invalid BH1750 reading.");
        delay(1000);
        return;
    }

    bool sunlightDetected = lux >= SUNLIGHT_THRESHOLD;


    Serial.print("Light level: ");
    Serial.print(lux);
    Serial.println(" lux");

    if (sunlightDetected)
    {
        Serial.println("Status: SUNLIGHT DETECTED");
    }
    else
    {
        Serial.println("Status: NO SUNLIGHT");
    }


    String lightMessage = String(lux, 2);


if (mqttClient.publish(MQTT_TOPIC, lightMessage.c_str()))
{
    Serial.print("Published light level: ");
    Serial.print(lightMessage);
    Serial.println(" lux");
}
else
{
    Serial.println("MQTT publish failed.");
}


  

    delay(5000);
}
