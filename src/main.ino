#include "Arduboy.h"
#include "RawHID.h"
#include "VirtualPortal.h"

#define HID_MAX_LENGTH 32

#define LINE_HEIGHT 8

#define LINE_INCOMING 0 * LINE_HEIGHT
#define LINE_OUTGOING 1 * LINE_HEIGHT
#define LINE_LOADS 2 * LINE_HEIGHT

// make an instance of arduboy used for many functions
Arduboy arduboy;
uint8_t rawhidData[HID_MAX_LENGTH];

VirtualPortal vp = VirtualPortal();
long previousMillis = 0;
long interval = 1000;

int libraryId = 0; //Token being displayed
Token *next = NULL;
unsigned int token_count = 1;

int blockNum = 0;

void setup() {
  RawHID.begin(rawhidData, sizeof(rawhidData));

  // initiate arduboy instance
  arduboy.beginNoLogo();

  arduboy.setFrameRate(15);
  arduboy.setTextWrap(true);
  vp.connect();
}

void loop() {
  unsigned long currentMillis = millis();

  auto bytesAvailable = RawHID.available();
  uint8_t incoming[HID_MAX_LENGTH] = {0};
  if (bytesAvailable) {
    arduboy.setCursor(0, LINE_INCOMING);
    arduboy.print("=>");
    for (int i = 0; i < bytesAvailable; i++) {
      incoming[i] = RawHID.read();
    }
    arduboy.write((char)incoming[0]);
    printHex(bytesAvailable);

    uint8_t response[HID_MAX_LENGTH] = {0};
    uint8_t len = vp.respondTo(incoming, response);
    if(len) {
      arduboy.setCursor(0, LINE_OUTGOING);
      arduboy.print("<=");
      arduboy.write((char)response[0]);
      for (int i = 1; i < 6; i++) {
        printHex(response[i]);
      }
      RawHID.write(response, HID_MAX_LENGTH);
    }
  }

  if (arduboy.pressed(A_BUTTON)) {
    arduboy.clear();
  } else if (arduboy.pressed(B_BUTTON)) {
    if(next == NULL) {
      next = new Token(0);
      arduboy.setCursor(0, LINE_LOADS);
      arduboy.print("Loaded ");
      arduboy.print(next->getName());
    }
  } else if (arduboy.pressed(UP_BUTTON)) {
  } else if (arduboy.pressed(RIGHT_BUTTON)) {
  } else if (arduboy.pressed(DOWN_BUTTON)) {
  } else if (arduboy.pressed(LEFT_BUTTON)) {
  }
  libraryId = positive_modulo(libraryId, token_count);

  if(currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;

    if (next) {
      vp.loadToken(next);
      next = NULL;
    }

    //Only send proactive status update when we didn't respond to a message
    if (bytesAvailable == 0 && vp.sendStatus()) {
      uint8_t response[HID_MAX_LENGTH] = {0};
      uint8_t len = vp.status(response);
      if(len) {
        arduboy.setCursor(0, LINE_OUTGOING);
        arduboy.print("<-");
        arduboy.write((char)response[0]);
        for (int i = 1; i < len; i++) {
          printHex(response[i]);
        }
        RawHID.write(response, HID_MAX_LENGTH);
      }
    }

  }

  arduboy.display();
}
void printHex(int num) {
  char tmp[3];
  sprintf(tmp, "%02x", num);
  arduboy.print(tmp);
}

inline int positive_modulo(int i, int n) {
  return (i % n + n) % n;
}
