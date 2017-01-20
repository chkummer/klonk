#ifndef SERIAL_UI_H
#define SERIAL_UI_H 1

#include "config.h"
#include "token.h"



#define PRINT(val)      Serial.print(val)
#define PRINT_HEX(val)  Serial.print(val, HEX)
#define PRINT_LN(val)   Serial.println(val)
#define PRINT_S(str)    Serial.print(F(str))
#define PRINT_LN_S(str) Serial.println(F(str))



#if (DEBUG > 0)
#define debug(msg)   PRINT_LN_S(msg)
#define debug_v(val) PRINT_LN(val)
#else
#define debug(msg)
#define debug_v(msg)
#endif



String readSerialString();
String readSerialStringLen(int len);

void klonk_init();
void klonk_pw_reset(int id, token_t *rfid_ptr);

void handle_serial_input(boolean LOCK, token_t *rfid_ptr, rfid_tag *tag_ptr);

#endif
