#ifndef Sega2PS2_h
#define Sega2PS2_h

#include <SegaController.h>

typedef struct {
  word button;
  String mnemonic;
  bool isExtendedKey;
  byte ps2ScanCode;
  byte ps2SpecialScanCode;
} button;

// For a list of PS/2 scan codes, see: <https://web.archive.org/web/20180202062621/http://www.computer-engineering.org/ps2keyboard/scancodes2.html>
const button buttonsAvailable[] = {
  { SC_BTN_UP,    "SC_BTN_UP",    true,  0x75, 0x00 }, // normal: cursor up     special: none
  { SC_BTN_DOWN,  "SC_BTN_DOWN",  true,  0x72, 0x00 }, // normal: cursor down   special: none
  { SC_BTN_LEFT,  "SC_BTN_LEFT",  true,  0x6B, 0x00 }, // normal: cursor left   special: none
  { SC_BTN_RIGHT, "SC_BTN_RIGHT", true,  0x74, 0x00 }, // normal: cursor right  special: none
  { SC_BTN_START, "SC_BTN_START", false, 0xFF, 0x76 }, // normal: special       special: escape (it's escape if you just pressed and released)
  { SC_BTN_A,     "SC_BTN_A",     false, 0x14, 0x5A }, // normal: left control  special: enter
  { SC_BTN_B,     "SC_BTN_B",     false, 0x29, 0x35 }, // normal: space         special: y
  { SC_BTN_C,     "SC_BTN_C",     false, 0x11, 0x00 }, // normal: left alt      special: none
  { SC_BTN_X,     "SC_BTN_X",     false, 0x16, 0x00 }, // normal: 1             special: none
  { SC_BTN_Y,     "SC_BTN_Y",     false, 0x1E, 0x00 }, // normal: 2             special: none
  { SC_BTN_Z,     "SC_BTN_Z",     false, 0x26, 0x00 }, // normal: 3             special: none
  { SC_BTN_MODE,  "SC_BTN_MODE",  false, 0x0D, 0x00 }  // normal: tab           special: none
};

// the number of buttons in the above array
const byte BUTTONS = 12;

#endif
