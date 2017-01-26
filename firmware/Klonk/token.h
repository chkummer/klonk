#ifndef TOKEN_H
#define TOKEN_H

#include "config.h"
#include "Klonk.h"

#if (RFID_MFRC522 > 0)
#include <SPI.h>
#include <MFRC522.h>

// RFID pins
#define SDA_PIN 2
#define RST_PIN 3

#define TOKEN_INIT MFRC522(SDA_PIN, RST_PIN)

typedef MFRC522 token_t;
#endif


#if (RFID_PN532 > 0)
#include <Adafruit_PN532.h>

#define PN532_SCK   15
#define PN532_MOSI  16
#define PN532_SS     2
#define PN532_MISO  14
#define PN532_IRQ    2
#define PN532_RESET  3

#define TOKEN_INIT Adafruit_PN532(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS)

typedef Adafruit_PN532 token_t;
#endif


struct rfid_tag {
  uint8_t bytes[TAG_LEN];
  uint8_t len;
};


void token_init(token_t *token_ptr);

boolean is_tag_available(token_t *rfid_ptr);

void get_tag(token_t *rfid_ptr, rfid_tag *tag_ptr);
void wait_for_tag(token_t *rfid_ptr, rfid_tag *tag_ptr);


#endif
