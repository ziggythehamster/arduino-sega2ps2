#ifndef Sega2PS2_h
#define Sega2PS2_h

#include <SegaController.h>

typedef struct {
  word button;
  String mnemonic;
  bool isExtendedKey;
  byte ps2ScanCode;
} button;

// For a list of PS/2 scan codes, see: <https://web.archive.org/web/20180202062621/http://www.computer-engineering.org/ps2keyboard/scancodes2.html>
const button buttonsAvailable[] = {
  { SC_BTN_UP,    "SC_BTN_UP",    true,  0x75 }, // cursor up
  { SC_BTN_DOWN,  "SC_BTN_DOWN",  true,  0x72 }, // cursor down
  { SC_BTN_LEFT,  "SC_BTN_LEFT",  true,  0x6B }, // cursor left
  { SC_BTN_RIGHT, "SC_BTN_RIGHT", true,  0x74 }, // cursor right
  { SC_BTN_START, "SC_BTN_START", false, 0x76 }, // escape
  { SC_BTN_A,     "SC_BTN_A",     false, 0x5A }, // enter
  { SC_BTN_B,     "SC_BTN_B",     false, 0x29 }, // space
  { SC_BTN_C,     "SC_BTN_C",     false, 0x14 }, // left control
  { SC_BTN_X,     "SC_BTN_X",     false, 0x16 }, // 1
  { SC_BTN_Y,     "SC_BTN_Y",     false, 0x1E }, // 2
  { SC_BTN_Z,     "SC_BTN_Z",     false, 0x26 }, // 3
  { SC_BTN_MODE,  "SC_BTN_MODE",  false, 0x0D }  // tab
};

// the number of buttons in the above array
const byte BUTTONS = 12;

#endif
