#define BUTTON_PIN 2
#define PORCH_LED_RED_PIN 5
#define HALL_LED_GREEN_PIN 7

void setup() {//this loop runs one one 
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(PORCH_LED_RED_PIN, OUTPUT);
  pinMode(HALL_LED_GREEN_PIN, OUTPUT);
}

// Function to turn light on.
void turn_Lights_On() {
  digitalWrite(PORCH_LED_RED_PIN, HIGH);//high depicts that the LED recieves the voltage and is turned on 
  digitalWrite(HALL_LED_GREEN_PIN, HIGH);
}

// Function to turn lights off.
void turn_Lights_Off(int delay1, int delay2) {
  delay(delay1);//time delay after which the porch led turn off 

  digitalWrite(PORCH_LED_RED_PIN, LOW);
  delay(delay2);//time delay after whcih the Hallway LED turn off .

  digitalWrite(HALL_LED_GREEN_PIN, LOW);
}

void loop() {
  //if loop check the condition weather the button is pressed or not 
  //if the button is pressed that is , it is in the LOW state ,the if loop will run
  if (digitalRead(BUTTON_PIN) == LOW) {

    //when the if loop runs first the turn_Lights_On function run 
    turn_Lights_On();

    //after the first function the second function i.e turns_lights_OFF run 
    //here dealy 1 and delay 2 are both 30000 ms .
    turn_Lights_Off(30000, 30000);
  }
}