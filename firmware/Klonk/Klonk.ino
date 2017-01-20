//
// This sketch pretends to be a keyboard and prints a stored password by pressing a button. Furthermore you can login/logout by registered RFID-Tag.
// There are 4 commands available to administer the user and password.
//
// Author: Ming-Che Lee <ming-che.lee@gmx.ch
//
// Licence: You may use, alternate and re-distribute it as you wish. Use at own risk!
//

#include "config.h"

#include <Arduino.h>
#include <SPI.h>
#include <EEPROM.h>

#include "Klonk.h"
#include "usToDE.h"
#include "serial_ui.h"
#include "store.h"
#include "HID-Project.h"



#ifdef LED
#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(1, LED_PIN, LED_TYPE);

void led_init() {
  pixels.begin();
}

void set_led(uint8_t red, uint8_t green, uint8_t blue) {
  pixels.setPixelColor(0, red, green, blue);
  pixels.show();
}

#else
// No RGB LED; nothing to be done
void led_init() {
}

void set_led(uint8_t red, uint8_t green, uint8_t blue) {
}
#endif      // #ifdef LED



void send_string_to_keyboard(const char *str, boolean lang)
{
  debug("Sending KBD string");
  debug_v(str);

  for (int i = 0; i < strlen(str); i++)
  {
    if (lang)
    {
      BootKeyboard.write(str[i]);
    } else {
      BootKeyboard.write(pgm_read_byte(usToDE + str[i]));
    }
  }
}



void send_password(user_data *user_ptr, boolean lang)
{
  send_string_to_keyboard(user_ptr->pwd, lang);
  BootKeyboard.write(KEY_RETURN);
  delay(500);
}



void send_user_password(user_data *user_ptr, boolean lang)
{
  send_string_to_keyboard(user_ptr->uid, lang);
  delay(500);

  BootKeyboard.press(KEY_TAB);
  delay(100);
  BootKeyboard.release(KEY_TAB);

  send_password(user_ptr, lang);
}



void logout()
{
  return;
  // Press WINDOWS + L
  BootKeyboard.press(KEY_LEFT_GUI);
  BootKeyboard.press(KEY_L);
  delay(100);
  BootKeyboard.releaseAll();
  delay(2000);
}



void login(user_data *user_ptr, boolean lang)
{
  return;
  // CTRL-ALT-DEL:
  BootKeyboard.press(KEY_LEFT_CTRL);
  BootKeyboard.press(KEY_LEFT_ALT);
  BootKeyboard.press(KEY_DELETE);
  delay(100);
  BootKeyboard.releaseAll();
  delay(1000);
  send_password(user_ptr, lang);
  delay(2000);
}



byte get_button_action()
{
  unsigned long timeHold;
  unsigned long startTime = millis();
  byte          state = NONE;

  while (digitalRead(BUTTON_PIN) == PRESSED)
  {
    timeHold = millis() - startTime;

    if (timeHold > 50 && timeHold < LONG_PRESS)
    {
      set_led(COLOR_WHITE);
      state = SHORT;
    }
    else if (timeHold < LANG_PRESS)
    {
      set_led(COLOR_LIGHT_BLUE);
      state = LONG;
    }
    else
    {
      set_led(COLOR_LILA);
      state = LANG;
    }
  }

  return state;
}



/*
   Arduino setup function
*/
void setup()
{
  SPI.begin();

  led_init();
  set_led(COLOR_WHITE);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  //  Keyboard.begin();
  BootKeyboard.begin();
  Serial.begin(9600);
  PRINT_LN_S("klonk - Type \"help\" for available commands");
}


/*
   Arduino mein function
*/
void loop()
{
  static byte     userNum;
  static boolean  lock;
  static token_t  rfid;
  static meta_data meta;
  static user_data user;
  static rfid_tag tag;
  byte   action;

  token_init(&rfid);

  lock = true;
  set_led(COLOR_RED);

  load_metadata(&meta);

  if (meta.k_state != STATE_ACTV)
  {
    debug("State undefined; initializing");

    klonk_init();
    klonk_pw_reset(0, &rfid);

    load_metadata(&meta);
  }
  tag = {.bytes = {0}, .len = 0};


  while (1)
  {
    action = get_button_action();

    if (lock == false) {
      if (BootKeyboard.getLeds() & LED_SCROLL_LOCK) {
        set_led(COLOR_YELLOW);
        userNum = 1;
      } else {
        set_led(COLOR_GREEN);
        userNum = 0;
      }
      switch (action)
      {
        case SHORT:
          debug("P");
          load_user(userNum, &tag, &user);
          send_password(&user, meta.lang);
          break;
        case LONG:
          debug("UP");
          load_user(userNum, &tag, &user);
          send_user_password(&user, meta.lang);
          break;
        case LANG:
          debug("LANG");
          meta.lang = ! meta.lang;
        default:
          break;
      }
    } else {
      set_led(COLOR_RED);
    }

    // handle serial input
    if (Serial.available() > 0)
    {
      debug("Handling input");

      handle_serial_input(lock, &rfid, &tag);
      load_metadata(&meta);
    }

    if (is_tag_available(&rfid))
    {
      debug("Getting TAG");
      get_tag(&rfid, &tag);

#if (DEBUG > 0)
      PRINT_LN_S("Got Tag");
      for (int i = tag.len - 1; i > 0; i--)
      {
        PRINT_HEX(tag.bytes[i]);
      }
      debug("");
      debug("END TAG");
#endif

      if (validate_tag(&tag))
      {
        load_user(userNum, &tag, &user);
        lock = !lock;            //toggle status
        if (lock)
        {
          logout();
        }
        else
        {
          login(&user, meta.lang);
        }

        delay(500);
      } // end if (validate_tag(&tag))
      delay(500);
    }
  } // end WHILE
}

