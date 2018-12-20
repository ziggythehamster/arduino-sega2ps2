#include "PS2Keyboard.h"

// the constructor
PS2Keyboard::PS2Keyboard(int clk, int data) : _clkPin(clk), _dataPin(data), _enabled(false) {}

// acknowledges the host command
void PS2Keyboard::ack() {
  fid_ps2h_write(PS2_CMD_ACK);
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

// Reads the waiting host command(s) and processes them.
void PS2Keyboard::processHostCommand() {
  byte command = 0x00;

  while (fid_ps2h_read(&command)) {
    if (command == 0x00) {
      break;
    }
    
    processHostCommand(command);
  }
}

// Takes a host command and does something with it
// Reference: <http://web.archive.org/web/20180118181151/http://www.computer-engineering.org/ps2protocol>
void PS2Keyboard::processHostCommand(byte command) {
  byte response = 0x00;

  switch (command) {
    case PS2_HOSTCMD_DISABLE:
      Serial.println("PS2_HOSTCMD_DISABLE");
      ack();
      _enabled = false;
      break;
    case PS2_HOSTCMD_ENABLE: // enable keyboard
      Serial.println("PS2_HOSTCMD_ENABLE");
      ack();
      _enabled = true;
      break;
    case PS2_HOSTCMD_DEVICE_ID: // get device ID
      Serial.println("PS2_HOSTCMD_DEVICE_ID");
      ack();
      fid_ps2h_write(0xAB);
      fid_ps2h_write(0x83);
      break;
    case PS2_HOSTCMD_ECHO: // echo
      Serial.println("PS2_HOSTCMD_ECHO");
      fid_ps2h_write(PS2_CMD_ECHO);
      break;

    case PS2_HOSTCMD_RESET: // reset
      Serial.println("PS2_HOSTCMD_RESET");
      ack();

      // reset
      reset();
      break;

    // these are commands which we pretend to handle that don't have a second value
    case PS2_HOSTCMD_SET_ALL_KEY_FLAGS:
    case PS2_HOSTCMD_SET_AUTOREPEAT_ONLY:
    case PS2_HOSTCMD_SET_DEFAULTS:
    case PS2_HOSTCMD_SET_MAKE_BREAK:
    case PS2_HOSTCMD_SET_MAKE_ONLY:
    case PS2_HOSTCMD_RESEND:
      ack();
      break;

    // these are commands which we pretend to handle that require us to grab a second value
    case PS2_HOSTCMD_SET_LEDS: // set/reset LEDs
    case PS2_HOSTCMD_SET_SCANCODES: // set scan code set
    case PS2_HOSTCMD_SET_RATE: // set typing rate
      ack();
      fid_ps2h_read(&response); // we don't do anything with this
      ack();
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
      fid_ps2h_write(PS2_CMD_EXTENDED);
    }
  
    if (keyUp) {
      fid_ps2h_write(PS2_CMD_BREAK);
    }
  
    fid_ps2h_write(btn.ps2ScanCode);
  }
}

// call this in your main sketch setup routine
void PS2Keyboard::setup() {
  fid_ps2h_init(_dataPin, _clkPin);
  reset();
}

// pretends to perform a Basic Assurance Test (BAT)
void PS2Keyboard::reset() {
  // keyboard is initially enabled on boot
  _enabled = true;

  delay(1000);

  Serial.println("Telling host BAT OK");
  fid_ps2h_write(PS2_CMD_BAT_OK);
  Serial.println("BAT OK done.");
}
