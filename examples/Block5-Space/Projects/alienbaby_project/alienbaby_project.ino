
#include <EducationShield.h>

BLEuart ble=BLEuart(TYPE_TAMAGOTCHI);
IMU imu;

// led pins
LED redLed(12);
LED greenLed(11);
LED yellowLed(10);

//const int lightPin = A0; //phototransistor
LDR sleepSensor(A1);

Button foodButton(9);

int exerciseLv, foodLv, sleepLv; // the saved stats that will go down over time

long sleepTimer;
long updateTimer; 

void setup() {
  Serial.begin(9600);

  // Curie IMU setup
  imu.begin();
  imu.detectShock();
  imu.attachCallback(shockCallback);

  // set advertised local name
  ble.setName("Tamagotchi");
  ble.begin();

  // configure the 2 leds to be outputs
  redLed.begin();
  greenLed.begin();
  yellowLed.begin();

  foodButton.begin();

  sleepSensor.config(1000,200);

  //Set initial values for the three states
  exerciseLv = 100;
  foodLv = 100;
  sleepLv = 100;
  
  updateTimer = millis();
  sleepTimer = millis();
}

void loop() {

  updateStatus(); // used to update the stats values

  // if a central is connected to peripheral:
  if(ble.searchCentral()){
    Serial.println("Connected to central ");
    // while the central is still connected to peripheral:
    while(ble.connected()){
      updateStatus();// used to update the stats values
      Serial.println("Sending data");
      ble.addValue(exerciseLv);
      ble.addValue(foodLv);
      ble.addValue(sleepLv);    
      ble.send();
    }
    // when the central disconnects, print it out:
    Serial.println(F("Disconnected from central "));

  }

}

void updateStatus() {
  // if the light level is low, add 1 to the sleepLv
  if(sleepSensor.getState() && millis()-sleepTimer>200){
    sleepLv++;
    sleepTimer=millis();
  }

  //Fill the food stat when the button is being pressed
  if(foodButton.beenPressed()){
    foodLv+=5;
  }

  // every 3 sec update and lower the 3 stats in diffrent rates
  if (millis() - updateTimer >= 3000) {
    exerciseLv = exerciseLv - 1;
    foodLv = foodLv - 2;
    sleepLv = sleepLv - 3;

    updateTimer = millis();
  }

  //Constrain the values so they don't exceed 100 or go below 0
  exerciseLv=constrain(exerciseLv,0,100);
  foodLv=constrain(foodLv,0,100);
  sleepLv=constrain(sleepLv,0,100);

  // if any stats are to low, light the leds with the fitting color
  if (sleepLv < 5) {
    redLed.on();
  } else {
    redLed.off();
  }

  if (foodLv < 5) {
    greenLed.on();
  } else {
    greenLed.off();
  }

  if (exerciseLv < 5) {
    yellowLed.on();
  } else {
    yellowLed.off();
  }
}

// this is called when the Imu detects  a shock in any direction
static void shockCallback(void)
{
  //add 1 to the exerciseLv
  exerciseLv++;
}
