#ifndef SERIAL_UI_H
#define SERIAL_UI_H 1

#include "config.h"
#include "token.h"

#if (DEBUG > 0)
#define debug(msg) Serial.println(msg)
#else
#define debug(msg)
#endif

String readSerialString();
String readSerialStringLen(int len);

void klonk_init();
void klonk_pw_reset(int id, token_t *rfid_ptr);

void handle_serial_input(boolean LOCK, token_t *rfid_ptr, rfid_tag *tag_ptr);

#endif
