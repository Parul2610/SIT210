#include <Wire.h>
#include <BH1750.h>

#define PIR_PIN 7
#define PORCH_LED 2
#define HALLWAY_LED 3
#define BUTTON_PIN 4

// This is the light level where the LEDs should turn off
#define HIGH_LUX 500.0

BH1750 lightMeter;

// These variables are used for the interrupts
volatile bool motion_Detected = false;
volatile bool button_Pressed = false;

bool lights_On = false;

// This runs when the PIR detects motion
void motionInterrupt() {
  motion_Detected = true;
}

// This runs when the button is pressed
void buttonInterrupt() {
  button_Pressed = true;
}

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // Set the pins for the sensors, button and LEDs
  pinMode(PIR_PIN, INPUT);
  pinMode(PORCH_LED, OUTPUT);
  pinMode(HALLWAY_LED, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Start with both lights off
  digitalWrite(PORCH_LED, LOW);
  digitalWrite(HALLWAY_LED, LOW);

  // Start the light sensor
  Wire.begin();
  lightMeter.begin();

  // PIR interrupt when motion is detected
  attachInterrupt(
    digitalPinToInterrupt(PIR_PIN),
    motion_Interrupt,
    RISING
  );

  // Button interrupt when the button is pressed
  attachInterrupt(
    digitalPinToInterrupt(BUTTON_PIN),
    button_Interrupt,
    FALLING
  );

  Serial.println("System ready");
}

void loop() {

  // Get the current light level from the BH1750
  float lux = lightMeter.readLightLevel();

  Serial.print("Light level: ");
  Serial.print(lux);
  Serial.println(" lux");

  // If there is enough light, turn both LEDs off
  if (lux >= HIGH_LUX) {

    lights_On = false;

    digitalWrite(PORCH_LED, LOW);
    digitalWrite(HALLWAY_LED, LOW);

    Serial.println("It is bright - Lights OFF");
  }

  // Check if the PIR detected movement
  if (motion_Detected) {
    motion_Detected = false;

    Serial.print("Motion detected - Light level: ");
    Serial.print(lux);
    Serial.println(" lux");

    // Turn the lights on only when it is dark enough
    if (lux < HIGH_LUX) {

      lights_On = true;

      digitalWrite(PORCH_LED, HIGH);
      digitalWrite(HALLWAY_LED, HIGH);

      Serial.println("It is dark - Lights ON");
    }
    else {
      Serial.println("It is bright - Lights stay OFF");
    }
  }

  // Check if the button was pressed
  if (button_Pressed) {
    button_Pressed = false;

    // Do not turn the lights on if it is already bright
    if (lux >= HIGH_LUX) {

      lights_On = false;

      digitalWrite(PORCH_LED, LOW);
      digitalWrite(HALLWAY_LED, LOW);

      Serial.println("It is bright - Lights stay OFF");
    }
    else {

      // Pressing the button changes the light state
      lights_On = !lightsOn;

      digitalWrite(PORCH_LED, lights_On);
      digitalWrite(HALLWAY_LED, lights_On);

      if (lights_On) {
        Serial.println("Button pressed - Lights ON");
      }
      else {
        Serial.println("Button pressed - Lights OFF");
      }
    }

    // Small delay so one press is not counted many times
    delay(200);
  }

  // Wait a little before checking everything again
  delay(500);
}
