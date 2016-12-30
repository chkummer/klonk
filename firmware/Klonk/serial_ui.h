#ifndef SERIAL_UI_H
#define SERIAL_UI_H 1

#include "token.h"

String readSerialString();
String readSerialStringLen(int len);

void klonk_init();
void klonk_pw_reset(int id, token_t *rfid_ptr);

void handle_serial_input(boolean LOCK, token_t *rfid_ptr, RFID_TAG *tag_ptr);

#endif
