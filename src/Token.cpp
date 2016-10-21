
#include "Token.h"
#include "kingpen.h"

Token::Token(int libraryId) : libraryId(libraryId) {
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
  Serial.print(F("readFlash ")); Serial.print(chapter); Serial.print(","); Serial.print(page_offset); Serial.print(","); Serial.println(block_offset);

  //dflash.Page_Read_Str(chapter + page_offset, block_offset, BLOCK_SIZE, buffer);
}

void Token::writeFlash(int block, uint8_t* buffer) {
  int chapter = TOC_SIZE + (libraryId * CHAPTER_SIZE);
  int page_offset = block / BLOCKS_PER_PAGE; //Which page in chapter [0,3]
  int block_offset = (block % BLOCKS_PER_PAGE) * BLOCK_SIZE;
  uint8_t dflash_buffer = 0;

  switch(type()) {
    case TRAP_MASTER:
    case MINI:
    case REGULAR:
      dflash_buffer = PRIMARY_BUFFER;
      break;
    case TRAP:
      dflash_buffer = SECONDARY_BUFFER;
      break;
    default:
      return;
  }

  /*
  dflash.Page_To_Buffer(chapter + page_offset, dflash_buffer);
  dflash.Buffer_Write_Str(dflash_buffer, block_offset, BLOCK_SIZE, buffer);
  dflash.Buffer_To_Page(dflash_buffer, chapter + page_offset);
  */
}

void Token::display() {
  int page_offset = libraryId / BLOCKS_PER_PAGE;
  int block_offset = (libraryId % BLOCKS_PER_PAGE) * BLOCK_SIZE;
  //Serial.print("TOC ");  Serial.print(page_offset); Serial.print(","); Serial.println(block_offset);
  uint8_t name_len = BLOCK_SIZE - 1;
  //dflash.Page_Read_Str(page_offset, block_offset, name_len, (uint8_t*)name);
  //dflash.Page_Read_Str(page_offset, block_offset + name_len, 1, &elementAndType);

  //Topline Character name
  LCD.print(name);
  Serial.write(name, min(strlen(name), BLOCK_SIZE - 1));
  Serial.println(" ");

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

uint16_t Token::typeId() {
  uint8_t buffer[BLOCK_SIZE];
  readFlash(1, buffer);
  return (buffer[0] | (buffer[1] << sizeof(uint8_t)));
}

char* Token::getName() {
  int page_offset = libraryId / BLOCKS_PER_PAGE;
  int block_offset = (libraryId % BLOCKS_PER_PAGE) * BLOCK_SIZE;
  uint8_t name_len = BLOCK_SIZE - 1;
  //dflash.Page_Read_Str(page_offset, block_offset, name_len, (uint8_t*)name);
  //dflash.Page_Read_Str(page_offset, block_offset + name_len, 1, &elementAndType);

  return name;
}

uint8_t Token::element() {
  return (elementAndType & ELEMENT_MASK);
}

