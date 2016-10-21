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
#include "VirtualPortal.h"

#define HID_MAX_LENGTH 32

// make an instance of arduboy used for many functions
Arduboy arduboy;
uint8_t rawhidData[HID_MAX_LENGTH];

VirtualPortal vp = VirtualPortal();
long previousMillis = 0;
long interval = 1000;

int libraryId = 0; //Token being displayed
Token *next = NULL;
unsigned int token_count = 1;

void setup() {
  Serial.begin(115200);
  RawHID.begin(rawhidData, sizeof(rawhidData));

  // initiate arduboy instance
  arduboy.beginNoLogo();

  arduboy.setFrameRate(15);
  vp.connect();
}


void loop() {
  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;

    if (next) {
      vp.loadToken(next);
      next = NULL;
    }
  }

  auto bytesAvailable = RawHID.available();
  uint8_t incoming[HID_MAX_LENGTH] = {0};
  if (bytesAvailable) {
    arduboy.print("=>");

    for (int i = 0; i < bytesAvailable; i++) {
      incoming[i] = RawHID.read();
      arduboy.print(incoming[i], HEX);
    }
    arduboy.println(".");

    uint8_t response[HID_MAX_LENGTH] = {0};
    uint8_t len = vp.respondTo(incoming, response);
    if(len) {
      arduboy.print("<=");
      for (int i = 0; i < len; i++) {
        arduboy.print(response[i], HEX);
      }
      arduboy.println(".");
      RawHID.write(response, HID_MAX_LENGTH);
    }
  }

  if (arduboy.pressed(A_BUTTON)) {
    arduboy.clear();
    arduboy.setCursor(0, 0);

  } else if (arduboy.pressed(B_BUTTON)) {
    uint8_t response[HID_MAX_LENGTH] = {0};
    uint8_t len = vp.status(response);
    if(len) {
      arduboy.print("<=");
      for (int i = 0; i < len; i++) {
        arduboy.print(response[i], HEX);
      }
      arduboy.println(".");
      RawHID.write(response, HID_MAX_LENGTH);
    }
  } else if (arduboy.pressed(UP_BUTTON)) {
    libraryId++;
  } else if (arduboy.pressed(RIGHT_BUTTON)) {
    next = new Token(libraryId);
    arduboy.clear();
    arduboy.setCursor(0, 0);
  } else if (arduboy.pressed(DOWN_BUTTON)) {
    libraryId--;
  } else if (arduboy.pressed(LEFT_BUTTON)) {
    arduboy.print(F("L"));
  }
  libraryId = positive_modulo(libraryId, token_count);

  // then we finaly we tell the arduboy to display what we just wrote to the display
  arduboy.display();
}

inline int positive_modulo(int i, int n) {
  return (i % n + n) % n;
}

