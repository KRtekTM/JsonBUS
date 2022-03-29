/* Arduino USB Joystick HID H-shifter */

/* Author: Ondrej Kotas, 2021
 * GNU GPL v3
 */

#undef DEBUG

#define NUM_BUTTONS	40
#define NUM_AXES	8	       // 8 axes, X, Y, Z, etc

#define PIN_BRAKE_BTN 53
#define PIN_BRAKE_RANGE A7
#define BRAKE_FORCE_OVERRIDE 50 // set force override parking brake button press (10=first teeth in my case)

#define PIN_MODE1_BTN 32
#define PIN_MODE2_BTN 33

int btnPins[6] = {PIN_MODE1_BTN,PIN_MODE2_BTN,34,35,36,PIN_BRAKE_BTN}; // 32+33 mode btn, 34 electricity, 35 ignition, 36 enter btn, A7 parking brake range, 53 parking brake btn

typedef struct joyReport_t {
    int16_t axis[NUM_AXES];
    uint8_t button[(NUM_BUTTONS+7)/8]; // 8 buttons per byte
} joyReport_t;

joyReport_t joyReport;

void setup(void);
void loop(void);
void setButton(joyReport_t *joy, uint8_t button);
void clearButton(joyReport_t *joy, uint8_t button);
void sendJoyReport(joyReport_t *report);


void setup() 
{
  //set pin as input buttons
  for(int btnPin : btnPins)
  {
    pinMode(btnPin, INPUT_PULLUP);
  }
    
  Serial.begin(115200);
  delay(200);

  for (uint8_t ind=0; ind<8; ind++) {
	  joyReport.axis[ind] = ind*1000;
  }
  
  for (uint8_t ind=0; ind<sizeof(joyReport.button); ind++) {
    joyReport.button[ind] = 0;
  }
}

// Send an HID report to the USB interface
void sendJoyReport(struct joyReport_t *report)
{
#ifndef DEBUG
    Serial.write((uint8_t *)report, sizeof(joyReport_t));
#else
    // dump human readable output for debugging
/*    for (uint8_t ind=0; ind<NUM_AXES; ind++) {
	Serial.print("axis[");
	Serial.print(ind);
	Serial.print("]= ");
	Serial.print(report->axis[ind]);
	Serial.print(" ");
    }
    Serial.println();
*/
    for (uint8_t ind=0; ind<NUM_BUTTONS/8; ind++) {
	Serial.print("button[");
	Serial.print(ind);
	Serial.print("]= ");
	Serial.print(report->button[ind], HEX);
	Serial.print(" ");
    }
    Serial.println();
#endif
}

// turn a button on
void setButton(joyReport_t *joy, uint8_t button)
{
    uint8_t index = button/8;
    uint8_t bit = button - 8*index;

    joy->button[index] |= 1 << bit;
}

// turn a button off
void clearButton(joyReport_t *joy, uint8_t button)
{
    uint8_t index = button/8;
    uint8_t bit = button - 8*index;

    joy->button[index] &= ~(1 << bit);
}

uint8_t button=0;	// current button
bool press = true;	// turn buttons on?

/* Turn each button on in sequence 1 - 40, then off 1 - 40
 * add values to each axis each loop
 */
void loop() 
{
    // Turn on a different button each time
    /*if (press) {
	setButton(&joyReport, button);
    } else {
	clearButton(&joyReport, button);
    }
*/
  // Create Matrix of digital pin values
  int i = 0;
  for(int btnPin : btnPins)
  {
    uint8_t btnState = digitalRead(btnPin);
        
    if (btnState == LOW) // PRESSED
    {
      // Prevent reporting mode button as 2 pressed buttons when value set on max
      //if(btnPin == PIN_MODE1_BTN && digitalRead(PIN_MODE2_BTN) == LOW) {
      if(btnPin == PIN_MODE2_BTN) {
        clearButton(&joyReport, i-1);
      }
      #ifdef DEBUG
        Serial.println((int)analogRead(PIN_BRAKE_RANGE));
      #endif
      if(btnPin == PIN_BRAKE_BTN && (int)analogRead(PIN_BRAKE_RANGE) > BRAKE_FORCE_OVERRIDE) {
        clearButton(&joyReport, i);
        
      }
      else {
        setButton(&joyReport, i);
        #ifdef DEBUG
          /*Serial.println("button on pin:");
          Serial.println(btnPin);
          Serial.println("pressed");*/
        #endif
      }
    } else if (btnState == HIGH) // NOT PRESSED
    {
      clearButton(&joyReport, i);
      #ifdef DEBUG
       /*Serial.println("button on pin:");
       Serial.println(btnPin);
       Serial.println("cleared");*/
      #endif
    }

    i++;
  }

    /* Move all of the axes */
    /*for (uint8_t axis = 0; axis < 5; axis++) {
      joyReport.axis[axis] = map(analogRead(axis), 0, 1023, -32768,32767 );
    }*/
    joyReport.axis[0]=0;
    joyReport.axis[1]=0;
    joyReport.axis[2]=0;
    joyReport.axis[3]=0;
    joyReport.axis[4]=0;
    joyReport.axis[5]=0;
    joyReport.axis[6]=0;
    joyReport.axis[7]=0;
    /*for (uint8_t ind=0; ind<8; ind++) {
	    joyReport.axis[ind] += 10 * (ind+1);
    }*/

    sendJoyReport(&joyReport);

    /*button++;
    if (button >= 40) {
       button = 0;
       press = !press;
    }*/
    
    delay(100);
}
