#ifndef STORE_H
#define STORE_H 1

  #include "config.h"
  #include "Klonk.h"
  
  #define BUF_SIZE 25
  #define STATE_INIT 0x4242
  #define STATE_ACTV 0xBDBD

  
  struct UserData {
    char      uid[UID_MAX_LEN + 1];
    char      pwd[PWD_MAX_LEN + 1];
    uint32_t  sum;
  };

  struct MetaData {
    uint16_t  k_state;
    byte      lang;
  };

  boolean eeprom_is_addr_ok(int addr);
  uint32_t get_user_crc32(const UserData *user_ptr);

  boolean validate_tag(RFID_TAG *tag);

  void load_metadata(MetaData *meta_ptr);
  void store_metadata(const MetaData *meta_ptr);

  boolean load_user(int id, const RFID_TAG *tag_ptr, UserData *user_ptr);
  boolean store_user(int id, const RFID_TAG *tag_ptr, const UserData *user_ptr);

#endif
