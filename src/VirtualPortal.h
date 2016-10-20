#ifndef VirtualPortal_h
#define VirtualPortal_h

#include "Arduino.h"
#include "Token.h"
#include "Arduboy.h"
extern Arduboy arduboy;

#define BLE_ATTRIBUTE_MAX_VALUE_LENGTH 20
#define LCD arduboy

class VirtualPortal {
  public:
    VirtualPortal();

    int respondTo(uint8_t *message, uint8_t *response);
    bool loadToken(Token *t);
    uint8_t light(); //get current light value
    void connect();
    void disconnect();
    void subscribe();
    void unsubscribe();
    bool removeType(uint8_t);

  private:
    int sequence;
    uint8_t lightVal;
    Token *characterToken, *trapToken, *locationToken, *itemToken;

    int activate(uint8_t* message, uint8_t* response);
    int color(uint8_t* message);
    int jay(uint8_t* response);
    int light(uint8_t* message);
    int query(uint8_t* message, uint8_t* response);
    int reset(uint8_t* response);
    int status(uint8_t* response);
    int write(uint8_t* message, uint8_t* response);

    void printCommand(bool incoming, const uint8_t* command);
};

#endif
