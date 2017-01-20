/*
   Handle serial IO
*/

#include <Arduino.h>
#include <WString.h>

#include "Klonk.h"
#include "config.h"
#include "serial_ui.h"
#include "store.h"
#include "token.h"



String read_serial_string()
{
  String str = "";

  if (Serial.available() > 0) {
    str = Serial.readString();
  }

  str.trim();
  return str;
}



String read_serial_string_len(int len)
{
  String str;

  do
  {
    str = read_serial_string();
    if (str.length() > len)
    {
      return "";
    }
  } while (str.length() == 0);


  return str;
}



byte get_user_id()
{
  byte id = 0;

  while (1)
  {
    PRINT_S("Please enter user id (1-");
    PRINT(MAX_USERS);
    PRINT_LN_S("): ");

    while (id == 0) {
      id = read_serial_string().toInt();
    }
    if (id > 0 && id <= MAX_USERS)
    {
      return id - 1;
    }
  }

  return id;
}



boolean get_password(char *pwd) {
  String pw_str;

  memset(pwd, 0, PWD_MAX_LEN + 1);
  PRINT_LN_S("Please enter new password: ");
  pw_str = read_serial_string_len(PWD_MAX_LEN);
  strncpy(pwd, pw_str.c_str(), PWD_MAX_LEN);
  pwd[PWD_MAX_LEN ] = 0;

  if (strlen(pwd) == 0)
  {
    PRINT_S("Password too long - max length are ");
    PRINT(PWD_MAX_LEN);
    PRINT_LN_S(" chars, please retry\n");

    return false;
  }

  PRINT_LN_S("Please enter new password again: ");
  pw_str = read_serial_string_len(PWD_MAX_LEN);

  if (strcmp(pwd, pw_str.c_str()) == 0)
  {
    return true;
  }
  else
  {
    PRINT_LN_S("Passwords do not match - please start over\n");
    return false;
  }
}



void set_password(int id, user_data *user_ptr, rfid_tag *tag_ptr)
{
  char pwd[PWD_MAX_LEN + 1];

  if (get_password(pwd))
  {
    debug("Got passwd");
    debug_v(pwd);
    strncpy(user_ptr->pwd, pwd, PWD_MAX_LEN);
    user_ptr->sum = get_user_crc32(user_ptr);
    store_user(id, tag_ptr, user_ptr);
  }
}



void klonk_init()
{
  meta_data  meta = {.k_state = STATE_INIT, .lang = true};
  user_data  user = {.uid = {0}, .pwd = {0}};
  rfid_tag   tag = {.bytes = {0}, .len = 0};
  String     input;

  do {
    delay(1000);
    PRINT_LN_S("Enter 'go' to start initializing");
    input = read_serial_string();
  } while (input != 'go');

  debug("Initializing Klonk");
  set_led(COLOR_BLUE);

  user.sum = get_user_crc32(&user);
  debug("SUM:");
  debug_v(user.sum);

  for (int i = 0; i < MAX_USERS; i++)
  {
    store_user(i, &tag, &user);
  }
  PRINT_LN_S("Initialized Klonk");
}



void klonk_set_lang()
{
  meta_data meta;
  String lang_str = "";

  load_metadata(&meta);
  PRINT_S("Please enter kbd language (sg/en):");

  while (lang_str.length() == 0)
  {
    lang_str = read_serial_string();
  }
  lang_str.trim();

  if (lang_str == "en")
  {
    meta.lang = true;
  }
  else if (lang_str == "sg")
  {
    meta.lang = false;
  }
  else
  {
    PRINT_LN_S("Unknown language");
    return;
  }

  PRINT_LN();
  store_metadata(&meta);
}



void klonk_chpw(int id, token_t *rfid_ptr)
{
  user_data user;
  rfid_tag tag;

  PRINT_LN_S("Reset password and RF-Tag");
  PRINT_LN_S("=========================");

  debug("User_ID");
  debug_v(id);

  wait_for_tag(rfid_ptr, &tag);

  load_user(id, &tag, &user);
  set_password(id, &user, &tag);
}



void klonk_pw_reset(int id, token_t *rfid_ptr)
{
  char      uid[UID_MAX_LEN + 1];
  user_data user;
  meta_data meta;
  rfid_tag  tag;

  PRINT_LN_S("Reset user, password and RF-Tag");
  PRINT_LN_S("===============================");

  wait_for_tag(rfid_ptr, &tag);

  do
  {
    PRINT_S("Please enter new ");
    PRINT(id + 1);
    PRINT_LN_S(" user: ");
    strncpy(uid, read_serial_string_len(UID_MAX_LEN).c_str(), UID_MAX_LEN);
  } while (strlen(uid) == 0);
  strncpy(user.uid, uid, UID_MAX_LEN);

  set_password(id, &user, &tag);
  load_metadata(&meta);
  meta.k_state = STATE_ACTV;
  store_metadata(&meta);
}



void handle_serial_input(boolean lock, token_t *rfid_ptr, rfid_tag *tag_ptr)
{
  String command = read_serial_string();
  command.trim();
  debug("Got command:");
  debug_v(command);
  debug("Command length:");
  debug_v(command.length());

  if (command == "passwd")
  {
    debug("PASSWD");

    PRINT_LN_S("Changing existing password");
    PRINT_LN_S("==========================");

    klonk_chpw(get_user_id(), rfid_ptr);
  }
  else if (command == "pwgen")
  {
    debug("PWGEN");
  }
  else if (command == "reset")
  {
    debug("RESET");

    klonk_pw_reset(get_user_id(), rfid_ptr);
  }
  else if (command == "init")
  {
    debug("INIT");

    klonk_init();
    klonk_pw_reset(0, rfid_ptr);
  }
  else if (command == "lang")
  {
    klonk_set_lang();
  }
  else if (command == "help")
  {
    debug("HELP");
    PRINT_LN_S("help        List of commands");
    PRINT_LN_S("passwd      Change password");
    PRINT_LN_S("pwgen       Generate random password");
    PRINT_LN_S("reset       Reset user, password and tag");
    PRINT_LN_S("lang        Set keyboard language");
    PRINT_LN_S("init        Initialize Klonk");
    PRINT_LN_S("");
    delay(100);
  } else {

  }
}
