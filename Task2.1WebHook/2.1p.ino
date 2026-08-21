#include "DHT.h"         // this library is used for the DHT22 senesor
#include <BH1750.h>      // this library is used for the BH1750 senesor
#include <Wire.h>        //this library is used for I2C communication
#include <WiFiNINA.h>    //this library is used for connecting the Arduino Nano 33 IOT to the wifi
#include "secrets.h"     //this file contains  the information regarding the wifi and thingSpeak
#include "ThingSpeak.h"  /// this library is used for sending the data to the ThingSpeak


unsigned long myChannelNumber = SECRET_CH_ID;     //this is thingSpeak channel number
const char* myWriteAPIKey = SECRET_WRITE_APIKEY;  //this is the API key

WiFiClient client;  //it creats a wifi client for the communication
#define DHTPIN 2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);  //DHT22 sensor object
BH1750 lightMeter;         // BH1750 light sensor object


float temperature;
float humidity;
float light_level;

void setup() {
  Serial.begin(115200);  // Starts the Serial Monitor at 115200 speed
  while (!Serial)        // Waits until the Serial Monitor is ready
  {
    ;
  }
  dht.begin();
  wifi_connection();
  ThingSpeak.begin(client);
  light_sensor_setting();
}

void wifi_connection()  //this function is made to connect  the arduino nano IOT 33 to wifi
{

  if (WiFi.status() != WL_CONNECTED)  //this checks if there is wifi connection or not
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);

    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(SECRET_SSID, SECRET_PASS);
      Serial.print(".");
      delay(5000);
    }
    Serial.println("\nConnected.");
  }
}

void temperature_reading()  //this function is used to read the temperature and humidity from DHT22 sensor
{

  humidity = dht.readHumidity();

  temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature))  //this checks is the reading are taken from the sensor or not
  {
    Serial.println("Failed to read from DHT22 sensor");
    return;
  }

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print("%  Temperature: ");
  Serial.print(temperature);
  Serial.println("°C");
}

void light_sensor_setting()
// This function prepares the BH1750 light sensor
{

  Wire.begin();  // Starts I2C communication


  lightMeter.begin();

  Serial.println("BH1750 Test begin");
}

void light_readings()  // This function reads the light level from the BH1750 sensor
{
  // Reads the amount of light in the room in lux
  light_level = lightMeter.readLightLevel();

  Serial.print("Light: ");
  Serial.print(light_level);
  Serial.println(" lx");
}

void send_to_thingspeak()  // This function sends the sensor readings to ThingSpeak
{

  ThingSpeak.setField(1, temperature);
  ThingSpeak.setField(2, humidity);
  ThingSpeak.setField(3, light_level);

  int response = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  if (response == 200) {
    Serial.println("Channel update successful.");
  } else {
    Serial.print("Problem updating channel. HTTP error code: ");
    Serial.println(response);
  }
}

//this is the main loop which runs repeatedly .
void loop() {
  wifi_connection();
  temperature_reading();
  light_readings();
  send_to_thingspeak();

  delay(30000);
}
