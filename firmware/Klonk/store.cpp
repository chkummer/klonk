#include <Arduino.h>
#include <EEPROM.h>
#include <FastCRC.h>

#include "store.h"



boolean eeprom_is_addr_ok(int addr) 
{
  return ((addr >= EEPROM.begin()) && (addr < EEPROM.end()));
}



uint32_t get_user_crc32(const UserData *user_ptr)
{
  FastCRC32 crc;

  crc.cksum(user_ptr->uid, UID_MAX_LEN);
  return crc.cksum_upd(user_ptr->pwd, PWD_MAX_LEN);
}



boolean validate_tag(RFID_TAG *tag_ptr)
{
  UserData  user;
  
  load_user(0, tag_ptr, &user);
  if (user.sum == get_user_crc32(&user))
  {
    return true;
  }

  return false;
}



void encrypt(UserData *user_ptr, const RFID_TAG *tag_ptr) 
{
  FastCRC32 crc;
  byte *data = (byte *) user_ptr;
  int len = sizeof(*user_ptr);
  byte iv[10];

  // initialize IV
  randomSeed(crc.cksum(tag_ptr->bytes, tag_ptr->len));
  for(int i = 0; i < 10; i++)
  {
    iv[i] = random(255);
  }

  for(int i = 0; i < len; i++)
  {
    byte salt = (i < tag_ptr->len) ? iv[i] : data[i - tag_ptr->len];
    data[i] = data[i] ^ tag_ptr->bytes[i % tag_ptr->len] ^ salt;
  }
}



void decrypt(UserData *user_ptr, const RFID_TAG *tag_ptr) 
{
  encrypt(user_ptr, tag_ptr);
}



void store_metadata(MetaData *meta_ptr)
{
  EEPROM.put(0, *meta_ptr);  
}



void load_metadata(MetaData *meta_ptr)
{
  EEPROM.get(0, *meta_ptr);
}



boolean load_user(int id, const RFID_TAG *tag_ptr, UserData *user_ptr)
{
  if (id < MAX_USERS)
  {
    int addr = sizeof(MetaData) + id * sizeof(UserData);

    if (eeprom_is_addr_ok(addr) && eeprom_is_addr_ok(addr + sizeof(UserData)))
    {
      EEPROM.get(addr, *user_ptr);
      decrypt(user_ptr, tag_ptr);
      return true;
    }
  }
  
  return false;
}



boolean store_user(int id, const RFID_TAG *tag_ptr, const UserData *user_ptr)
{
  if (id < MAX_USERS)
  {
    int addr = sizeof(MetaData) + id * sizeof(UserData);

    if (eeprom_is_addr_ok(addr) && eeprom_is_addr_ok(addr + sizeof(UserData)))
    {
      encrypt(user_ptr, tag_ptr);
      EEPROM.put(addr, *user_ptr);
    }
  }
}
