#include <Arduino.h>
#include <EEPROM.h>
#include <FastCRC.h>

#include "serial_ui.h"
#include "store.h"



boolean eeprom_is_addr_ok(int addr)
{
  return ((addr >= EEPROM.begin()) && (addr < EEPROM.end()));
}



uint32_t get_user_crc32(const user_data *user_ptr)
{
  FastCRC32 crc;

  crc.cksum(user_ptr->uid, UID_MAX_LEN);
  return crc.cksum_upd(user_ptr->pwd, PWD_MAX_LEN);
}



boolean validate_tag(rfid_tag *tag_ptr)
{
  user_data  user;

  debug_v(user.uid);
  debug_v(user.pwd);

  load_user(0, tag_ptr, &user);
  if (user.sum == get_user_crc32(&user))
  {
    return true;
  }

  return false;
}



void get_iv(const rfid_tag *tag_ptr, byte *iv_ptr)
{
  FastCRC32 crc;

  randomSeed(crc.cksum(tag_ptr->bytes, tag_ptr->len));
  for (int i = 0; i < 10; i++)
  {
    iv_ptr[i] = random(255);
  }
}



void encrypt(user_data *user_ptr, const rfid_tag *tag_ptr)
{
  byte *data = (byte *) user_ptr;
  byte iv[TAG_LEN];

  get_iv(tag_ptr, iv);

  for (int i = 0; i < sizeof(*user_ptr); i++)
  {
    byte salt = (i < tag_ptr->len) ? iv[i] : data[i - tag_ptr->len];
    data[i] = data[i] ^ tag_ptr->bytes[i % tag_ptr->len] ^ salt;
  }
}



void decrypt(user_data *user_ptr, const rfid_tag *tag_ptr)
{
  byte *data = (byte *) user_ptr;
  byte iv[TAG_LEN];
  byte cipher_blk[TAG_LEN];

  get_iv(tag_ptr, iv);

  for (int i = 0; i < sizeof(*user_ptr); i++)
  {
    byte salt = (i < tag_ptr->len) ? iv[i] : cipher_blk[i % tag_ptr->len];
    cipher_blk[i % tag_ptr->len] = data[i];
    data[i] = data[i] ^ tag_ptr->bytes[i % tag_ptr->len] ^ salt;
  }
}



void store_metadata(const meta_data *meta_ptr)
{
  EEPROM.put(0, *meta_ptr);
}



void load_metadata(meta_data *meta_ptr)
{
  EEPROM.get(0, *meta_ptr);
}



boolean load_user(int id, const rfid_tag *tag_ptr, user_data *user_ptr)
{
  if (id < MAX_USERS)
  {
    int addr = sizeof(meta_data) + id * sizeof(user_data);

    if (eeprom_is_addr_ok(addr) && eeprom_is_addr_ok(addr + sizeof(user_data)))
    {
      EEPROM.get(addr, *user_ptr);
      decrypt(user_ptr, tag_ptr);
      return true;
    }
  }

  return false;
}



boolean store_user(int id, const rfid_tag *tag_ptr, const user_data *user_ptr)
{
  if (id < MAX_USERS)
  {
    int addr = sizeof(meta_data) + id * sizeof(user_data);

    if (eeprom_is_addr_ok(addr) && eeprom_is_addr_ok(addr + sizeof(user_data)))
    {
      encrypt(user_ptr, tag_ptr);
      EEPROM.put(addr, *user_ptr);
    }
  }
}
