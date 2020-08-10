#include "SwitecX25.h"
#include "ArduinoJson.h"
#include <Multi_BitBang.h>
#include <Multi_OLED.h>

// standard X25.168 range 315 degrees at 1/3 degree steps
#define STEPS (315*3)


// PIN configuration
const SwitecX25 gauge_rpm(STEPS, A10,A11,A8,A9); //uses 4 digital pins, 315*3=945 steps
const SwitecX25 gauge_speed(STEPS, A14,A15,A12,A13); //uses 4 digital pins, 315*3=945 steps
const SwitecX25 gauge_temp(STEPS, A6,A7,A4,A5); //uses 4 digital pins, 315*3=945 steps
const SwitecX25 gauge_gas(STEPS, A2,A3,A0,A1); //uses 4 digital pins, 315*3=945 steps
const int cluster_light = 11;
const int left_indicator = 52;
const int right_indicator = 53;
const int airbag = 36;
const int light_lowbeam = 34;
const int light_mainbeam = 48;
const int light_fogFront = 33;
const int light_fogRear = 32;
const int warn_temp = 46;
const int warn_gas = 49;
const int warn_oil = 44;
const int warn_engine = 37;
const int warn_failure = 39;
const int warn_abs = 38;
const int warn_immobilizer = 41;
const int warn_handbrake = 40;
const int warn_battery = 43;
const int warn_safe = 42;
const int cruisecontrol = 47;
const int speaker = 50;
const int trailer = 35;
const int beacon = 45;
const int both_indicators = 30;
const int button_left = 9;
const int button_ext1 = 31;

const int noteFrequency = 4500;

int state_electricity, state_blinker = 0;
bool state_gasLow = false;
int msgFuel, msgRange = 0;

bool cityShown = false;
String lastCity = "";

bool showTime = true;
String lastTime = "";

bool lowFuelBeep, buttonPressed = false;
float gasLevel, currentRPM, currentSpeed, currentTemp = 0;

int clusterLightIntensity = 0;

// OLED display settings
#define NUM_DISPLAYS 2
#define NUM_BUSES 2
// I2C bus info
uint8_t scl_list[NUM_BUSES] = {6,5}; //pins 5,6
uint8_t sda_list[NUM_BUSES] = {8,7};  //pins 7,8
int32_t speed_list[NUM_BUSES] = {4000000L,4000000L};
// OLED display info
uint8_t bus_list[NUM_DISPLAYS] = {0,1}; // can be multiple displays per bus
uint8_t addr_list[NUM_DISPLAYS] = {0x3c, 0x3c};
uint8_t type_list[NUM_DISPLAYS] = {OLED_128x32, OLED_128x32};
uint8_t flip_list[NUM_DISPLAYS] = {0,1};
uint8_t invert_list[NUM_DISPLAYS] = {0,0};

// Celebration is typical Skoda/Volkswagen instrument cluster lightshow when electricity is on
void clusterCelebration() {
  Serial.end(); // reactivate on end of celebration to prevent looping
  // Ignition switch position 1
  //analogWrite(cluster_light, 10);
  Multi_OLEDWriteString(1, 0, -1, "SKODA", FONT_LARGE, 0);
  digitalWrite(airbag, HIGH);
  digitalWrite(warn_temp, HIGH);
  digitalWrite(warn_gas, HIGH);
  digitalWrite(warn_oil, HIGH);
  digitalWrite(warn_engine, HIGH);
  digitalWrite(warn_failure, HIGH);
  digitalWrite(warn_abs, HIGH);
  digitalWrite(warn_battery, HIGH); //will light up until ignition switch set to position 2
  digitalWrite(warn_safe, HIGH);

  // Needle sweep start
  //gauge_rpm.setPosition(750);
  //gauge_rpm.updateBlocking();
  //gauge_speed.setPosition(750);
  //gauge_speed.updateBlocking();

  delay(500);
  
  digitalWrite(warn_immobilizer, HIGH);
  tone(speaker, noteFrequency);

  delay(500);
  
  // Needle sweep end
  //gauge_rpm.setPosition(0);
  //gauge_rpm.updateBlocking();
  //gauge_speed.setPosition(0);
  //gauge_speed.updateBlocking();
  digitalWrite(warn_gas, LOW);

  delay(500);
  
  digitalWrite(warn_immobilizer, LOW);
  digitalWrite(warn_safe, LOW);
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
  
  WriteLeftOLED("", true, false);

  delay(1000);
  digitalWrite(warn_immobilizer, HIGH);
  tone(speaker, noteFrequency);

  delay(1000);
  digitalWrite(warn_immobilizer, LOW);
  noTone(speaker);

  //String msggg = (String)"CC 0 km/h           ";
  //WriteRightOLED(msggg.c_str(), false, 3);
        
  Serial.begin(115200);
}

void clusterTurnOff() {
  analogWrite(cluster_light, 0);
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
  digitalWrite(trailer, LOW);
  digitalWrite(beacon, LOW);
  digitalWrite(both_indicators, LOW);
  digitalWrite(warn_safe, LOW);

  gasLevel = 0;
  currentTemp = 0;
  currentRPM = 0;
  currentSpeed = 0;

  gauge_rpm.zero();
  gauge_speed.zero();
  gauge_temp.zero();
  gauge_gas.zero();

  WriteLeftOLED("", true, false);
  WriteRightOLED("",true,0);
  cityShown = false;
}

void testLights() {
  analogWrite(cluster_light, 255);
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
  digitalWrite(trailer, HIGH);
  digitalWrite(beacon, HIGH);
  digitalWrite(both_indicators, HIGH);
  digitalWrite(warn_safe, HIGH);
}

void busCommunication() { 
  // Loop until there is data waiting to be read
  if(Serial.available())
  {
  
    DynamicJsonBuffer jb;
    JsonObject& root = jb.parseObject(Serial);
  
    // Test if parsing succeeds.
    if (!root.success()) {
      Serial.println("parseObject() failed");
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
    double featureValue = root["v"];
    const char* featureText = root["t"];
    
    
    // Print values.
    //Serial.println(feature);
    //Serial.println(featureValue);
    //Serial.println(featureText);
  
    switch(feature) {
      // electric on/off
      case 230:
        if(state_electricity != (int)featureValue) {
          ((int)featureValue == 1) ? clusterCelebration() : clusterTurnOff();  
          state_electricity = (int)featureValue;
        }
        break;
  
      // engine on/off
      case 231:
        if(state_electricity != 0) {
          if ((int)featureValue == 1) {
            digitalWrite(warn_battery, LOW);
          }
          else {
            digitalWrite(warn_battery, HIGH);
          }
        }
        break;
  
      // internet connected/disconnected
      case 232:
        if(state_electricity != 0) {
          if ((int)featureValue == 1) {
            digitalWrite(airbag, HIGH);
          }
          else {
            digitalWrite(airbag, LOW);
          }
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
  
        // control the gauge (MIN=50°C,MAX=130°C)// range 80°C shown on 260 steps
        if (featureValue <= 50) currentTemp = 0;
        if (featureValue > 50 && featureValue < 130) currentTemp = (featureValue - 50) * 3.25;  
        if (featureValue >= 130) currentTemp = (130 - 50) * 3.25;
        // TODO calculate steps for X25.168 stepper motor to specific temp value
        break;
        
      // gas level
      case 234:
        // turn the control on when gas level is below 15%
        if (featureValue <= 15) {
          digitalWrite(warn_gas, HIGH);
          if(!lowFuelBeep) {
            tone(speaker, noteFrequency, 500);
            lowFuelBeep = true;
          }
        }
        else {
          digitalWrite(warn_gas, LOW);
          lowFuelBeep = false;
        }
  
        // control the gauge (MIN=5%,MAX=100%)
        if (featureValue <= 5) featureValue = 0;
        if (featureValue >= 100) featureValue = 100;
        gasLevel = featureValue * 2.85;
        
        break;
  
        // RPM
      case 235:
        // control the gauge (MIN=0,MAX=7500)
        if (featureValue <= 0) featureValue = 0;
        if (featureValue >= 7500) featureValue = 7500;
        // TODO calculate steps for X25.168 stepper motor to specific rpm value
        currentRPM = featureValue / 10;
        break;
  
      // speed
      case 236:
        // control the gauge (MIN=0,MAX=240)
        if (featureValue <= 0) featureValue = 0;
        if (featureValue >= 230) featureValue = 230;
  
        // TODO calculate steps for X25.168 stepper motor to specific speed value
        if (featureValue == 0) currentSpeed = 0;
        if (featureValue > 0 && featureValue <= 120) currentSpeed = featureValue * 3.8;
        if (featureValue > 120 && featureValue <= 150) currentSpeed = featureValue * 3.6;
        if (featureValue > 150 && featureValue <= 230) currentSpeed = featureValue * 3.35;
        break;
  
      // low light beam on/off
      case 237:
        if(state_electricity == 1) { // turn on only when electricity is on
          if (featureValue == 1) {
            digitalWrite(light_lowbeam, HIGH);
            //analogWrite(cluster_light, clusterLightIntensity);
          }
          else {
            digitalWrite(light_lowbeam, LOW);
            //analogWrite(cluster_light, 0);
          }
        }
        break;
  
      // high light beam on/off
      case 238:
        ((int)featureValue == 1) ? digitalWrite(light_mainbeam, HIGH) : digitalWrite(light_mainbeam, LOW);
        break;
  
      // cluster backlight on/off
      case 239:
        if(state_electricity == 1) { // turn on only when electricity is on
          //clusterLightIntensity = featureValue;
          analogWrite(cluster_light, (int)featureValue);
        }
        else {
          //clusterLightIntensity = 0;
          analogWrite(cluster_light, 0);
        }
        break;
  
      // indicators on/off
      case 240:
        switch((int)featureValue) {
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
            digitalWrite(both_indicators, HIGH);
              if(state_blinker == 0) {
                tone(speaker, 500, 10);
                state_blinker = 1;
              }
            break;
          default:
            digitalWrite(left_indicator, LOW);
            digitalWrite(right_indicator, LOW);
            digitalWrite(both_indicators, LOW);
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
          ((int)featureValue == 1) ? digitalWrite(light_fogFront, HIGH) : digitalWrite(light_fogFront, LOW);
        }
        break;
  
      // rear fog light on/off
      case 242:
        if(state_electricity == 1) { // turn on only when electricity is on
          ((int)featureValue == 1) ? digitalWrite(light_fogRear, HIGH) : digitalWrite(light_fogRear, LOW);
        }
        break;
  
      // parking brake light on/off
      case 244:
        if(state_electricity == 1) { // turn on only when electricity is on
          ((int)featureValue == 1) ? digitalWrite(warn_handbrake, HIGH) : digitalWrite(warn_handbrake, LOW);
        }
        break;
  
      // motor brake light on/off
      case 245:
        if(state_electricity == 1) { // turn on only when electricity is on
          ((int)featureValue > 0) ? digitalWrite(warn_abs, HIGH) : digitalWrite(warn_abs, LOW);
        }
        break;
  
      // engine damage report
      case 246:
        if(state_electricity == 1) { // turn on only when electricity is on
          ((int)featureValue >= 20) ? digitalWrite(warn_engine, HIGH) : digitalWrite(warn_engine, LOW);
          ((int)featureValue >= 25) ? digitalWrite(warn_failure, HIGH) : digitalWrite(warn_failure, LOW);
          ((int)featureValue >= 30) ? digitalWrite(warn_oil, HIGH) : digitalWrite(warn_oil, LOW);
          ((int)featureValue >= 40) ? digitalWrite(warn_temp, HIGH) : digitalWrite(warn_temp, LOW);
          ((int)featureValue >= 65) ? digitalWrite(airbag, HIGH) : digitalWrite(airbag, LOW);
        }     
        break;
  
      // oil pressure warning light on/off
      case 247:
        if(state_electricity == 1) { // turn on only when electricity is on
          ((int)featureValue == 1) ? digitalWrite(warn_oil, HIGH) : digitalWrite(warn_oil, LOW);
        }
        break;
  
      // battery voltage warning light on/off
      case 248:
        if(state_electricity == 1) { // turn on only when electricity is on
          ((int)featureValue == 1) ? digitalWrite(warn_battery, HIGH) : digitalWrite(warn_battery, LOW);
        }
        break;
  
      // trailer attache indicator on/off
      case 249:
        if(state_electricity == 1) { // turn on only when electricity is on
          ((int)featureValue == 1) ? digitalWrite(trailer, HIGH) : digitalWrite(trailer, LOW);
        }
        break;
  
      // beacon light indicator on/off
      case 250:
        if(state_electricity == 1) { // turn on only when electricity is on
          ((int)featureValue == 1) ? digitalWrite(beacon, HIGH) : digitalWrite(beacon, LOW);
        }
        break;
  
      case 251:
        if(state_electricity == 1) { // turn on only when electricity is on
          if((int)featureValue != 0) {
            lastTime = featureText;
            if(showTime) {
              String msggg = (String)lastTime + (String)"              ";
              WriteLeftOLED(msggg.c_str(), false, true);  
            }
          }
        }
        break;  
  
      
      case 252:
        if(state_electricity == 1) { // turn on only when electricity is on
          if((int)featureValue != 0) {
            if((String)featureText != "NULL" && (String)featureText != "") {
              if(!cityShown || lastCity != (String)(featureText)) {
                lastCity = (String)(featureText);
                String msgggCity = lastCity + (String)("                ");
                WriteRightOLED(msgggCity.c_str(), false, 1);
                cityShown = true;
              }
            }
            else {
              String msgggCity = (String)("                ");
              lastCity = "";
              WriteRightOLED(msgggCity.c_str(), false, 1);
              cityShown = false;
            }
  
            String restDistText = (String)(featureValue / 10);
            char* restDist = restDistText.c_str();
            
            restDist[strlen(restDist)-1] = '\0';
            
            String msggg = (String)"TRIP " + (String)(restDist) + (String)" km          ";
            WriteRightOLED(msggg.c_str(), false, 0);
          }
         else {
            WriteRightOLED("                ", false, 0);
          }
        }
        break;   
  
      case 253:
        if(state_electricity == 1) { // turn on only when electricity is on
          if ((int)featureValue == 0) {
            digitalWrite(cruisecontrol, LOW);
            showTime = true;
            WriteLeftOLED("                ", false, false);
            String msggg = (String)lastTime + (String)"              ";
            WriteLeftOLED(msggg.c_str(), false, true);
          }
          else {
            digitalWrite(cruisecontrol, HIGH);
            String msggg = (String)featureValue + (String)" km/h           ";
            showTime = false;
            WriteLeftOLED(msggg.c_str(), false, false);
          }
          
        }
        break;   
  
      case 254:
        if(state_electricity == 1) { // turn on only when electricity is on
          if((int)featureValue != 0) {
            
            msgFuel = featureValue;
  
            String msggg = (String)("# ") + (String)(msgFuel) + (String)(" l ~ ") + (String)(msgRange) + (String)(" km          ");
            WriteRightOLED(msggg.c_str(), false, 3);
          }
        }
        break;
  
      case 255:
        if(state_electricity == 1) { // turn on only when electricity is on
          if((int)featureValue != 0) {
            
            msgRange = featureValue;
          }
        }
        break;
  
  
      // light test
      case 888:
        if ((int)featureValue == 1 && state_electricity == 0) testLights();
        break;  
  
      // zero gauges
      case 999:
        gauge_rpm.zero();
        gauge_speed.zero();
        gauge_temp.zero();
        gauge_gas.zero();
        break;
    }
  
    if(buttonPressed == true) {
      gauge_rpm.zero();
      gauge_speed.zero();
      gauge_temp.zero();
      gauge_gas.zero();  
      buttonPressed = false;
    }
  }
}

void WriteRightOLED(char* msg, bool refill, int line) {
  //char msgTemp[60];
  //sprintf(msgTemp, "%s", msg);
  
  if(refill) {
    Multi_OLEDFill(0, 0);
  }
  else {
    Multi_OLEDWriteString(0, 0, line, msg, FONT_NORMAL, 0);
  }
}

void WriteLeftOLED(char* msg, bool refill, bool middle) {
  if(refill) Multi_OLEDFill(1, 0);
  //else Multi_OLEDWriteString(1, 25, -1, (char*)"  :  ", FONT_LARGE, 0);
  if(middle) {
    Multi_OLEDWriteString(1, 25, -1, msg, FONT_LARGE, 0);
  }
  else {
    Multi_OLEDWriteString(1, 0, -1, msg, FONT_LARGE, 0);
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
  pinMode(trailer, OUTPUT);
  pinMode(beacon, OUTPUT);
  pinMode(both_indicators, OUTPUT);
  pinMode(warn_safe, OUTPUT);
  pinMode(button_left, INPUT_PULLUP);
  pinMode(button_ext1, INPUT_PULLUP);

  // Initialize OLED displays
  Multi_I2CInit(sda_list, scl_list, speed_list, NUM_BUSES);
  Multi_OLEDInit(bus_list, addr_list, type_list, flip_list, invert_list, NUM_DISPLAYS);
  Multi_OLEDSetContrast(0, 20);
  Multi_OLEDSetContrast(1, 20);

  // Turn off all lights and set needles to 0
  clusterTurnOff();
}

void loop() {
  // update motors frequently to allow them to step
  //gauge_rpm.update();
  //gauge_speed.update();
  //gauge_temp.update();
  //gauge_gas.update();

  if(digitalRead(button_left) == LOW || digitalRead(button_ext1) == LOW) {
    buttonPressed = true;
  }

  busCommunication();

  gauge_rpm.setPosition(currentRPM);
  gauge_rpm.updateBlocking();

  gauge_speed.setPosition(currentSpeed);
  gauge_speed.updateBlocking();

  gauge_temp.setPosition(currentTemp);
  gauge_temp.updateBlocking();

  gauge_gas.setPosition(gasLevel);
  gauge_gas.updateBlocking();
}
