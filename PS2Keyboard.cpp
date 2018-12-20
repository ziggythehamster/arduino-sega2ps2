#include "PS2Keyboard.h"

// the constructor
PS2Keyboard::PS2Keyboard(int clk, int data) : _clkPin(clk), _dataPin(data), _enabled(false), _ps2dev(clk, data, 400) {}

// acknowledges the host command
void PS2Keyboard::ack() {
  while(_ps2dev.devWrite(PS2_CMD_ACK));
}

// logs that a button on the controller was pressed
void PS2Keyboard::debugKey(button btn, bool keyUp) {
  Serial.print(btn.mnemonic);

  if (keyUp) {
    Serial.print(" break");
  } else {
    Serial.print(" make");
  }

  if (!_enabled) {
    Serial.print(" (but keyboard disabled)");
  }

  Serial.print('\n');
}

// The device can neither send nor receive if communication is inhibited
bool PS2Keyboard::isCommunicationInhibited() {
  return digitalRead(_clkPin) == LOW;
}

// Checks if the host wants to communicate
bool PS2Keyboard::isHostWaiting() {
  return digitalRead(_dataPin) == LOW;
}

// Reads the waiting host command and processes it.
void PS2Keyboard::processHostCommand() {
  byte command = 0x00;

  while(_ps2dev.devRead(&command));
  processHostCommand(command);
}

// Takes a host command and does something with it
// Reference: <http://web.archive.org/web/20180118181151/http://www.computer-engineering.org/ps2protocol>
void PS2Keyboard::processHostCommand(byte command) {
  byte response = 0x00;

  switch (command) {
    case PS2_HOSTCMD_DISABLE:
      Serial.println("PS2_HOSTCMD_DISABLE");
      ack();
      delay(20);
      _enabled = false;
      break;
    case PS2_HOSTCMD_ENABLE: // enable keyboard
      Serial.println("PS2_HOSTCMD_ENABLE");
      ack();
      delay(20);
      _enabled = true;
      break;
    case PS2_HOSTCMD_DEVICE_ID: // get device ID
      Serial.println("PS2_HOSTCMD_DEVICE_ID");
      ack();
      delay(20);
      while(_ps2dev.devWrite(0xAB));
      delay(20);
      while(_ps2dev.devWrite(0x83));
      delay(20);
      break;
    case PS2_HOSTCMD_ECHO: // echo
      Serial.println("PS2_HOSTCMD_ECHO");
      while(_ps2dev.devWrite(PS2_CMD_ECHO));
      delay(20);
      break;

    case PS2_HOSTCMD_RESET: // reset
      Serial.println("PS2_HOSTCMD_RESET");

      // redo the setup procedure
      setup();
      break;

    // these are commands which we pretend to handle that don't have a second value
    case PS2_HOSTCMD_SET_ALL_KEY_FLAGS:
    case PS2_HOSTCMD_SET_AUTOREPEAT_ONLY:
    case PS2_HOSTCMD_SET_DEFAULTS:
    case PS2_HOSTCMD_SET_MAKE_BREAK:
    case PS2_HOSTCMD_SET_MAKE_ONLY:
    case PS2_HOSTCMD_RESEND:
      ack();
      delay(20);
      break;

    // these are commands which we pretend to handle that require us to grab a second value
    case PS2_HOSTCMD_SET_LEDS: // set/reset LEDs
    case PS2_HOSTCMD_SET_SCANCODES: // set scan code set
    case PS2_HOSTCMD_SET_RATE: // set typing rate
      ack();
      delay(20);
      while(_ps2dev.devRead(&response)); // we don't do anything with this
      delay(20);
      ack();
      delay(20);
      break;

    default:
      Serial.print("Unhandled PS/2 host command: ");
      Serial.print(command, HEX);
      Serial.print('\n');
  }
}

// take the passed button and send the PS2 command associated
void PS2Keyboard::sendKey(button btn, bool keyUp) {
  debugKey(btn, keyUp);

  if (_enabled) { 
    if (btn.isExtendedKey) {
      while(_ps2dev.devWrite(PS2_CMD_EXTENDED));
      delay(20);
    }
  
    if (keyUp) {
      while (_ps2dev.devWrite(PS2_CMD_BREAK));
      delay(20);
    }
  
    while (_ps2dev.devWrite(btn.ps2ScanCode));
    delay(20);
  }
}

// pretends to perform a Basic Assurance Test (BAT)
void PS2Keyboard::setup() {
  // keyboard is initially enabled on boot
  _enabled = true;

  Serial.println("Telling host BAT OK");
  while(_ps2dev.devWrite(PS2_CMD_BAT_OK)); // waits for the host to be ready
  Serial.println("BAT OK done.");
  delay(20);
}
