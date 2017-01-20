#include "serial_ui.h"
#include "token.h"



#if (TOKEN == MFRC522)
void token_init(token_t *token_ptr)
{
  *token_ptr = MFRC522(SDA_PIN, RST_PIN);
  token_ptr->PCD_Init();
}



void get_tag(token_t *rfid_ptr, rfid_tag *tag_ptr)
{
  tag_ptr->len = rfid_ptr->uid.size;
  strncpy(tag_ptr->bytes, rfid_ptr->uid.uidByte, TAG_LEN);
}

void wait_for_tag(token_t *rfid_ptr, rfid_tag *tag_ptr)
{
  set_led(COLOR_BLUE);

  PRINT_LN_S("Please register tag:");
  while (! is_tag_available(rfid_ptr))
  {
    delay(100);
  }
  get_tag(rfid_ptr, tag_ptr);

  PRINT_LN_S("Got Tag");
  for (int i = tag_ptr->len - 1; i > 0; i--)
  {
    PRINT_HEX(tag_ptr->bytes[i]);
  }
  debug("END TAG");

  set_led(COLOR_GREEN);
}



boolean is_tag_available(token_t *rfid_ptr)
{
  return (rfid_ptr->PICC_IsNewCardPresent() && rfid_ptr->PICC_ReadCardSerial());
}
#endif




