#include "PS2Keyboard.h"

// the constructor
PS2Keyboard::PS2Keyboard(int clk, int data) : _clkPin(clk), _dataPin(data), _enabled(false) {}

// acknowledges the host command
void PS2Keyboard::ack() {
  sendCommand(PS2_CMD_ACK);
}

// logs that a button on the controller was pressed
void PS2Keyboard::debugKey(button btn, bool keyUp, bool useSpecial) {
  Serial.print(btn.mnemonic);

  if (useSpecial) {
    Serial.print(" SPECIAL");
  }

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
      sendCommand(0xAB);
      sendCommand(0x83);
      break;
    case PS2_HOSTCMD_ECHO: // echo
      Serial.println("PS2_HOSTCMD_ECHO");
      sendCommand(PS2_CMD_ECHO);
      break;

    case PS2_HOSTCMD_RESET: // reset
      Serial.println("PS2_HOSTCMD_RESET");
      ack();

      // reset
      reset();
      break;

    case PS2_HOSTCMD_SET_LEDS: // set/reset LEDs
      Serial.print("PS2_HOSTCMD_SET_LEDS");
      ack();

      delay(20); // otherwise there might not be a value in the buffer
      fid_ps2h_read(&response);
      ack();
      
      Serial.print(" - host wants LEDs: ");
      Serial.println(response, HEX);

      if (response == 0x00) {
        // no LEDs on
        digitalWrite(LED_BUILTIN, LOW);
      } else {
        // some LEDs on
        digitalWrite(LED_BUILTIN, HIGH);
      }

      response = 0x00;
      break;

    case PS2_HOSTCMD_SET_SCANCODES: // set scan code set
      Serial.print("PS2_HOSTCMD_SET_SCANCODES");
      ack();

      delay(20); // otherwise there might not be a value in the buffer
      fid_ps2h_read(&response);
      ack();
      Serial.print(" - host wants set ");
      Serial.println(response, HEX);
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

// sends a PS/2 command byte
void PS2Keyboard::sendCommand(byte command) {
  fid_ps2h_write(command);
}

// take the passed button and send the PS2 command associated
void PS2Keyboard::sendKey(button btn, bool keyUp, bool useSpecial) {
  // Don't do anything if the key doesn't have a special function assigned
  if (useSpecial && btn.ps2SpecialScanCode == 0x00) {
    return;
  }
  
  debugKey(btn, keyUp, useSpecial);

  if (_enabled) { 
    if (btn.isExtendedKey) {
      sendCommand(PS2_CMD_EXTENDED);
    }
  
    if (keyUp) {
      sendCommand(PS2_CMD_BREAK);
    }

    if (useSpecial) {
      sendCommand(btn.ps2SpecialScanCode);
    } else {
      sendCommand(btn.ps2ScanCode);
    }
  }
}

// call this in your main sketch setup routine
void PS2Keyboard::setup() {
  reset();
}

// pretends to perform a Basic Assurance Test (BAT)
void PS2Keyboard::reset() {
  // keyboard is initially enabled on boot/reset
  _enabled = true;
  digitalWrite(LED_BUILTIN, LOW);

  fid_ps2h_init(_dataPin, _clkPin); // reinit on every reset to clear buffers

  delay(1000);

  Serial.println("Telling host BAT OK");
  sendCommand(PS2_CMD_BAT_OK);
  Serial.println("BAT OK done.");
}
