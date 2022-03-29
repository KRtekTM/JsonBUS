#include <Arduino.h>
#include <OneButton.h>
#include <Keyboard.h>
#include "ArduinoJson.h"

// PIN definitions
#define LED_ACTIVE 3
#define LED_STANDBY 9
#define BUTTON_PIN 2

// control variables, do not change
int CB_MODE = true;
unsigned long lastBlink = 0;
int lastBlinkLevel = LOW;
boolean charWritten = false;
int electricityOn = 1;
OneButton button(BUTTON_PIN,true,true);
enum channel {
  UP,
  DOWN
};

// the setup function runs once when you press reset or power the board
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_STANDBY, OUTPUT);
  pinMode(LED_ACTIVE, OUTPUT);

  button.attachClick(shortPress); // CB_MODE: nothing, CHANNEL_SWITCH_MODE: channel up
  button.attachDoubleClick(changeMode); // switch between CB_MODE and CHANNEL_SWITCH_MODE
  button.attachDuringLongPress(longPress); // CB_MODE: enable microphone in TruckersMP, CHANNEL_SWITCH_MODE: channel down
  button.attachLongPressStop(longPressEnd); // CB_MODE: nothing, CHANNEL_SWITCH_MODE: used for non-repeated sending of keystroke to channel down

  button.setPressTicks(250); // frequency to distinguish long press (default 250ms)
  button.setClickTicks(350); // frequency to distinguish single click from doubleclick (default 350ms)

  Serial.begin(115200);
}

// the loop function runs over and over again forever
void loop() {
  // turn off internal LED
  digitalWrite(LED_BUILTIN, LOW);

  // listen for JsonBUS
  //busCommunication();

  // if electricity turned on, turn on CB radio
  if(electricityOn == 1) {
    // CB_MODE turn on Standby LED
    if(CB_MODE) {
      digitalWrite(LED_ACTIVE, LOW);
      digitalWrite(LED_STANDBY, HIGH);
    }
    // CHANNEL_SWITCH_MODE blink with Standby and Active LED
    else {
      unsigned long lastBlinkNow = millis();
      if(lastBlinkNow-lastBlink >= 100) { // blink frequency in miliseconds (default 100ms)
        lastBlink = lastBlinkNow;
        lastBlinkLevel = (lastBlinkLevel == LOW) ? HIGH : LOW;
        digitalWrite(LED_ACTIVE, lastBlinkLevel);
        digitalWrite(LED_STANDBY, !lastBlinkLevel);
      }
    }
  
    // check the status of the button
    button.tick();
    delay(10); // a short wait between checking the button 
  }
  else {
    digitalWrite(LED_ACTIVE, LOW);
    digitalWrite(LED_STANDBY, LOW);
  }
}

void changeMode() {
  // Switch CB mode
  // true: CB_MODE
  // false: CHANNEL_CHANGE_MODE
  CB_MODE = (CB_MODE) ? false : true;
}

void shortPress() {
  if(!CB_MODE) {
    channelUpDown(UP);
    delay(100);
  }
}

void longPress() {
  if(CB_MODE) {
    microphoneOn();
  }
  else {
    if(!charWritten) {
      channelUpDown(DOWN);
      charWritten = true;
    }
  }
}

void longPressEnd() {
  if(!CB_MODE) {
    charWritten = false;
  }
}

void microphoneOn() {
    digitalWrite(LED_ACTIVE, HIGH);
    digitalWrite(LED_STANDBY, LOW);

    Keyboard.write('x');
    delay(25);
}

void channelUpDown(channel chnl) {
  switch (chnl) {
    case UP: 
      Keyboard.write('.');
      break;
    case DOWN:
      Keyboard.write(',');
      break;
  }
}

void busCommunication() { 
  // Loop until there is data waiting to be read
  if(Serial.available())
  {
    // JsonBUS communication
    DynamicJsonBuffer jb;
    JsonObject& root = jb.parseObject(Serial);
  
    // Test if parsing succeeds.
    if (!root.success()) {
      Serial.println("parseObject() failed");
      return;
    }
    int feature = root["f"];
    double featureValue = root["v"];
    const char* featureText = root["t"];

    switch(feature) {
      // electric on/off
      case 230:
        if(electricityOn != (int)featureValue) {
          electricityOn = (int)featureValue;
        }
        break;
    }
  }
}
