#include "VirtualPortal.h"
#include "Arduboy.h"
extern Arduboy arduboy;

VirtualPortal::VirtualPortal() : lightVal(0), sequence(0), characterToken(NULL), characterLoaded(false), proactiveStatus(false) {
}

int VirtualPortal::respondTo(uint8_t* message, uint8_t* response) {
  printCommand(true, message);
  switch(message[0]) {
    case 'A':
      activate(message, response);
      break;
    case 'C': //Ring color R G B
      return 0; //No response
      break;
    case 'J':
      return 0; //No response
      break;
    case 'L': //Trap light
      light(message);
      return 0; //No response
      break;
    case 'Q': //Query / read
      query(message, response);
      break;
    case 'R':
      reset(response);
      break;
    case 'S': //Status
      status(response);
      break;
    case 'W': //Write
      write(message, response);
      break;
  }
  printCommand(false, response);
  return BLE_ATTRIBUTE_MAX_VALUE_LENGTH;
}

int VirtualPortal::query(uint8_t* message, uint8_t* response) {
  int index = message[1];
  int block = message[2];
  int arrayIndex = index & 0x0f;

  response[0] = 'Q';
  response[1] = index;
  response[2] = block;

  characterToken->read(block, response+3);
  return BLE_ATTRIBUTE_MAX_VALUE_LENGTH;
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

  return BLE_ATTRIBUTE_MAX_VALUE_LENGTH;
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

void VirtualPortal::printCommand(bool incoming, const uint8_t* command) {
  int interestingBytes = 0;

  LCD.write(LCD_MOVE); //MoveTo
  LCD.write(LCD_TOP + 0x0F); //top + Last character
  LCD.write(command[0]);

  switch(command[0]) {
    case 'C':
      //interestingBytes = 3;
      break;
    case 'L': //Trap light
      //interestingBytes = 1;
      break;
    case 'R':
      //interestingBytes = incoming ? 1 : 12;
      break;
    case 'A':
      //interestingBytes = 1;
      break;
    case 'S':
      //interestingBytes = incoming ? 0 : 5;
      break;
    case 'Q': //Query / read
    case 'W': //Write
      if (command[2] == 1) {
        interestingBytes = incoming ? 2 : 18;
      }
      break;
  }

  if (interestingBytes > 0) {
    Serial.print(incoming ? "<= " : "=> ");
    Serial.print((char)command[0]);
    Serial.print(F(" "));

    for(int i = 0; i < interestingBytes; i++) {
      Serial.print(command[i+1], HEX); //+1 to ignore ASCII first byte
      Serial.print(F(" "));
    }
    if (incoming) {
      Serial.print(F(" "));
    } else {
      Serial.println(F(" "));
    }
  }
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
