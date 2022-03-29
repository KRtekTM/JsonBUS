#include "SwitecX25.h"
#include "ArduinoJson.h"
#include <Multi_BitBang.h>
#include <Multi_OLED.h>

/* CONFIGURATION */

// standard X25.168 range 315 degrees at 1/3 degree steps
#define STEPS (315*3)

// speaker tone frequencies
#define BEEP_FREQ 4500
#define BLINKER_ON_FREQ 500
#define BLINKER_OFF_FREQ 300

// == PIN configuration ==
// Gauges
const SwitecX25 Gauge_Rpm(STEPS, A10,A11,A8,A9);
const SwitecX25 Gauge_Speed(STEPS, A14,A15,A12,A13);
const SwitecX25 Gauge_Temp(STEPS, A6,A7,A4,A5);
const SwitecX25 Gauge_Gas(STEPS, A2,A3,A0,A1);

// Lights
const int Pin_DashLight = 11;

const int Pin_BlinkerL = 52;
const int Pin_BlinkerR = 53;
const int Pin_BlinkerBoth = 30;
const int Pin_Airbag = 36;
const int Pin_LowBeam = 34;
const int Pin_MainBeam = 48;
const int Pin_FogFront = 33;
const int Pin_FogRear = 32;
const int Pin_WaterTempWarn = 46;
const int Pin_LowGas = 49;
const int Pin_OilWarn = 44;
const int Pin_EngineWarn = 37;
const int Pin_Failure = 39;
const int Pin_Abs = 38;
const int Pin_Immobilizer = 41;
const int Pin_ParkingBrake = 40;
const int Pin_Battery = 43;
const int Pin_SafeWarn = 42;
const int Pin_CruiseControl = 47;
const int Pin_Trailer = 35;
const int Pin_Beacon = 45;

const int Pin_Speaker = 50;


// Buttons
const int Pin_ButtonLeft = 9;

// EXT1 port reserve
const int Pin_Ext1_Reserve1 = 26;
const int Pin_Ext1_Reserve2 = 27;
const int Pin_Ext1_Reserve3 = 28;
const int Pin_Ext1_Reserve4 = 29;
const int Pin_Ext1_Reserve5 = 31;

// EXT3 port reserve
const int Pin_Ext3_Reserve1 = 24;
const int Pin_Ext3_Reserve2 = 25;

// OLED displays
const int Pin_SCL_Display1 = 6;
const int Pin_SDA_Display1 = 8;
const int Pin_SCL_Display2 = 5;
const int Pin_SDA_Display2 = 7;


// == OLED display settings ==
#define OLED_NUM_DISPLAYS 2
#define OLED_NUM_BUSES 2
#define OLED_CONTRAST 20
// I2C bus info1
uint8_t OledSclList[OLED_NUM_BUSES] = { Pin_SCL_Display1, Pin_SCL_Display2 };
uint8_t OledSdaList[OLED_NUM_BUSES] = { Pin_SDA_Display1, Pin_SDA_Display2 };
int32_t OledSpeedList[OLED_NUM_BUSES] = { 4000000L,4000000L };
// OLED display info
uint8_t OledBusList[OLED_NUM_DISPLAYS] = { 0,1 }; // can be multiple displays per bus
uint8_t OledAddrList[OLED_NUM_DISPLAYS] = { 0x3c, 0x3c };
uint8_t OledTypeList[OLED_NUM_DISPLAYS] = { OLED_128x32, OLED_128x32 };
uint8_t OledFlipList[OLED_NUM_DISPLAYS] = { 0,1 };
uint8_t OledInvertList[OLED_NUM_DISPLAYS] = { 0,0 };

const char* Oled_Text_Empty = "                ";

/*
* JsonBUS parser
* Version: 1.0
* example: {"GameRunning":true,"ElectricityOn":true,"Engine":false,"BlinkerL":false,"BlinkerR":false,"Beacon":false,"LowBeam":false,"MainBeam":false,"ParkingBrake":false,"Abs":false,"Trailer":false,"DashLightLevel":100.0,"FuelAmountLitres":1000.0,"FuelRangeKm":1500.0,"FuelAmountPercent":250.0,"Speedometer":0.0,"RPM":0.0,"WaterTemp":0.0,"DamageState":0,"TimeMessage":"21:52","TripDestination":"","TripDistance":0.0,"CruiseControl":false,"CruiseControlValue":0.0,"Airbag":false,"FogFront":false,"FogRear":false,"OilWarn":false,"EngineWarn":false,"Failure":false,"Immobilizer":false,"SafeWarn":false,"Speaker":false,"SpeedLimit":0.0,"ForwardGearCountReached":false,"GearSelected":0}
*/
bool GameRunning = false;
bool ElectricityOn = false;
bool Engine =  false;
bool BlinkerL = false;
bool BlinkerR = false;
bool Beacon = false;
bool LowBeam = false;
bool MainBeam = false;
bool ParkingBrake = false;
bool Abs = false;
bool Trailer = false;
bool CruiseControl = false;
bool Airbag = false;
bool FogFront = false;
bool FogRear = false;
bool OilWarn = false;
bool EngineWarn = false;
bool Failure = false;
bool Immobilizer = false;
bool SafeWarn = false;
bool Speaker = false;
bool ForwardGearCountReached = false;
int DashLightLevel = 0;
int SpeedLimit = 0;
int GearSelected = 0;
float FuelAmountLitres = 0.0;
float FuelRangeKm = 0.0;
float FuelAmountPercent = 0.0;
float Speedometer = 0.0;
float RPM = 0.0;
float WaterTemp = 0.0;
float DamageState = 0.0;
float TripDistance = 0.0;
float CruiseControlValue = 0.0;
const char* TimeMessage =  "";
const char* TripDestination =  "";

void setup() {
    Serial.begin(115200);

    // Initialize PINs
    pinMode(Pin_DashLight, OUTPUT);

    pinMode(Pin_BlinkerL, OUTPUT);
    pinMode(Pin_BlinkerR, OUTPUT);
    pinMode(Pin_BlinkerBoth, OUTPUT);
    pinMode(Pin_Airbag, OUTPUT);
    pinMode(Pin_LowBeam, OUTPUT);
    pinMode(Pin_MainBeam, OUTPUT);
    pinMode(Pin_FogFront, OUTPUT);
    pinMode(Pin_FogRear, OUTPUT);
    pinMode(Pin_WaterTempWarn, OUTPUT);
    pinMode(Pin_LowGas, OUTPUT);
    pinMode(Pin_OilWarn, OUTPUT);
    pinMode(Pin_EngineWarn, OUTPUT);
    pinMode(Pin_Failure, OUTPUT);
    pinMode(Pin_Abs, OUTPUT);
    pinMode(Pin_Immobilizer, OUTPUT);
    pinMode(Pin_ParkingBrake, OUTPUT);
    pinMode(Pin_Battery, OUTPUT);
    pinMode(Pin_SafeWarn, OUTPUT);
    pinMode(Pin_CruiseControl, OUTPUT);
    pinMode(Pin_Trailer, OUTPUT);
    pinMode(Pin_Beacon, OUTPUT);

    pinMode(Pin_Speaker, OUTPUT);

    pinMode(Pin_ButtonLeft, INPUT_PULLUP);

    // Initialize OLED displays
    Multi_I2CInit(OledSdaList, OledSclList, OledSpeedList, OLED_NUM_BUSES);
    Multi_OLEDInit(OledBusList, OledAddrList, OledTypeList, OledFlipList, OledInvertList, OLED_NUM_DISPLAYS);
    Multi_OLEDSetContrast(0, OLED_CONTRAST);
    Multi_OLEDSetContrast(1, OLED_CONTRAST);

    // Turn off all lights and set needles to 0
    clusterTurnOff();
}

void loop() {
  busCommunication();
/*
  if(ElectricityOn) {
    Gauge_Temp.setPosition((WaterTemp - 50) * 3.25); // gauge range 260 steps for 80 possible values
    Gauge_Temp.updateBlocking();
    Gauge_Gas.setPosition(FuelAmountPercent * 2.6); // gauge range 260 steps for 100 possible values
    Gauge_Gas.updateBlocking();
  }

  if(ElectricityOn && Engine) {
    Gauge_Rpm.setPosition(RPM / 10); // gauge range 750 steps for 7500 possible values
    Gauge_Rpm.updateBlocking();
    Gauge_Speed.setPosition((3.11603 * Speedometer) + 40.8874);
    Gauge_Speed.updateBlocking();
  }
*/
  /* Buttons */
  if(digitalRead(Pin_ButtonLeft) == LOW) {
      Gauge_Rpm.zero();
      Gauge_Speed.zero();
      Gauge_Temp.zero();
      Gauge_Gas.zero();
  }
}

void busCommunication() {
  if (Serial.available()) {      // If anything comes in Serial (USB),
    //Serial.write(Serial.read());   // read it and send it out Serial1 (pins 0 & 1)

    // Stream& input;
    StaticJsonDocument<768> root;
    //byte input = Serial.read();
    //Serial.write(input);
    
    DeserializationError error = deserializeJson(root, Serial);
    
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }
    else {
      // parseObject() succeeded
      GameRunning = root["GameRunning"] | GameRunning;
      
      if(ElectricityOn != root["ElectricityOn"]) {
        ElectricityOn = root["ElectricityOn"];
        (ElectricityOn) ? clusterCelebration() : clusterTurnOff();
      }

      if(ElectricityOn) {
        if(Engine != root["Engine"]) {
          if(root["Engine"]) {
            analogWrite(Pin_DashLight, 125);
            digitalWrite(Pin_Battery, LOW);
          }
          else {
            analogWrite(Pin_DashLight, 10);
            digitalWrite(Pin_Battery, HIGH);
          }
          Engine = root["Engine"];
        }
      }

      if(BlinkerL != root["BlinkerL"] || BlinkerR != root["BlinkerR"]) {
        BlinkerL = root["BlinkerL"];
        BlinkerR = root["BlinkerR"];
        
        if(BlinkerL && BlinkerR) {
          digitalWrite(Pin_BlinkerL, HIGH);
          digitalWrite(Pin_BlinkerR, HIGH);
          digitalWrite(Pin_BlinkerBoth, HIGH);
          tone(Pin_Speaker, BLINKER_ON_FREQ, 10);
        }
        else if(BlinkerL && !BlinkerR && ElectricityOn) {
          digitalWrite(Pin_BlinkerL, HIGH);
          digitalWrite(Pin_BlinkerR, LOW);
          digitalWrite(Pin_BlinkerBoth, LOW);
          tone(Pin_Speaker, BLINKER_ON_FREQ, 10);
        }
        else if(!BlinkerL && BlinkerR && ElectricityOn) {
          digitalWrite(Pin_BlinkerL, LOW);
          digitalWrite(Pin_BlinkerR, HIGH);
          digitalWrite(Pin_BlinkerBoth, LOW);
          tone(Pin_Speaker, BLINKER_ON_FREQ, 10);
        }
        else {
          digitalWrite(Pin_BlinkerL, LOW);
          digitalWrite(Pin_BlinkerR, LOW);
          digitalWrite(Pin_BlinkerBoth, LOW);
          tone(Pin_Speaker, BLINKER_OFF_FREQ, 10);
        }
      }

      if(Beacon != root["Beacon"] && ElectricityOn) {
        if(root["Beacon"]) {
          digitalWrite(Pin_Beacon, HIGH);
        }
        else {
          digitalWrite(Pin_Beacon, LOW);
        }
        Beacon = root["Beacon"];
      }

      if(LowBeam != root["LowBeam"] && ElectricityOn) {
        if(root["LowBeam"]) {
          digitalWrite(Pin_LowBeam, HIGH);
        }
        else {
          digitalWrite(Pin_LowBeam, LOW);
        }
        LowBeam = root["LowBeam"];
      }

      if(MainBeam != root["MainBeam"]) {
        if(root["MainBeam"]) {
          digitalWrite(Pin_MainBeam, HIGH);
        }
        else {
          digitalWrite(Pin_MainBeam, LOW);
        }
        MainBeam = root["MainBeam"];
      }

      if(ParkingBrake != root["ParkingBrake"] && ElectricityOn) {
        if(root["ParkingBrake"]) {
          digitalWrite(Pin_ParkingBrake, HIGH);
        }
        else {
          digitalWrite(Pin_ParkingBrake, LOW);
        }
        ParkingBrake = root["ParkingBrake"];
      }

      if(OilWarn != root["OilWarn"] && ElectricityOn && Engine) {
        if(root["OilWarn"]) {
          digitalWrite(Pin_OilWarn, HIGH);
        }
        else {
          digitalWrite(Pin_OilWarn, LOW);
        }
        OilWarn = root["OilWarn"];
      }

      /*
        Abs = root["Abs"] | Abs;
        Trailer = root["Trailer"] | Trailer;
        CruiseControl = root["CruiseControl"] | CruiseControl;
        Airbag = root["Airbag"] | Airbag;
        FogFront = root["FogFront"] | FogFront;
        FogRear = root["FogRear"] | FogRear;
        EngineWarn = root["EngineWarn"] | EngineWarn;
        Failure = root["Failure"] | Failure;
        Immobilizer = root["Immobilizer"] | Immobilizer;
        SafeWarn = root["SafeWarn"] | SafeWarn;
        Speaker = root["Speaker"] | Speaker;
        DashLightLevel = root["DashLightLevel"] | DashLightLevel;
        FuelAmountLitres = root["FuelAmountLitres"] | FuelAmountLitres;
        FuelRangeKm = root["FuelRangeKm"] | FuelRangeKm;
        DamageState = root["DamageState"] | DamageState;
        TripDistance = root["TripDistance"] | TripDistance;
        CruiseControlValue = root["CruiseControlValue"] | CruiseControlValue;
        TimeMessage = root["TimeMessage"] | TimeMessage;
        TripDestination = root["TripDestination"] | TripDestination;
        SpeedLimit = root["SpeedLimit"] | SpeedLimit;
        ForwardGearCountReached = root["ForwardGearCountReached"] | ForwardGearCountReached;
        GearSelected = root["GearSelected"] | GearSelected;
      */

        if(RPM != root["RPM"] && ElectricityOn && Engine) {
          RPM = root["RPM"];
          
          if(RPM < 0) {
              RPM = 0;
          }
          else if(RPM > 7500) {
              RPM = 7500;
          }
        }
        
        if(Speedometer != root["Speedometer"] && ElectricityOn && Engine) {
          Speedometer = root["Speedometer"];
          
          if(Speedometer < 0) {
              Speedometer = 0;
          }
          else if(Speedometer > 240) {
              Speedometer = 240;
          }
        }
        
        // Temp gauge range 50°C-130°C
        if(WaterTemp != root["WaterTemp"] && ElectricityOn) {
          WaterTemp = root["WaterTemp"];
          
          if(WaterTemp < 50) {
              WaterTemp = 50;
          }
          else if(WaterTemp > 130) {
              WaterTemp = 130;
          }
        }

        if(FuelAmountPercent != root["FuelAmountPercent"] && ElectricityOn) {
          FuelAmountPercent = root["FuelAmountPercent"];
          
          if(FuelAmountPercent < 0) {
              FuelAmountPercent = 0;
          }
          else if(FuelAmountPercent > 100) {
              FuelAmountPercent = 100;
          }
        }


    }
  }
}

void clusterTurnOff() {
  WriteLeftOLED("", true, false);
  WriteRightOLED("",true,0);

  analogWrite(Pin_DashLight, 0);
  digitalWrite(Pin_BlinkerL, LOW);
  digitalWrite(Pin_BlinkerR, LOW);
  digitalWrite(Pin_Airbag, LOW);
  digitalWrite(Pin_LowBeam, LOW);
  digitalWrite(Pin_MainBeam, LOW);
  digitalWrite(Pin_FogFront, LOW);
  digitalWrite(Pin_FogRear, LOW);
  digitalWrite(Pin_WaterTempWarn, LOW);
  digitalWrite(Pin_LowGas, LOW);
  digitalWrite(Pin_OilWarn, LOW);
  digitalWrite(Pin_EngineWarn, LOW);
  digitalWrite(Pin_Failure, LOW);
  digitalWrite(Pin_Abs, LOW);
  digitalWrite(Pin_Immobilizer, LOW);
  digitalWrite(Pin_ParkingBrake, LOW);
  digitalWrite(Pin_Battery, LOW);
  digitalWrite(Pin_CruiseControl, LOW);
  digitalWrite(Pin_Trailer, LOW);
  digitalWrite(Pin_Beacon, LOW);
  digitalWrite(Pin_BlinkerBoth, LOW);
  digitalWrite(Pin_SafeWarn, LOW);

  GameRunning = false;
  ElectricityOn = false;
  Engine =  false;
  BlinkerL = false;
  BlinkerR = false;
  Beacon = false;
  LowBeam = false;
  MainBeam = false;
  ParkingBrake = false;
  Abs = false;
  Trailer = false;
  CruiseControl = false;
  Airbag = false;
  FogFront = false;
  FogRear = false;
  OilWarn = false;
  EngineWarn = false;
  Failure = false;
  Immobilizer = false;
  SafeWarn = false;
  Speaker = false;
  ForwardGearCountReached = false;
  DashLightLevel = 0;
  SpeedLimit = 0;
  GearSelected = 0;
  FuelAmountLitres = 0.0;
  FuelRangeKm = 0.0;
  FuelAmountPercent = 0.0;
  Speedometer = 0.0;
  RPM = 0.0;
  WaterTemp = 0.0;
  DamageState = 0.0;
  TripDistance = 0.0;
  CruiseControlValue = 0.0;
  TimeMessage =  "";
  TripDestination =  "";

  Gauge_Rpm.zero();
  Gauge_Speed.zero();
  Gauge_Temp.zero();
  Gauge_Gas.zero();
}

// Celebration is typical Skoda/Volkswagen instrument cluster lightshow when electricity is on
void clusterCelebration() {
  Serial.end(); // reactivate on end of celebration to prevent looping
  // Ignition switch position 1
  analogWrite(Pin_DashLight, 10);
  Multi_OLEDWriteString(1, 0, -1, "SKODA", FONT_LARGE, 0);
  digitalWrite(Pin_Airbag, HIGH);
  digitalWrite(Pin_WaterTempWarn, HIGH);
  digitalWrite(Pin_LowGas, HIGH);
  digitalWrite(Pin_OilWarn, HIGH);
  OilWarn = true;
  digitalWrite(Pin_EngineWarn, HIGH);
  digitalWrite(Pin_Failure, HIGH);
  digitalWrite(Pin_Abs, HIGH);
  digitalWrite(Pin_Battery, HIGH); //will light up until ignition switch set to position 2
  digitalWrite(Pin_SafeWarn, HIGH);

  // Needle sweep start
  //gauge_rpm.setPosition(750);
  //gauge_rpm.updateBlocking();
  //gauge_speed.setPosition(750);
  //gauge_speed.updateBlocking();

  delay(500);
  
  digitalWrite(Pin_Immobilizer, HIGH);
  tone(Pin_Speaker, BEEP_FREQ);

  delay(500);
  
  // Needle sweep end
  //gauge_rpm.setPosition(0);
  //gauge_rpm.updateBlocking();
  //gauge_speed.setPosition(0);
  //gauge_speed.updateBlocking();
  digitalWrite(Pin_LowGas, LOW);

  delay(500);
  
  digitalWrite(Pin_Immobilizer, LOW);
  digitalWrite(Pin_SafeWarn, LOW);
  noTone(Pin_Speaker);

  delay(500);

  digitalWrite(Pin_EngineWarn, LOW);

  delay(50);
  
  digitalWrite(Pin_Airbag, LOW);
  
  delay(200);
  
  digitalWrite(Pin_Failure, LOW);

  delay(100);
  
  digitalWrite(Pin_Abs, LOW);
  
  delay(150);

  digitalWrite(Pin_Immobilizer, HIGH);
  tone(Pin_Speaker, BEEP_FREQ);
  
  delay(500);
  
  digitalWrite(Pin_WaterTempWarn, LOW);

  delay(500);
  //digitalWrite(Pin_OilWarn, LOW);
  digitalWrite(Pin_Immobilizer, LOW);
  noTone(Pin_Speaker);
  
  WriteLeftOLED("", true, false);

  delay(1000);
  digitalWrite(Pin_Immobilizer, HIGH);
  tone(Pin_Speaker, BEEP_FREQ);

  delay(1000);
  digitalWrite(Pin_Immobilizer, LOW);
  noTone(Pin_Speaker);

  //String msggg = (String)"CC 0 km/h           ";
  //WriteRightOLED(msggg.c_str(), false, 3);
        
  Serial.begin(115200);
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
