#ifndef Token_H
#define Token_H

#include "Arduino.h"
#include <EEPROM.h>

#define BLOCK_SIZE 16
#define BLOCK_COUNT 64

#define PAGE_SIZE 256 //bytes
#define PAGE_COUNT 2048 //4Mbit chip

#define BLOCKS_PER_PAGE 16

#define TOC_CHAPTER 0
#define TOC_SIZE 32 //Enough pages to store 512 tokens
#define CHAPTER_SIZE 4 //pages in a chapter/token

#define TYPE_MASK 0xF0
//All greater than 0x0F so they can be stored in high order bits
enum Type {
  //NONE = 0x00,
  TRAP_MASTER = 0x10,
  TRAP = 0x20,
  MAGIC_ITEM = 0x30,
  LOCATION = 0x40,
  MINI = 0x50,
  REGULAR = 0x60,
  GIANT = 0x70,
  LEGEND = 0x80,
};

#define ELEMENT_MASK 0x0F
enum Element { NONE, MAGIC, EARTH, WATER, FIRE, TECH, UNDEAD, LIFE, AIR, DARK, LIGHT};

class Token {
  public:
    Token(int libraryId);
    int read(int block, uint8_t* buffer);
    int write(int block, uint8_t* data);
    void display();
    uint8_t type();
    uint8_t element();
    uint16_t typeId();
    char* getName();

  private:
    int libraryId;
    char name[BLOCK_SIZE];
    uint8_t elementAndType; //enums
    void readFlash(int block, uint8_t* buffer);
    void writeFlash(int block, uint8_t* buffer);
};


#endif
