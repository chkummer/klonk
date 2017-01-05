#ifndef STORE_H
#define STORE_H 1

  #include "config.h"
  #include "Klonk.h"
  
  #define BUF_SIZE 25
  #define STATE_INIT 0x4242
  #define STATE_ACTV 0xBDBD

  
  struct user_data {
    char      uid[UID_MAX_LEN + 1];
    char      pwd[PWD_MAX_LEN + 1];
    uint32_t  sum;
  };

  struct meta_data {
    uint16_t  k_state;
    byte      lang;
  };

  boolean eeprom_is_addr_ok(int addr);
  uint32_t get_user_crc32(const user_data *user_ptr);

  boolean validate_tag(rfid_tag *tag);

  void load_metadata(meta_data *meta_ptr);
  void store_metadata(const meta_data *meta_ptr);

  boolean load_user(int id, const rfid_tag *tag_ptr, user_data *user_ptr);
  boolean store_user(int id, const rfid_tag *tag_ptr, const user_data *user_ptr);

#endif
