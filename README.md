Sega2PS2
========

This is a simple Arduino project which converts Sega controller presses into PS/2 keyboard presses. You might want to do this if you want to play old MS-DOS games using a Sega controller instead of a keyboard.

Configuration
-------------

* Pin 2 is PS/2 clock and pin 4 is PS/2 data. These are hard-coded in the FidPS2 library, though you can edit the FidPS2 library to comment-out the `#ifdef`s and you can use any pins. However, you probably want to use these pins because FidPS2 supports interrupts.
* Pins A0-A5 and pin 7 are the Sega controller pins. See Sega2PS2.ino for what DB-9 pins they map to. You can change these, but since Sega controllers are electrically compatible with Atari/Commodore joysticks, you might one day want to add support for reading analog pot values.
* You can safely power the Arduino and controller from the +5V provided by the PS/2 port, but only if you supply the power to the Arduino's +5V pin. This means that you cannot have the power connected via PS/2 and USB at the same time. Since PS/2 is not hot-pluggable, you probably won't find this to be a problem.

Libraries Used
--------------

* [FidPS2](https://github.com/ndusart/ps2-keyboard) - this is the only library I found which adequately supported operating the Arduino as a PS/2 device. It's also the only one which supports interrupts. Score.
* [SegaController](https://github.com/jonthysell/SegaController)

Known Issues
------------

* If you're holding down a button (for example, a button on the d-pad) and then hold start because you want to use a shifted/special button, and then release the d-pad button and then press a shifted button, the break from the first button never gets sent and it acts like the button is held down when it isn't. The common case of this is probably easy to resolve by making the D-Pad special buttons the same as the normal buttons, so at worst the computer sees two "make"s.


Wishlist
--------

* Support for a client keyboard. My device is a Toshiba Satellite 100CS laptop, so I can use its internal keyboard to do stuff, but it would be more convenient to be able to use an external keyboard.
