#ifndef TOKEN_H
#define TOKEN_H

#include "Klonk.h"

#if (TOKEN == MFRC522)
#include <MFRC522.h>

// RFID pins
#define SDA_PIN 2
#define RST_PIN 3

#define TOKEN_INIT MFRC522(SDA_PIN, RST_PIN)
typedef MFRC522 token_t;
#endif

struct rfid_tag {
  byte bytes[TAG_LEN];
  byte len;
};


void token_init(token_t *token_ptr);

boolean is_tag_available(token_t *rfid_ptr);

void get_tag(token_t *rfid_ptr, rfid_tag *tag_ptr);
void wait_for_tag(token_t *rfid_ptr, rfid_tag *tag_ptr);


#endif
