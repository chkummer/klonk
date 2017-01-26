#include "serial_ui.h"
#include "token.h"



/*
 *  Code for MFRC522
 */

#if (RFID_MFRC522 > 0)
void token_init(token_t *token_ptr)
{
  SPI.begin();

  *token_ptr = MFRC522(SDA_PIN, RST_PIN);
  token_ptr->PCD_Init();
}



boolean get_tag(token_t *rfid_ptr, rfid_tag *tag_ptr)
{
  if (rfid_ptr->PICC_IsNewCardPresent() && rfid_ptr->PICC_ReadCardSerial())
  {
    tag_ptr->len = rfid_ptr->uid.size;
    strncpy(tag_ptr->bytes, rfid_ptr->uid.uidByte, TAG_LEN);

    return true;
  }

  return false;
}
#endif //(TOKEN MFRC522)



/*
 *  Code for PN532
 */

#if (RFID_PN532 > 0)
void token_init(token_t *token_ptr)
{
  token_ptr->begin();

  uint32_t versiondata = token_ptr->getFirmwareVersion();
  if (! versiondata) {
    PRINT_LN_S("PN53x not found");
    while (1); // halt
  }
  token_ptr->SAMConfig();
}



boolean get_tag(token_t *rfid_ptr, rfid_tag *tag_ptr)
{
  uint8_t tag_bytes[TAG_LEN];
  uint8_t tag_len;

  if (rfid_ptr->readPassiveTargetID(PN532_MIFARE_ISO14443A, tag_bytes, &tag_len, 100))
  {
    tag_ptr->len = tag_len;
    strncpy(tag_ptr->bytes, tag_bytes, TAG_LEN);

    return true;
  }

  return false;
}
#endif //(TOKEN PN532)



/*
 * Generic functions
 */

void wait_for_tag(token_t *rfid_ptr, rfid_tag *tag_ptr)
{
  set_led(COLOR_BLUE);

  PRINT_LN_S("Please register tag:");
  while (! get_tag(rfid_ptr, tag_ptr))
  {
    delay(100);
  }

  PRINT_LN_S("Got Tag");
  for (int i = tag_ptr->len - 1; i > 0; i--)
  {
    PRINT_HEX(tag_ptr->bytes[i]);
  }
  debug("END TAG");

  set_led(COLOR_GREEN);
}

