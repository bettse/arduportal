/*
   Hello, World! example
   June 11, 2015
   Copyright (C) 2015 David Martinez
   All rights reserved.
   This code is the most basic barebones code for writing a program for Arduboy.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.
 */

#include "Arduboy.h"
#include "RawHID.h"

// make an instance of arduboy used for many functions
Arduboy arduboy;
uint8_t rawhidData[255];


// This function runs once in your game.
// use it for anything that needs to be set only once in your game.
void setup() {
  Serial.begin(115200);
  RawHID.begin(rawhidData, sizeof(rawhidData), &arduboy);

  // initiate arduboy instance
  arduboy.beginNoLogo();

  // here we set the framerate to 15, we do not need to run at
  // default 60 and it saves us battery life
  arduboy.setFrameRate(15);
}


// our main game loop, this runs once every cycle/frame.
// this is where our game logic goes.
void loop() {
  // pause render until it's time for the next frame
  if (!(arduboy.nextFrame()))
    return;


  if (arduboy.pressed(A_BUTTON)) {
    //arduboy.print(F("A"));
    arduboy.clear();
  } else if (arduboy.pressed(B_BUTTON)) {
    arduboy.print(F("B"));
  } else if (arduboy.pressed(UP_BUTTON)) {
    arduboy.setCursor(0, 0);
    arduboy.print(F("U"));
  } else if (arduboy.pressed(RIGHT_BUTTON)) {
    arduboy.print(F("R"));
  } else if (arduboy.pressed(DOWN_BUTTON)) {
    arduboy.print(F("D"));
  } else if (arduboy.pressed(LEFT_BUTTON)) {
    arduboy.print(F("L"));
  }

  auto bytesAvailable = RawHID.available();
  if (bytesAvailable) {
    while (bytesAvailable--) {
      //Serial.println(RawHID.read());
      arduboy.print(RawHID.read());
    }
  }

  // then we finaly we tell the arduboy to display what we just wrote to the display
  arduboy.display();
}
