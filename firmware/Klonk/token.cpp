#include "token.h"


#if (TOKEN == MFRC522)
  void token_init(token_t *token_ptr)
  {
    *token_ptr = MFRC522(SDA_PIN, RST_PIN);
    token_ptr->PCD_Init();
  }



  void get_tag(token_t *rfid_ptr, RFID_TAG *tag_ptr)
  {
    tag_ptr->len = rfid_ptr->uid.size;
    strncpy(tag_ptr->bytes, rfid_ptr->uid.uidByte, TAG_LEN);
  }
  
  
  
  void wait_for_tag(token_t *rfid_ptr, RFID_TAG *tag_ptr)
  {
    set_led(COLOR_BLUE);
  
    Serial.println("Please register tag:");
  
    while (! is_tag_available(rfid_ptr))
    {
      delay(100);
    }
    get_tag(rfid_ptr, tag_ptr);
    Serial.println("Got Tag");
  
    set_led(COLOR_GREEN);
  }



  boolean is_tag_available(token_t *rfid_ptr)
  {
    return (rfid_ptr->PICC_IsNewCardPresent() && rfid_ptr->PICC_ReadCardSerial());
  }
#endif




