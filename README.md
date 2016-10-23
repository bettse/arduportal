
#Arduportal

Maybe I'll rename to "portal of arduboy".  This is an emulator of the skylanders power of power using an [Arduboy](https://www.arduboy.com).  I'm using [PlatformIO](http://platformio.org) instead of the Arduino IDE, so that will explain the folder structure and the platformio.ini.  Along with this code, I made some changes to the Arduino core library, and used a custom platformio 'board' file.

## Arduino changes

Exactly what changes I made, I'm not certain because I wasn't tracking them carefully.  In essence, I removed the CDC USB profile.  I'm going to try to recreate and document my changes the next time.

## Platformio board

Copy arduportal.json to `~/.platformio/platforms/atmelavr/boards/`

## Firmware binary

In the hopes that it may be of benefit, I've added a bin folder with the hex and elf firmware files.

## Use

The 'B' button places the token on the virtual portal.  The 'A' button removes the token from the virtual portal.

I initially loaded the token data using a separate utility to copy the data directly into EEPROM, but I think it should be possible to load a token by using the W commands.

## Arduboy specs
 * Processor: ATmega32u4 (same as Arduino Leonardo & Micro)
 * Memory: 32KB Flash, 2.5KB RAM, 1KB EEPROM
 * Connectivity: USB 2.0 w/ built in HID profile
 * Inputs: 6 momentary tactile buttons
 * Outputs: 128x64 1Bit OLED, 2 Ch. Piezo Speaker & Blinky LED
 * Battery: 180 mAh Thin-Film Lithium Polymer
 * Programming: Codebender, Arduino IDE, GCC & AVRDude


