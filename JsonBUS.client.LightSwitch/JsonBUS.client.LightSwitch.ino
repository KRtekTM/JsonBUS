// VW turn indicator and light switch joystick sketch
// Ondrej Kotas, 2021
// Arduino Leonardo
//--------------------------------------------------------------------

#include <Joystick.h>

#define RIGHT_BTN 13
#define LEFT_BTN 12
#define BEAM_BTN1 11
#define BEAM_BTN2 10

Joystick_ Joystick;

int btnPins[4] = {RIGHT_BTN, LEFT_BTN, BEAM_BTN1, BEAM_BTN2}; //
uint8_t btnPinsState[4];
bool firstRun = true;
uint8_t btnState;

void setup() {
  // Initialize Button Pins
  for(int btnPin : btnPins)
  {
    pinMode(btnPin, INPUT_PULLUP);
  }

  // Initialize Joystick Library
  Joystick.begin();
}

void loop() {
  // turn off internal LED
  digitalWrite(LED_BUILTIN, LOW);
  
  // Read pin values
  for(int i=0; i < 4; i++)
  {
    /*if(btnPins[i]==BEAM_BTN1) btnState = (digitalRead(BEAM_BTN1) == LOW || digitalRead(BEAM_BTN2) == LOW) ? LOW : HIGH;
    else*/ btnState = digitalRead(btnPins[i]);
    
    if(btnPinsState[i] != btnState) {
      btnPinsState[i] = btnState;
      if(!firstRun) {
        // press and unpress button on every change
        if(btnPins[i] != BEAM_BTN2) {
          Joystick.setButton(i, HIGH);
          delay(50);
          Joystick.setButton(i, LOW);
        }
        else {
          Joystick.setButton(i, !btnState);
        }
      }
    }
  }
  
  firstRun = false;
  delay(50);
}
