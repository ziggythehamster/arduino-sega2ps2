#ifndef PS2Keyboard_h
#define PS2Keyboard_h

#include <Arduino.h>
#include <FidPS2Keyboard.h>
#include <FidPS2Host.h>
#include "Sega2PS2.h"

// PS/2 device commands
const byte PS2_CMD_ACK               = 0xFA;
const byte PS2_CMD_BAT_OK            = 0xAA; // Basic Assurance Test OK
const byte PS2_CMD_BREAK             = 0xF0; // the key has been let go
const byte PS2_CMD_ECHO              = 0xEE;
const byte PS2_CMD_EXTENDED          = 0xE0; // this is an extended key sequence (arrows, mostly)

// PS/2 host (incoming) commands
const byte PS2_HOSTCMD_DISABLE             = 0xF5;
const byte PS2_HOSTCMD_ECHO                = 0xEE;
const byte PS2_HOSTCMD_ENABLE              = 0xF4;
const byte PS2_HOSTCMD_DEVICE_ID           = 0xF2;
const byte PS2_HOSTCMD_RESEND              = 0xFE;
const byte PS2_HOSTCMD_RESET               = 0xFF;
const byte PS2_HOSTCMD_SET_ALL_KEY_FLAGS   = 0xFA; // Scan code set 3: Set all keys to typematic/autorepeat/make/release
const byte PS2_HOSTCMD_SET_AUTOREPEAT_ONLY = 0xF7; // Scan code set 3: Set All Keys Typematic
const byte PS2_HOSTCMD_SET_DEFAULTS        = 0xF6;
const byte PS2_HOSTCMD_SET_LEDS            = 0xED;
const byte PS2_HOSTCMD_SET_MAKE_ONLY       = 0xF9; // Scan code set 3: Set All Keys Make
const byte PS2_HOSTCMD_SET_MAKE_BREAK      = 0xF8; // Scan code set 3: Set All Keys Make/Break
const byte PS2_HOSTCMD_SET_RATE            = 0xF3; // the typematic repeat rate
const byte PS2_HOSTCMD_SET_SCANCODES       = 0xF0;

// a class to abstract the keyboard functionality
class PS2Keyboard {
  public:
    PS2Keyboard(int clk, int data);
    void processHostCommand();
    void sendCommand(byte command);
    void sendKey(button btn, bool keyUp, bool useSpecial);
    void setup();

  private:
    int _clkPin;
    int _dataPin;
    bool _enabled;
    void ack();
    void debugKey(button btn, bool keyUp, bool useSpecial);
    void processHostCommand(byte command);
    void reset();
};

#endif
