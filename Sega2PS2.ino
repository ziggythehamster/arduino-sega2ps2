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

// my ribbon cable colors:
// black  = pin 1
// gray   = pin 2
// blue   = pin 3
// yellow = pin 4
// red    = pin 5
// white  = pin 6
// purple = pin 7
// green  = pin 8
// orange = pin 9

// I didn't want to put the controller or the PS/2 lines on the ICSP pins,
// and the PS/2 lines need to be 2 or 3 for IRQ purposes, so most of these
// are attached to the analog pins.
#define SC_PIN1 7
#define SC_PIN2 A1
#define SC_PIN3 A3
#define SC_PIN4 A4
#define SC_PIN6 A0
#define SC_PIN7 A2
#define SC_PIN9 A5

// specify pins for the PS/2 connection
// clock should be on pin 2 or 3 for best results because the PS/2 library uses an IRQ
// also, these are configurable, but if you use different pins, you'll have to comment out all of the defines in
// FidPS2Host.cpp.
#define PS2_CLK_PIN 2
#define PS2_DATA_PIN 4

// initialize controller and keyboard
SegaController controller(SC_PIN7, SC_PIN1, SC_PIN2, SC_PIN3, SC_PIN4, SC_PIN6, SC_PIN9);
PS2Keyboard keyboard(PS2_CLK_PIN, PS2_DATA_PIN);

// Serial port incoming command
String serialCommand = "";

// Controller states
word currentState = 0;
word lastState = 0;

// Special button functions
bool isHoldingSpecialButton = false;
bool didPressButtonWhileHoldingSpecial = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  keyboard.setup();
}

void loop() { 
  // put your main code here, to run repeatedly:
  char rxByte = 0;
  byte parsedCommandByte = 0;
  unsigned long parsedCommand = 0;

  // process an incoming host command
  keyboard.processHostCommand();

  // Handle incoming data from the serial console
  if (Serial.available() > 0) {
    rxByte = Serial.read();

    if (rxByte != '\n' && rxByte != ' ') {
      serialCommand += rxByte;
    }

    if (serialCommand.length() == 2) {
      parsedCommand = strtoul(serialCommand.c_str(), 0, 16);

      // prevent an overflow since strtoul wants an unsigned long
      if (parsedCommand <= 255 && parsedCommand >= 0) {
        parsedCommandByte = byte(parsedCommand);

        Serial.print("Sending command: ");
        Serial.println(parsedCommandByte, HEX);

        keyboard.sendCommand(parsedCommandByte);
      }

      // consume the serial command
      serialCommand = "";
    }
  }

  // send controller state changes
  currentState = controller.getState();
  sendStates();
}

void sendStates() {
  button syntheticButton;

  for (byte i = 0; i < BUTTONS; i++) {
    word last = (lastState & buttonsAvailable[i].button);
    word current = (currentState & buttonsAvailable[i].button);

    if (last != current) {
      if (current) {
        // check for special button being held down
        if (buttonsAvailable[i].ps2ScanCode == 0xFF) {
          isHoldingSpecialButton = true;
          didPressButtonWhileHoldingSpecial = false; 
        } else {
          // make
          if (isHoldingSpecialButton) {
            didPressButtonWhileHoldingSpecial = true;

            keyboard.sendKey(buttonsAvailable[i], false, true);
          } else {
            keyboard.sendKey(buttonsAvailable[i], false, false);
          }
        }
      } else {
        // check for special button being held down
        if (buttonsAvailable[i].ps2ScanCode == 0xFF) {
          // if you didn't press any other buttons while holding down the special key, make and break the special key
          if (!didPressButtonWhileHoldingSpecial) {
            // send make
            keyboard.sendKey(buttonsAvailable[i], false, true);
            
            // send break
            keyboard.sendKey(buttonsAvailable[i], true, true);
          }

          // reset state
          isHoldingSpecialButton = false;
          didPressButtonWhileHoldingSpecial = false;
        } else {
          // break
          if (isHoldingSpecialButton) {
            keyboard.sendKey(buttonsAvailable[i], true, true);
          } else {
            keyboard.sendKey(buttonsAvailable[i], true, false);
          }
        }
      }
    }
  }

  lastState = currentState;
}
