/*
   Handle serial IO
*/

#include <Arduino.h>
#include <Keyboard.h>
#include <WString.h>

#include "Klonk.h"
#include "config.h"
#include "serial_ui.h"
#include "store.h"
#include "token.h"



String read_serial_string()
{
  if (Serial.available() > 0) {
    return Serial.readString();
  }

  return "";
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
    Serial.print("Please enter user id (1-");
    Serial.print(MAX_USERS);
    Serial.println("): ");

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
  Serial.println("Please enter new password: ");
  pw_str = read_serial_string_len(PWD_MAX_LEN);
  strncpy(pwd, pw_str.c_str(), PWD_MAX_LEN);
  pwd[PWD_MAX_LEN ] = 0;

  if (strlen(pwd) == 0)
  {
    Serial.print("Password too long - max length are ");
    Serial.print(PWD_MAX_LEN);
    Serial.println(" chars, please retry\n");

    return false;
  }

  Serial.println("Please enter new password again: ");
  pw_str = read_serial_string_len(PWD_MAX_LEN);

  if (strcmp(pwd, pw_str.c_str()) == 0)
  {
    return true;
  }
  else
  {
    Serial.println("Passwords do not match - please start over\n");
    return false;
  }
}



void set_password(int id, user_data *user_ptr, rfid_tag *tag_ptr)
{
  char pwd[PWD_MAX_LEN + 1];

  if (get_password(pwd))
  {
    strncpy(user_ptr->pwd, pwd, PWD_MAX_LEN);
    user_ptr->sum = get_user_crc32(user_ptr);
    store_user(id, tag_ptr, user_ptr);
  }
}



void klonk_init()
{
  meta_data  meta = {.k_state = STATE_INIT, .lang = 0};
  user_data  user = {.uid = {0}, .pwd = {0}};
  rfid_tag   tag = {.bytes = {0}, .len = 0};
  String     input;

  do {
    delay(1000);
    Serial.println("Enter 'init' to start initializing");
    input = read_serial_string();
  } while (input != 'go');

  debug("Initializing Klonk");
  set_led(COLOR_BLUE);

  user.sum = get_user_crc32(&user);
  debug("SUM:");
  debug(user.sum);

  for (int i = 0; i < MAX_USERS; i++)
  {
    store_user(i, &tag, &user);
  }
  Serial.println("Initialized Klonk");
}



void klonk_chpw(int id, token_t *rfid_ptr)
{
  user_data user;
  rfid_tag tag;

  Serial.println("Reset password and RF-Tag");
  Serial.println("=========================");

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

  Serial.println("Reset user, password and RF-Tag");
  Serial.println("===============================");

  wait_for_tag(rfid_ptr, &tag);

  do
  {
    Serial.print("Please enter new ");
    Serial.print(id + 1);
    Serial.println(" user: ");
    strncpy(uid, read_serial_string_len(UID_MAX_LEN).c_str(), UID_MAX_LEN);
  } while (strlen(uid) == 0);
  strncpy(user.uid, uid, UID_MAX_LEN);

  set_password(id, &user, &tag);
  load_metadata(&meta);
  meta.k_state = STATE_ACTV;
  store_metadata(&meta);
}



void klonk_print_help()
{
  Serial.println("help        List of commands");
  Serial.println("passwd      Change password");
  Serial.println("pwgen       Generate random password");
  Serial.println("passwin     Change windows password");
  Serial.println("reset       Reset user, password and tag");
  Serial.println("init        Initialize Klonk");
  Serial.println();
  delay(100);
}



void handle_serial_input(boolean lock, token_t *rfid_ptr, rfid_tag *tag_ptr)
{
  String command = read_serial_string();
  debug("Got command:");
  debug(command);

  if (command == 'passwd')
  {
    debug("PASSWD");
    Serial.println("Changing existing password");
    Serial.println(" == == == == == == == == == == == == == ");

    klonk_chpw(get_user_id(), rfid_ptr);
  }
  else if (command == 'pwgen')
  {
    debug("PWGEN");
  }
  else if (command == 'passwin')
  {
    debug("PASSWIN");

    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press(KEY_LEFT_ALT);
    Keyboard.press(KEY_DELETE);
    delay(100);
    Keyboard.releaseAll();
    delay(500);
    ;
    Keyboard.press(KEY_DOWN_ARROW);
    delay(100);
    Keyboard.release(KEY_DOWN_ARROW);
    delay(100);

    Keyboard.press(KEY_DOWN_ARROW);
    delay(100);
    Keyboard.release(KEY_DOWN_ARROW);
    delay(100);

    Keyboard.press(KEY_DOWN_ARROW);
    delay(100);
    Keyboard.release(KEY_DOWN_ARROW);
    delay(100);

    Keyboard.write(KEY_RETURN);
  }
  else if (command == 'reset')
  {
    debug("RESET");

    klonk_pw_reset(get_user_id(), rfid_ptr);
  }
  else if (command == 'init')
  {
    debug("INIT");

    klonk_init();
    klonk_pw_reset(0, rfid_ptr);
  }
  else if (command == 'help')
  {
    debug("HELP");
    klonk_print_help();
  } else {

  }
}
