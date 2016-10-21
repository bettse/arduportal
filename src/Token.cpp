
#include "Token.h"
#include "kingpen.h"

Token::Token(int libraryId) : libraryId(libraryId), elementAndType(0x63) {
  strcpy(name, "Kingpen");
}

int Token::read(int block, uint8_t* buffer) {
  if (block > BLOCK_COUNT) {
    return 0;
  }
  readFlash(block, buffer);
  return BLOCK_SIZE;
}

int Token::write(int block, uint8_t* buffer) {
  if (block > BLOCK_COUNT) {
    return 0;
  }
  writeFlash(block, buffer);
  return BLOCK_SIZE;
}

void Token::readFlash(int block, uint8_t* buffer) {
  int chapter = TOC_SIZE + (libraryId * CHAPTER_SIZE);
  int page_offset = block / BLOCKS_PER_PAGE; //Which page in chapter [0,3]
  int block_offset = (block % BLOCKS_PER_PAGE) * BLOCK_SIZE;
  //memcpy(buffer, tokens_masterkingpen_bin + BLOCK_SIZE * block, BLOCK_SIZE);
}

void Token::writeFlash(int block, uint8_t* buffer) {
  int chapter = TOC_SIZE + (libraryId * CHAPTER_SIZE);
  int page_offset = block / BLOCKS_PER_PAGE; //Which page in chapter [0,3]
  int block_offset = (block % BLOCKS_PER_PAGE) * BLOCK_SIZE;

}

void Token::display() {
  int page_offset = libraryId / BLOCKS_PER_PAGE;
  int block_offset = (libraryId % BLOCKS_PER_PAGE) * BLOCK_SIZE;
  uint8_t name_len = BLOCK_SIZE - 1;

  //Topline Character name
  LCD.print(name);

  switch(element()) {
    case NONE:
      LCD.print("");
    case MAGIC:
      LCD.print("Magic");
      break;
    case EARTH:
      LCD.print("Earth");
      break;
    case WATER:
      LCD.print("Water");
      break;
    case FIRE:
      LCD.print("Fire");
      break;
    case TECH:
      LCD.print("Tech");
      break;
    case UNDEAD:
      LCD.print("Undead");
      break;
    case LIFE:
      LCD.print("Life");
      break;
    case AIR:
      LCD.print("Air");
      break;
    case DARK:
      LCD.print("Dark");
      break;
    case LIGHT:
      LCD.print("Light");
      break;
    default:
      LCD.print(element());
      break;
  }

  switch(type()) {
    case TRAP_MASTER:
      LCD.print("T.Master");
      break;
    case TRAP:
      LCD.print("Trap");
      break;
    case MAGIC_ITEM:
      LCD.print("Item");
      break;
    case LOCATION:
      LCD.print("Location");
      break;
    case MINI:
      LCD.print("Mini");
      break;
    case REGULAR:
      LCD.print("Regular");
      break;
    case GIANT:
      LCD.print("Giant");
     break;
    default:
      LCD.print(type());
      break;
  }

}

uint8_t Token::type() {
  return (elementAndType & TYPE_MASK);
}
uint8_t Token::element() {
  return (elementAndType & ELEMENT_MASK);
}

uint16_t Token::typeId() {
  uint8_t buffer[BLOCK_SIZE];
  readFlash(1, buffer);
  return (buffer[0] | (buffer[1] << sizeof(uint8_t)));
}

char* Token::getName() {
  int page_offset = libraryId / BLOCKS_PER_PAGE;
  int block_offset = (libraryId % BLOCKS_PER_PAGE) * BLOCK_SIZE;
  uint8_t name_len = BLOCK_SIZE - 1;

  return name;
}



