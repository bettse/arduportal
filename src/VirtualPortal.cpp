#include "VirtualPortal.h"

VirtualPortal::VirtualPortal() : lightVal(0), sequence(0) {
    characterToken = NULL;
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

    switch(arrayIndex) {
      case 0:
        characterToken->read(block, response+3);
        break;
      case 4:
        trapToken->read(block, response+3);
        break;
      case 8:
        locationToken->read(block, response+3);
        break;
      case 12:
        itemToken->read(block, response+3);
        break;
    }

    return BLE_ATTRIBUTE_MAX_VALUE_LENGTH;
}

int VirtualPortal::write(uint8_t* message, uint8_t* response) {
    int index = message[1];
    int block = message[2];
    int arrayIndex = index & 0x0f;

    switch(arrayIndex) {
      case 0:
        characterToken->write(block, message+3);
        break;
      case 4:
        trapToken->write(block, message+3);
        break;
      case 8:
        locationToken->write(block, message+3);
        break;
      case 12:
        itemToken->write(block, message+3);
        break;
    }

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
  response[3] = 0;
  response[4] = 0;
  response[5] = sequence++ % 0xFF;
  response[6] = 0;
  response[7] = 0xaa;
  response[8] = 0xc2;
  response[9] = 0x02;
  response[10] = 0x19;

  return 11;
}

int VirtualPortal::activate(uint8_t* message, uint8_t* response) {
  response[0] = message[0];
  response[1] = message[1];
  response[2] = 0x62;
  response[3] = 0x02;
  response[4] = 0x19;
  response[5] = 0xaa;
  response[6] = 0x01;
  response[7] = 0x53;
  response[8] = 0xbc;
  response[9] = 0x58;
  response[10] = 0xfc;
  response[11] = 0x7d;
  response[12] = 0xf4;
  return 13;
}

int VirtualPortal::status(uint8_t* response) {
  response[0] = 'S';
  response[1] = characterToken ? 0x01 : 0x00;
  response[2] = trapToken ? 0x01 : 0x00;
  response[3] = locationToken ? 0x01 : 0x00;
  response[4] = itemToken ? 0x01 : 0x00;

  response[5] = sequence++ % 0xFF;

  response[6] = 1;
  response[7] = 0xaa;
  response[8] = 0xc2;
  response[9] = 0x02;
  response[10] = 0x19;

  return BLE_ATTRIBUTE_MAX_VALUE_LENGTH;
}

bool VirtualPortal::loadToken(Token *t) {
  Serial.print("Load ");
  switch(t->type()) {
    case TRAP:
      trapToken = t;
      break;
    case MAGIC_ITEM:
      itemToken = t;
      break;
    case LOCATION:
      locationToken = t;
      break;
    case TRAP_MASTER:
    case MINI:
    case REGULAR:
      characterToken = t;
      break;
  }
  Serial.println(t->getName());
  return true;
}

bool VirtualPortal::removeType(uint8_t type) {
  //Serial.print("Removing token type "); Serial.println(type, HEX);
  switch(type) {
    case TRAP:
      if (trapToken) {
        Serial.println("Unload trap");
        delete trapToken;
        trapToken = NULL;
      }
      break;
    case MAGIC_ITEM:
      if (itemToken) {
        Serial.println("Unload item");
        delete itemToken;
        itemToken = NULL;
      }
      break;
    case LOCATION:
      if (locationToken) {
        Serial.println("Unload location");
        delete locationToken;
        locationToken = NULL;
      }
      break;
    case TRAP_MASTER:
    case MINI:
    case REGULAR:
      if (characterToken) {
        Serial.println("Unload character");
        delete characterToken;
        characterToken = NULL;
      }
      break;
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

void VirtualPortal::subscribe() {
  sequence = 0;
}

void VirtualPortal::unsubscribe() {
}
