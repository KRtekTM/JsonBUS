#include "SwitecX25.h"
#include "ArduinoJson.h"

// standard X25.168 range 315 degrees at 1/3 degree steps
#define STEPS (315*3)


// PIN configuration
//const SwitecX25 gauge_rpm(STEPS, 2,3,4,5); //uses 4 digital pins, 315*3=945 steps
//const SwitecX25 gauge_speed(STEPS, 6,7,8,9); //uses 4 digital pins, 315*3=945 steps
//const SwitecX25 gauge_temp(STEPS, 10,11,12,13); //uses 4 digital pins, 315*3=945 steps
//const SwitecX25 gauge_gas(STEPS, 14,15,16,17); //uses 4 digital pins, 315*3=945 steps
const int cluster_light = 11;
const int left_indicator = 22;
const int right_indicator = 32;
//const int both_indicators = 0; // for connecting HW button light on dashboard
const int airbag = 23;
const int light_lowbeam = 25;
const int light_mainbeam = 27;
const int light_fogFront = 24;
const int light_fogRear = 26;
const int warn_temp = 29;
const int warn_gas = 31;
const int warn_oil = 28;
const int warn_engine = 33;
const int warn_failure = 34;
const int warn_abs = 35;
const int warn_immobilizer = 36;
const int warn_handbrake = 37;
const int warn_battery = 38;
const int cruisecontrol = 30;
const int speaker = 42;
const int reservePin0 = 39;
const int reservePin1 = 40;
const int reservePin2 = 41;

const int noteFrequency = 4500;

int state_electricity, state_blinker = 0;
bool state_gasLow = false;

// Celebration is typical Skoda/Volkswagen instrument cluster lightshow when electricity is on
void clusterCelebration() {
  // Ignition switch position 1
  analogWrite(cluster_light, 10);
  digitalWrite(airbag, HIGH);
  digitalWrite(warn_temp, HIGH);
  digitalWrite(warn_gas, HIGH);
  digitalWrite(warn_oil, HIGH);
  digitalWrite(warn_engine, HIGH);
  digitalWrite(warn_failure, HIGH);
  digitalWrite(warn_abs, HIGH);
  digitalWrite(warn_battery, HIGH); //will light up until ignition switch set to position 2

  // Needle sweep
  //gauge_rpm.setPosition(STEPS);
  //gauge_speed.setPosition(STEPS);

  delay(500);
  
  digitalWrite(warn_immobilizer, HIGH);
  tone(speaker, noteFrequency);

  delay(500);
  
  //gauge_rpm.setPosition(0);
  //gauge_speed.setPosition(0);
  digitalWrite(warn_gas, LOW);

  delay(500);
  
  digitalWrite(warn_immobilizer, LOW);
  noTone(speaker);

  delay(500);

  digitalWrite(warn_engine, LOW);

  delay(50);
  
  digitalWrite(airbag, LOW);
  
  delay(200);
  
  digitalWrite(warn_failure, LOW);

  delay(100);
  
  digitalWrite(warn_abs, LOW);
  
  delay(150);

  digitalWrite(warn_immobilizer, HIGH);
  tone(speaker, noteFrequency);
  
  delay(500);
  
  digitalWrite(warn_temp, LOW);

  delay(500);
  digitalWrite(warn_oil, LOW);
  digitalWrite(warn_immobilizer, LOW);
  noTone(speaker);

  delay(1000);
  digitalWrite(warn_immobilizer, HIGH);
  tone(speaker, noteFrequency);

  delay(1000);
  digitalWrite(warn_immobilizer, LOW);
  noTone(speaker);

}

void clusterTurnOff() {
  digitalWrite(cluster_light, LOW);
  digitalWrite(left_indicator, LOW);
  digitalWrite(right_indicator, LOW);
  digitalWrite(airbag, LOW);
  digitalWrite(light_lowbeam, LOW);
  digitalWrite(light_mainbeam, LOW);
  digitalWrite(light_fogFront, LOW);
  digitalWrite(light_fogRear, LOW);
  digitalWrite(warn_temp, LOW);
  digitalWrite(warn_gas, LOW);
  digitalWrite(warn_oil, LOW);
  digitalWrite(warn_engine, LOW);
  digitalWrite(warn_failure, LOW);
  digitalWrite(warn_abs, LOW);
  digitalWrite(warn_immobilizer, LOW);
  digitalWrite(warn_handbrake, LOW);
  digitalWrite(warn_battery, LOW);
  digitalWrite(cruisecontrol, LOW);

  //gauge_rpm.zero();
  //gauge_speed.zero();
  //gauge_temp.zero();
  //gauge_gas.zero();
}

void testLights() {
  analogWrite(cluster_light, 100);
  digitalWrite(left_indicator, HIGH);
  digitalWrite(right_indicator, HIGH);
  digitalWrite(airbag, HIGH);
  digitalWrite(light_lowbeam, HIGH);
  digitalWrite(light_mainbeam, HIGH);
  digitalWrite(light_fogFront, HIGH);
  digitalWrite(light_fogRear, HIGH);
  digitalWrite(warn_temp, HIGH);
  digitalWrite(warn_gas, HIGH);
  digitalWrite(warn_oil, HIGH);
  digitalWrite(warn_engine, HIGH);
  digitalWrite(warn_failure, HIGH);
  digitalWrite(warn_abs, HIGH); //
  digitalWrite(warn_immobilizer, HIGH);
  digitalWrite(warn_handbrake, HIGH);
  digitalWrite(warn_battery, HIGH);
  digitalWrite(cruisecontrol, HIGH);

  delay(10000);

  clusterTurnOff();

  analogWrite(cluster_light, 100);
  delay(300);
  analogWrite(cluster_light, 0);
  digitalWrite(left_indicator, HIGH);
  delay(300);
  digitalWrite(left_indicator, LOW);
  digitalWrite(right_indicator, HIGH);
  delay(300);
  digitalWrite(right_indicator, LOW);
  digitalWrite(airbag, HIGH);
  delay(300);
  digitalWrite(airbag, LOW);
  digitalWrite(light_lowbeam, HIGH);
  delay(300);
  digitalWrite(light_lowbeam, LOW);
  digitalWrite(light_fogFront, HIGH);
  delay(300);
  digitalWrite(light_fogFront, LOW);
  digitalWrite(light_fogRear, HIGH);
  delay(300);
  digitalWrite(light_fogRear, LOW);
  digitalWrite(light_mainbeam, HIGH);
  delay(300);
  digitalWrite(light_mainbeam, LOW);
  digitalWrite(warn_temp, HIGH);
  delay(300);
  digitalWrite(warn_temp, LOW);
  digitalWrite(cruisecontrol, HIGH);
  delay(300);
  digitalWrite(cruisecontrol, LOW);
  digitalWrite(warn_gas, HIGH);
  delay(300);
  digitalWrite(warn_gas, LOW);
  digitalWrite(warn_oil, HIGH);
  delay(300);
  digitalWrite(warn_oil, LOW);
  digitalWrite(warn_engine, HIGH);
  delay(300);
  digitalWrite(warn_engine, LOW);
  digitalWrite(warn_failure, HIGH);
  delay(300);
  digitalWrite(warn_failure, LOW);
  digitalWrite(warn_abs, HIGH);
  delay(300);
  digitalWrite(warn_abs, LOW);
  digitalWrite(warn_immobilizer, HIGH);
  delay(300);
  digitalWrite(warn_immobilizer, LOW);
  digitalWrite(warn_handbrake, HIGH);
  delay(300);
  digitalWrite(warn_handbrake, LOW);
  digitalWrite(warn_battery, HIGH);
  delay(300);
  digitalWrite(warn_battery, LOW);


  delay(500);

  //testLights();
}

void busCommunication() {
  // Loop until there is data waiting to be read
  while (!Serial.available())
  delay(50);

  DynamicJsonBuffer jb;
  JsonObject& root = jb.parseObject(Serial);

  // Test if parsing succeeds.
  if (!root.success()) {
    //Serial.println("parseObject() failed");
    return;
  }
  
  // Fetch values.
  //
  // Most of the time, you can rely on the implicit casts.
  // In other case, you can do root["time"].as<long>();
  //const char* sensor = root["sensor"];
  //long time = root["time"];
  //double latitude = root["data"][0];
  //double longitude = root["data"][1];


  int feature = root["f"];
  int featureValue = root["v"];
  const char* featureText = root["t"];

  // Print values.
  Serial.println(feature);
  Serial.println(featureValue);
  Serial.println(featureText);

  switch(feature) {
    // electric on/off
    case 230:
      if(state_electricity != featureValue) {
        (featureValue == 1) ? clusterCelebration() : clusterTurnOff();  
        state_electricity = featureValue;
      }
      break;

    // engine on/off
    case 231:
      if(state_electricity != 0) {
        if (featureValue == 1) {
          digitalWrite(warn_battery, LOW);
        }
        else {
          digitalWrite(warn_battery, HIGH);
        }
      }
      break;

    // cruisecontrol on/off
    case 232:
      if(state_electricity == 1) { // turn on only when electricity is on
        (featureValue == 1) ? digitalWrite(cruisecontrol, HIGH) : digitalWrite(cruisecontrol, LOW);
      }
      break;

    // temp
    case 233:
      // turn the control on when temp is above 120°C
      if (featureValue >= 120) {
        digitalWrite(warn_temp, HIGH);
      }
      else {
        digitalWrite(warn_temp, LOW);
      }

      // control the gauge (MIN=50°C,MAX=130°C)
      if (featureValue <= 50) featureValue = 50;
      if (featureValue >= 130) featureValue = 130;
      // TODO calculate steps for X25.168 stepper motor to specific temp value
      break;
      
    // gas level
    case 234:
      // turn the control on when gas level is below 10%
      if (featureValue <= 15) {
        digitalWrite(warn_gas, HIGH);
      }
      else {
        digitalWrite(warn_gas, LOW);
      }

      // control the gauge (MIN=5%,MAX=100%)
      if (featureValue <= 5) featureValue = 5;
      if (featureValue >= 100) featureValue = 100;
      // TODO calculate steps for X25.168 stepper motor to specific gas value
      break;

      // RPM
    case 235:
      // control the gauge (MIN=0,MAX=7500)
      if (featureValue <= 0) featureValue = 0;
      if (featureValue >= 7500) featureValue = 7500;
      // TODO calculate steps for X25.168 stepper motor to specific rpm value
      break;

    // speed
    case 236:
      // control the gauge (MIN=0,MAX=240)
      if (featureValue <= 0) featureValue = 0;
      if (featureValue >= 240) featureValue = 240;
      // TODO calculate steps for X25.168 stepper motor to specific speed value
      break;

    // low light beam on/off
    case 237:
      if(state_electricity == 1) { // turn on only when electricity is on
        if (featureValue == 1) {
          digitalWrite(light_lowbeam, HIGH);
          //analogWrite(cluster_light, 100);
        }
        else {
          digitalWrite(light_lowbeam, LOW);
          //analogWrite(cluster_light, 0);
        }
      }
      break;

    // high light beam on/off
    case 238:
      (featureValue == 1) ? digitalWrite(light_mainbeam, HIGH) : digitalWrite(light_mainbeam, LOW);
      break;

    // cluster backlight on/off
    case 239:
      if(state_electricity == 1) { // turn on only when electricity is on
        analogWrite(cluster_light, featureValue);
      }
      break;

    // indicators on/off
    case 240:
      switch(featureValue) {
        case 1:
          if(state_electricity == 1) { // turn on only when electricity is on
            digitalWrite(left_indicator, HIGH);
            digitalWrite(right_indicator, LOW);
            if(state_blinker == 0) {
              tone(speaker, 500, 10);
              state_blinker = 1;
            }
          }
          break;
        case 2:
          if(state_electricity == 1) { // turn on only when electricity is on
            digitalWrite(left_indicator, LOW);
            digitalWrite(right_indicator, HIGH);
            if(state_blinker == 0) {
              tone(speaker, 500, 10);
              state_blinker = 1;
            }
          }
          break;
        case 3:
          digitalWrite(left_indicator, HIGH);
          digitalWrite(right_indicator, HIGH);
            if(state_blinker == 0) {
              tone(speaker, 500, 10);
              state_blinker = 1;
            }
          break;
        default:
          digitalWrite(left_indicator, LOW);
          digitalWrite(right_indicator, LOW);
          if(state_blinker == 1) {
            tone(speaker, 300, 10);
            state_blinker = 0;  
          }
          break;
      }
      break;

    // front fog light on/off
    case 241:
      if(state_electricity == 1) { // turn on only when electricity is on
        (featureValue == 1) ? digitalWrite(light_fogFront, HIGH) : digitalWrite(light_fogFront, LOW);
      }
      break;

    // rear fog light on/off
    case 242:
      if(state_electricity == 1) { // turn on only when electricity is on
        (featureValue == 1) ? digitalWrite(light_fogRear, HIGH) : digitalWrite(light_fogRear, LOW);
      }
      break;

    // parking brake light on/off
    case 244:
      if(state_electricity == 1) { // turn on only when electricity is on
        (featureValue == 1) ? digitalWrite(warn_handbrake, HIGH) : digitalWrite(warn_handbrake, LOW);
      }
      break;

    // motor brake light on/off
    case 245:
      if(state_electricity == 1) { // turn on only when electricity is on
        (featureValue > 0) ? digitalWrite(warn_abs, HIGH) : digitalWrite(warn_abs, LOW);
      }
      break;

    // engine damage report
    case 246:
      if(state_electricity == 1) { // turn on only when electricity is on
        (featureValue >= 20) ? digitalWrite(warn_engine, HIGH) : digitalWrite(warn_engine, LOW);
        (featureValue >= 25) ? digitalWrite(warn_failure, HIGH) : digitalWrite(warn_failure, LOW);
        (featureValue >= 30) ? digitalWrite(warn_oil, HIGH) : digitalWrite(warn_oil, LOW);
        (featureValue >= 40) ? digitalWrite(warn_temp, HIGH) : digitalWrite(warn_temp, LOW);
        (featureValue >= 65) ? digitalWrite(airbag, HIGH) : digitalWrite(airbag, LOW);
      }     
      break;

    // oil pressure warning light on/off
    case 247:
      if(state_electricity == 1) { // turn on only when electricity is on
        (featureValue == 1) ? digitalWrite(warn_oil, HIGH) : digitalWrite(warn_oil, LOW);
      }
      break;

    // battery voltage warning light on/off
    case 248:
      if(state_electricity == 1) { // turn on only when electricity is on
        (featureValue == 1) ? digitalWrite(warn_battery, HIGH) : digitalWrite(warn_battery, LOW);
      }
      break;


    // light test
    case 888:
      if (featureValue == 1 && state_electricity == 0) testLights();
      break;  
  }
}


void setup() {
  Serial.begin(115200);
  
  // Initialize PINs
  pinMode(cluster_light, OUTPUT);
  pinMode(left_indicator, OUTPUT);
  pinMode(right_indicator, OUTPUT);
  pinMode(airbag, OUTPUT);
  pinMode(light_lowbeam, OUTPUT);
  pinMode(light_mainbeam, OUTPUT);
  pinMode(light_fogFront, OUTPUT);
  pinMode(light_fogRear, OUTPUT);
  pinMode(warn_temp, OUTPUT);
  pinMode(warn_gas, OUTPUT);
  pinMode(warn_oil, OUTPUT);
  pinMode(warn_engine, OUTPUT);
  pinMode(warn_failure, OUTPUT);
  pinMode(warn_abs, OUTPUT);
  pinMode(warn_immobilizer, OUTPUT);
  pinMode(warn_handbrake, OUTPUT);
  pinMode(warn_battery, OUTPUT);
  pinMode(cruisecontrol, OUTPUT);

  // Turn off all lights and set needles to 0
  clusterTurnOff();
}

void loop() {
  // update motors frequently to allow them to step
  //gauge_rpm.update();
  //gauge_speed.update();
  //gauge_temp.update();
  //gauge_gas.update();

  busCommunication();
}
