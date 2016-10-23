
#include "Token.h"

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
  int start = block * BLOCK_SIZE;
  for(int i = 0; i < BLOCK_SIZE; i++) {
    buffer[i] = EEPROM.read(start+i);
  }
}

void Token::writeFlash(int block, uint8_t* buffer) {
  int chapter = TOC_SIZE + (libraryId * CHAPTER_SIZE);
  int page_offset = block / BLOCKS_PER_PAGE; //Which page in chapter [0,3]
  int block_offset = (block % BLOCKS_PER_PAGE) * BLOCK_SIZE;
  int start = block * BLOCK_SIZE;
  for(int i = 0; i < BLOCK_SIZE; i++) {
    EEPROM.update(start+i, buffer[i]);
  }

}

void Token::display() {
  int page_offset = libraryId / BLOCKS_PER_PAGE;
  int block_offset = (libraryId % BLOCKS_PER_PAGE) * BLOCK_SIZE;
  uint8_t name_len = BLOCK_SIZE - 1;

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
