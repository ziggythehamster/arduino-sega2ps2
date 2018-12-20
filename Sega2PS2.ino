// Sega controller -> PS/2 keyboard
//
// Author: Keith Gable <ziggy@ignition-project.com>
//
// This sketch takes a Sega controller and maps its buttons to PS/2 keyboard commands. I'm not a C++ programmer or an embedded programmer,
// so this code doesn't have tests and is not likely of high quality.

#include <SegaController.h>
#include "Sega2PS2.h"
#include "PS2Keyboard.h"

// Controller DB9 pins (looking face-on to the end of the plug):
//
// 5 4 3 2 1
//  9 8 7 6
//
// Connect pin 5 to +5V and pin 8 to GND
// Connect the remaining pins to digital I/O pins (see below)

// my color scheme:
// black  = pin 1
// gray   = pin 2
// blue   = pin 3
// yellow = pin 4
// red    = pin 5
// white  = pin 6
// purple = pin 7
// green  = pin 8
// orange = pin 9

#define SC_PIN1 4
#define SC_PIN2 A1
#define SC_PIN3 A3
#define SC_PIN4 A4
#define SC_PIN6 A0
#define SC_PIN7 A2
#define SC_PIN9 A5

// specify pins for the PS/2 connection
// using A0 and A1 to avoid the ICSP pins 10 and 11
#define PS2_CLK_PIN 3
#define PS2_DATA_PIN 2

// initialize controller and keyboard
SegaController controller(SC_PIN7, SC_PIN1, SC_PIN2, SC_PIN3, SC_PIN4, SC_PIN6, SC_PIN9);
PS2Keyboard keyboard(PS2_CLK_PIN, PS2_DATA_PIN);

// Controller states
word currentState = 0;
word lastState = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  keyboard.setup();
}

void loop() { 
  // put your main code here, to run repeatedly:
  if (keyboard.isCommunicationInhibited()) {
    digitalWrite(LED_BUILTIN, LOW);
  } else {
    digitalWrite(LED_BUILTIN, HIGH);

    if (keyboard.isHostWaiting()) {
      keyboard.processHostCommand();
    } else {
      currentState = controller.getState();
      sendStates();
    }
  }
}

void sendStates() {
  for (byte i = 0; i < BUTTONS; i++) {
    word last = (lastState & buttonsAvailable[i].button);
    word current = (currentState & buttonsAvailable[i].button);

    if (last != current) {
      if (current) {
        // make
        keyboard.sendKey(buttonsAvailable[i], false);
      } else {
        // break
        keyboard.sendKey(buttonsAvailable[i], true);
      }
    }
  }

  lastState = currentState;
}
