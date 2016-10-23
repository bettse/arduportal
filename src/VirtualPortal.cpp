#include "VirtualPortal.h"
#include "Arduboy.h"
extern Arduboy arduboy;

VirtualPortal::VirtualPortal() : lightVal(0), sequence(0), characterToken(NULL), characterLoaded(false), proactiveStatus(false) {
}

int VirtualPortal::respondTo(uint8_t* message, uint8_t* response) {
  switch(message[0]) {
    case 'A':
      return activate(message, response);
    case 'C': //Ring color R G B
      return 0; //No response
    case 'J':
      return 0; //No response
    case 'L': //Trap light
      light(message);
      return 0; //No response
    case 'Q': //Query / read
      return query(message, response);
    case 'R':
      return reset(response);
    case 'S': //Status
      return status(response);
    case 'W': //Write
      return write(message, response);
  }
  return 0;
}

int VirtualPortal::query(uint8_t* message, uint8_t* response) {
  int index = message[1];
  int block = message[2];
  int arrayIndex = index & 0x0f;

  response[0] = 'Q';
  response[1] = index;
  response[2] = block;

  characterToken->read(block, response+3);
  return 3 + BLOCK_SIZE;
}

int VirtualPortal::write(uint8_t* message, uint8_t* response) {
  int index = message[1];
  int block = message[2];
  int arrayIndex = index & 0x0f;

  characterToken->write(block, message+3);

  //Status update with different first 3 bytes
  status(response);
  response[0] = 'W';
  response[1] = index;
  response[2] = block;

  return 3;
}

int VirtualPortal::reset(uint8_t* response) {
  response[0] = 'R';
  response[1] = 0x02;
  response[2] = 0x19;
  return 3;
}

int VirtualPortal::activate(uint8_t* message, uint8_t* response) {
  proactiveStatus = (message[1] == 1);
  response[0] = message[0];
  response[1] = message[1];
  response[2] = 0xFF;
  response[3] = 0x77;
  return 4;
}

int VirtualPortal::status(uint8_t* response) {
  response[0] = 'S';
  response[1] = (characterToken ? 0x01 : 0x00) | (characterLoaded ? 0x02 : 0x00);
  response[2] = 0;
  response[3] = 0;
  response[4] = 0;
  response[5] = sequence++ % 0xFF;
  response[6] = 1;

  characterLoaded = false;
  return 7;
}

bool VirtualPortal::loadToken(Token *t) {
  characterLoaded = true;
  characterToken = t;
  return true;
}

bool VirtualPortal::removeType(uint8_t type) {
  if (characterToken) {
    delete characterToken;
    characterToken = NULL;
    characterLoaded = true;
  }
  return true;
}

int VirtualPortal::light(uint8_t* message) {
  lightVal = message[1];
  return lightVal;
}

uint8_t VirtualPortal::light() {
  return lightVal * 0x80;
}

void VirtualPortal::connect() {
}

void VirtualPortal::disconnect() {
}

bool VirtualPortal::sendStatus() {
  return proactiveStatus;
}

void VirtualPortal::subscribe() {
  sequence = 0;
}

void VirtualPortal::unsubscribe() {
}
