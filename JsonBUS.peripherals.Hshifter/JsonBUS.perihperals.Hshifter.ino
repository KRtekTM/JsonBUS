/* Arduino USB Joystick HID demo */

/* Author: Darran Hunt
 * Released into the public domain.
 */

#undef DEBUG

#define NUM_BUTTONS	40
#define NUM_AXES	8	       // 8 axes, X, Y, Z, etc

typedef struct joyReport_t {
    int16_t axis[NUM_AXES];
    uint8_t button[(NUM_BUTTONS+7)/8]; // 8 buttons per byte
} joyReport_t;

joyReport_t joyReport;
uint8_t btn[NUM_BUTTONS];

void setup(void);
void loop(void);
void setButton(joyReport_t *joy, uint8_t button);
void clearButton(joyReport_t *joy, uint8_t button);
void sendJoyReport(joyReport_t *report);


void setup() 
{
    //set pin to input Button , Pin 2 to 34
  for(int portId = 2; portId < 34; portId++)
  {
     pinMode( portId, INPUT_PULLUP);
  }

  // set output pins (will show selected gear - 40=R, 41=1, ..., 46=6)
  for(int portId = 40; portId <=46; portId++)
  {
    pinMode(portId, OUTPUT);  
  }

  //pinMode(49, INPUT);
  
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
    for (uint8_t ind=0; ind<NUM_AXES; ind++) {
	Serial.print("axis[");
	Serial.print(ind);
	Serial.print("]= ");
	Serial.print(report->axis[ind]);
	Serial.print(" ");
    }
    Serial.println();
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
  for(int bt = 0; bt < 32; bt ++)
  {
          btn[bt]= digitalRead(bt + 2);
  }
   // Mark pressed buttons
   for(int on = 0; on < 32; on++)
   {
       if (btn[on] == LOW)
       {
         setButton(&joyReport, on);
         //Serial.println("botao ON");
         //Serial.println(on);
       } else if (btn[on] == HIGH)
       {
         clearButton(&joyReport, on);
         //Serial.println("clear ON");
         //Serial.println(on);
       }
   }

   // Light up the LED for selected gear
   int selectedGear = -1;
   for(int gear = 0; gear<6; gear++) {
      if(btn[gear] == LOW) {
        selectedGear = gear;
      }
   }
    switch(selectedGear) {
      case 0:
        digitalWrite(40, LOW); // NEUTRAL
        btn[17] = HIGH;
        clearButton(&joyReport,17);
        digitalWrite(41, HIGH); // 1
        digitalWrite(42, LOW); // 2
        digitalWrite(43, LOW); // 3
        digitalWrite(44, LOW); // 4
        digitalWrite(45, LOW); // 5
        digitalWrite(46, LOW); // REVERSE
        break;
       case 1:
        digitalWrite(40, LOW); // NEUTRAL
        btn[17] = HIGH;
        clearButton(&joyReport,17);
        digitalWrite(41, LOW); // 1
        digitalWrite(42, HIGH); // 2
        digitalWrite(43, LOW); // 3
        digitalWrite(44, LOW); // 4
        digitalWrite(45, LOW); // 5
        digitalWrite(46, LOW); // REVERSE
        break;
       case 2:
        digitalWrite(40, LOW); // NEUTRAL
        btn[17] = HIGH;
        clearButton(&joyReport,17);
        digitalWrite(41, LOW); // 1
        digitalWrite(42, LOW); // 2
        digitalWrite(43, HIGH); // 3
        digitalWrite(44, LOW); // 4
        digitalWrite(45, LOW); // 5
        digitalWrite(46, LOW); // REVERSE
        break;
       case 3:
        digitalWrite(40, LOW); // NEUTRAL
        btn[17] = HIGH;
        clearButton(&joyReport,17);
        digitalWrite(41, LOW); // 1
        digitalWrite(42, LOW); // 2
        digitalWrite(43, LOW); // 3
        digitalWrite(44, HIGH); // 4
        digitalWrite(45, LOW); // 5
        digitalWrite(46, LOW); // REVERSE
        break;
       case 4:
        digitalWrite(40, LOW); // NEUTRAL
        btn[17] = HIGH;
        clearButton(&joyReport,17);
        digitalWrite(41, LOW); // 1
        digitalWrite(42, LOW); // 2
        digitalWrite(43, LOW); // 3
        digitalWrite(44, LOW); // 4
        digitalWrite(45, HIGH); // 5
        digitalWrite(46, LOW); // REVERSE
        break;
       case 5:
        digitalWrite(40, LOW); // NEUTRAL
        btn[17] = HIGH;
        clearButton(&joyReport,17);
        digitalWrite(41, LOW); // 1
        digitalWrite(42, LOW); // 2
        digitalWrite(43, LOW); // 3
        digitalWrite(44, LOW); // 4
        digitalWrite(45, LOW); // 5
        digitalWrite(46, HIGH); // REVERSE
        break;
       default:
        digitalWrite(40, HIGH); // NEUTRAL
        btn[17] = LOW;
        setButton(&joyReport,17);
        digitalWrite(41, LOW); // 1
        digitalWrite(42, LOW); // 2
        digitalWrite(43, LOW); // 3
        digitalWrite(44, LOW); // 4
        digitalWrite(45, LOW); // 5
        digitalWrite(46, LOW); // REVERSE
        break;
     }

    int parkingBrake = (int)analogRead(A14);
    if(parkingBrake > 750 && parkingBrake < 950) {
      setButton(&joyReport, 18);
      //Serial.println(parkingBrake);
    }
    else {
      clearButton(&joyReport,18);
    }


    /* Move all of the axes */
    /*for (uint8_t axis = 0; axis < 5; axis++) {
      joyReport.axis[axis] = map(analogRead(axis), 0, 1023, -32768,32767 );
    }*/
    // quick fix workaround - my joysticks are bad so disable by software
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
