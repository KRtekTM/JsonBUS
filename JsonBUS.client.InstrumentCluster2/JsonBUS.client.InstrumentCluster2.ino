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
* const char* json = "{\"GameRunning\":false,\"ElectricityOn\":false,\"Engine\":false,\"BlinkerL\":false,\"BlinkerR\":false,\"Beacon\":false,\"LowBeam\":false,\"MainBeam\":false,\"ParkingBrake\":false,\"Abs\":false,\"Trailer\":false,\"DashLightLevel\":100,\"FuelAmountLitres\":2910.12,\"FuelRangeKm\":1365.12,\"FuelAmountPercent\":227,\"Speedometer\":622.22,\"RPM\":6222,\"WaterTemp\":622.22,\"DamageState\":222,\"TimeMessage\":\"05:01:01\",\"TripDestination\":\"Sankt Peterburga\",\"TripDistance\":88888.88,\"CruiseControl\":false,\"CruiseControlValue\":123.23,\"Airbag\":false,\"FogFront\":false,\"FogRear\":false,\"OilWarn\":false,\"EngineWarn\":false,\"Failure\":false,\"Immobilizer\":false,\"SafeWarn\":false,\"Speaker\":false}";
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

bool ClusterActive = false;


void ParseJsonBUS() {
  if (Serial.available()) {      // If anything comes in Serial (USB),
    StaticJsonDocument<768> root;
    DeserializationError error = deserializeJson(root, Serial);
    
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }
  	else
  	{
  		// parseObject() succeeded
  		GameRunning = root["GameRunning"] | GameRunning;
  		ElectricityOn = root["ElectricityOn"] | ElectricityOn;
  		Engine = root["Engine"] | Engine;
  		BlinkerL = root["BlinkerL"] | BlinkerL;
  		BlinkerR = root["BlinkerR"] | BlinkerR;
  		Beacon = root["Beacon"] | Beacon;
  		LowBeam = root["LowBeam"] | LowBeam;
  		MainBeam = root["MainBeam"] | MainBeam;
  		ParkingBrake = root["ParkingBrake"] | ParkingBrake;
  		Abs = root["Abs"] | Abs;
  		Trailer = root["Trailer"] | Trailer;
  		CruiseControl = root["CruiseControl"] | CruiseControl;
      Airbag = root["Airbag"] | Airbag;
      FogFront = root["FogFront"] | FogFront;
      FogRear = root["FogRear"] | FogRear;
      OilWarn = root["OilWarn"] | OilWarn;
      EngineWarn = root["EngineWarn"] | EngineWarn;
      Failure = root["Failure"] | Failure;
      Immobilizer = root["Immobilizer"] | Immobilizer;
      SafeWarn = root["SafeWarn"] | SafeWarn;
      Speaker = root["Speaker"] | Speaker;
  		DashLightLevel = root["DashLightLevel"] | DashLightLevel;
  		FuelAmountLitres = root["FuelAmountLitres"] | FuelAmountLitres;
  		FuelRangeKm = root["FuelRangeKm"] | FuelRangeKm;
  		FuelAmountPercent = root["FuelAmountPercent"] | FuelAmountPercent;
  		Speedometer = root["Speedometer"] | Speedometer;
  		RPM = root["RPM"] | RPM;
  		WaterTemp = root["WaterTemp"] | WaterTemp;
  		DamageState = root["DamageState"] | DamageState;
  		TripDistance = root["TripDistance"] | TripDistance;
  		CruiseControlValue = root["CruiseControlValue"] | CruiseControlValue;
  		TimeMessage = root["TimeMessage"] | TimeMessage;
  		TripDestination = root["TripDestination"] | TripDestination;
      SpeedLimit = root["SpeedLimit"] | SpeedLimit;
      ForwardGearCountReached = root["ForwardGearCountReached"] | ForwardGearCountReached;
      GearSelected = root["GearSelected"] | GearSelected;
  	}
  }
}

void ClusterTurnOff() {
    analogWrite(Pin_DashLight, 0);

    digitalWrite(Pin_BlinkerL, LOW);
    digitalWrite(Pin_BlinkerR, LOW);
    digitalWrite(Pin_BlinkerBoth, LOW);
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
    digitalWrite(Pin_SafeWarn, LOW);
    digitalWrite(Pin_CruiseControl, LOW);
    digitalWrite(Pin_Trailer, LOW);
    digitalWrite(Pin_Beacon, LOW);

    digitalWrite(Pin_Speaker, LOW);
    
    Gauge_Rpm.zero();
    Gauge_Speed.zero();
    Gauge_Temp.zero();
    Gauge_Gas.zero();

    WriteLeftOLED("", true, false);
    WriteRightOLED("",true,0);
}

// Celebration is typical Skoda/Volkswagen instrument cluster lightshow when electricity is on
void ClusterCelebration() {
    Serial.end(); // reactivate on end of celebration to prevent looping
    // Ignition switch position 1
    //analogWrite(cluster_light, 10);
    Multi_OLEDWriteString(1, 0, -1, "SKODA", FONT_LARGE, 0);
    digitalWrite(Pin_SafeWarn, HIGH);
    digitalWrite(Pin_WaterTempWarn, HIGH);
    digitalWrite(Pin_LowGas, HIGH);
    digitalWrite(Pin_OilWarn, HIGH);
    digitalWrite(Pin_EngineWarn, HIGH);
    digitalWrite(Pin_Failure, HIGH);
    digitalWrite(Pin_Abs, HIGH);
    digitalWrite(Pin_Battery, HIGH); //will light up until ignition switch set to position 2
    digitalWrite(Pin_Airbag, HIGH);

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
    digitalWrite(Pin_OilWarn, LOW);
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

	if (refill) {
		Multi_OLEDFill(0, 0);
	}
	else {
		Multi_OLEDWriteString(0, 0, line, msg, FONT_NORMAL, 0);
	}
}

void WriteLeftOLED(char* msg, bool refill, bool middle) {
	if (refill) Multi_OLEDFill(1, 0);
	//else Multi_OLEDWriteString(1, 25, -1, (char*)"  :  ", FONT_LARGE, 0);
	if (middle) {
		Multi_OLEDWriteString(1, 25, -1, msg, FONT_LARGE, 0);
	}
	else {
		Multi_OLEDWriteString(1, 0, -1, msg, FONT_LARGE, 0);
	}
}

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
    ClusterTurnOff();
}

void loop() {
    Gauge_Rpm.update();
    Gauge_Speed.update();
    Gauge_Temp.update();
    Gauge_Gas.update();
    
    ParseJsonBUS();
    
    /*Gauge_Rpm.updateBlocking();
    Gauge_Speed.updateBlocking();
    Gauge_Temp.updateBlocking();
    Gauge_Gas.updateBlocking();*/
    
    if(GameRunning) {
        ClusterActive = true;

        if(Engine) { //if(Engine || CelebrationMode)
            if(RPM < 0) {
                RPM = 0;
            }
            else if(RPM > 7500) {
                RPM = 7500;
            }
            Gauge_Rpm.setPosition(RPM / 10); // gauge range 750 steps for 7500 possible values
        }
        
        if(Engine) { //if(Engine || CelebrationMode)
            if(Speedometer < 0) {
                Speedometer = 0;
            }
            else if(Speedometer > 240) {
                Speedometer = 240;
            }
            Gauge_Speed.setPosition((3.11603 * Speedometer) + 40.8874);
        }

        // Temp gauge range 50°C-130°C
        if(ElectricityOn) {
            if(WaterTemp < 50) {
                WaterTemp = 50;
            }
            else if(WaterTemp > 130) {
                WaterTemp = 130;
            }
            Gauge_Temp.setPosition((WaterTemp - 50) * 3.25); // gauge range 260 steps for 80 possible values
        }

        if(ElectricityOn) {
            if(FuelAmountPercent < 0) {
                FuelAmountPercent = 0;
            }
            else if(FuelAmountPercent > 100) {
                FuelAmountPercent = 100;
            }
            Gauge_Gas.setPosition(FuelAmountPercent * 2.6); // gauge range 260 steps for 100 possible values
        }

        if(ElectricityOn) {
            analogWrite(Pin_DashLight, DashLightLevel);
        }

        if(ElectricityOn) {
            digitalWrite(Pin_BlinkerL, BlinkerL ? HIGH : LOW);
            digitalWrite(Pin_BlinkerR, BlinkerR ? HIGH : LOW);
            //digitalWrite(Pin_BlinkerBoth, (BlinkerL && BlinkerR) ? HIGH : LOW);
            digitalWrite(Pin_Airbag, Airbag ? HIGH : LOW);
            digitalWrite(Pin_LowBeam, LowBeam ? HIGH : LOW);
            //digitalWrite(Pin_MainBeam, MainBeam ? HIGH : LOW);
            digitalWrite(Pin_FogFront, FogFront ? HIGH : LOW);
            digitalWrite(Pin_FogRear, FogRear ? HIGH : LOW);
            digitalWrite(Pin_WaterTempWarn, (WaterTemp >= 120) ? HIGH : LOW);
            digitalWrite(Pin_LowGas, (FuelAmountPercent <= 15) ? HIGH : LOW);
            digitalWrite(Pin_OilWarn, OilWarn ? HIGH : LOW);
            digitalWrite(Pin_EngineWarn, EngineWarn ? HIGH : LOW);
            digitalWrite(Pin_Failure, Failure ? HIGH : LOW);
            digitalWrite(Pin_Abs, Abs ? HIGH : LOW);
            digitalWrite(Pin_Immobilizer, Immobilizer ? HIGH : LOW);
            digitalWrite(Pin_ParkingBrake, ParkingBrake ? HIGH : LOW);
            digitalWrite(Pin_Battery, Engine ? LOW : HIGH);
            digitalWrite(Pin_SafeWarn, SafeWarn ? HIGH : LOW);
            digitalWrite(Pin_CruiseControl, CruiseControl ? HIGH : LOW);
            digitalWrite(Pin_Trailer, Trailer ? HIGH : LOW);
            digitalWrite(Pin_Beacon, Beacon ? HIGH : LOW);

            // damage report
        }
        digitalWrite(Pin_BlinkerBoth, (BlinkerL && BlinkerR) ? HIGH : LOW);
        digitalWrite(Pin_MainBeam, MainBeam ? HIGH : LOW);

        /*
        if(BlinkerL || BlinkerR) {
          tone(Pin_Speaker, BLINKER_ON_FREQ, 10);
        }
        else {
          tone(Pin_Speaker, BLINKER_OFF_FREQ, 10);
        }
        */
        
        if(ElectricityOn) {
            if(Speaker) {
                tone(Pin_Speaker, BEEP_FREQ);
            }
            else {
                noTone(Pin_Speaker);
            }
        }

        /* Right OLED */
        /*if(ElectricityOn) {
            // First line
            char Oled_Right_text[16];
            char* Oled_Right_text1 = "TRIP: "; // 6 chars
            char Oled_Right_text2[16];
            char* Oled_Right_text3 = " km          "; // 3 chars
            
            dtostrf(TripDistance, ((sizeof(TripDistance) < 7) ? sizeof(TripDistance) : 7), 2, Oled_Right_text2);

            strcpy(Oled_Right_text, Oled_Right_text1);
            strcpy(Oled_Right_text + strlen(Oled_Right_text1), Oled_Right_text2);
            strcpy(Oled_Right_text + strlen(Oled_Right_text1) + strlen(Oled_Right_text2), Oled_Right_text3);

            WriteRightOLED(Oled_Right_text, false, 0);

            // Second line
            if(TripDestination == "") {
                WriteRightOLED(Oled_Text_Empty, false, 1);
            }
            else {
                char Oled_Right_destination[16];
                strcpy(Oled_Right_destination, TripDestination);
                strcpy(Oled_Right_destination + strlen(TripDestination), Oled_Text_Empty);
                WriteRightOLED(Oled_Right_destination, false, 1);
            }

            // Second line
            // is empty

            // Third line
            char Oled_Right_text4[16];
            char* Oled_Right_text5 = "# "; // 2 chars
            char Oled_Right_text6[16];
            char* Oled_Right_text7 = " l ~ "; // 5 chars
            char Oled_Right_text8[16];
            char* Oled_Right_text9 = " km          "; // 3 chars

            dtostrf(FuelAmountLitres, (sizeof(FuelAmountLitres) < 4) ? sizeof(FuelAmountLitres) : 4, 0, Oled_Right_text6);
            dtostrf(FuelRangeKm, (sizeof(FuelRangeKm) < 4) ? sizeof(FuelRangeKm) : 4, 0, Oled_Right_text8);

            strcpy(Oled_Right_text4, Oled_Right_text5);
            strcpy(Oled_Right_text4 + strlen(Oled_Right_text5), Oled_Right_text6);
            strcpy(Oled_Right_text4 + strlen(Oled_Right_text5) + strlen(Oled_Right_text6), Oled_Right_text7);
            strcpy(Oled_Right_text4 + strlen(Oled_Right_text5) + strlen(Oled_Right_text6) + strlen(Oled_Right_text7), Oled_Right_text8);
            strcpy(Oled_Right_text4 + strlen(Oled_Right_text5) + strlen(Oled_Right_text6) + strlen(Oled_Right_text7) + strlen(Oled_Right_text8), Oled_Right_text9);

            WriteRightOLED(Oled_Right_text4, false, 3);
        }
*/
        /* Left OLED */
 /*       if(ElectricityOn) {
            if(CruiseControl) {
                char Oled_Left_textCC[16];
                char Oled_Left_textCC1[16];
                char* Oled_Left_textCC2 = " km/h          ";

                dtostrf(CruiseControlValue, (sizeof(CruiseControlValue) < 6) ? sizeof(CruiseControlValue) : 6, 2, Oled_Left_textCC1);

                strcpy(Oled_Left_textCC, Oled_Left_textCC1);
                strcpy(Oled_Left_textCC + strlen(Oled_Left_textCC1), Oled_Left_textCC2);

                WriteLeftOLED(Oled_Left_textCC, false, true);
            }
            else {
                WriteLeftOLED(TimeMessage, false, true);
            }
        }*/
    }
    else {
        if(ClusterActive) {
          ClusterTurnOff();
          ClusterActive = false;
        }
    }

    /* Buttons */
    if(digitalRead(Pin_ButtonLeft) == LOW) {
        Gauge_Rpm.zero();
        Gauge_Speed.zero();
        Gauge_Temp.zero();
        Gauge_Gas.zero();
    }
}
