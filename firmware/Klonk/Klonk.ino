
//
// This sketch pretends to be a keyboard and prints a stored password by pressing a button. Furthermore you can login/logout by registered RFID-Tag.
// There are 4 commands available to administer the user and password.
//
// Author: Ming-Che Lee <ming-che.lee@gmx.ch
//
// Licence: You may use, alternate and re-distribute it as you wish. Use at own risk!
//

#include <Arduino.h>
#include <SPI.h>
#include <Keyboard.h>
#include <EEPROM.h>

#include "Klonk.h"
#include "usToDE.h"
#include "config.h"
#include "serial_ui.h"
#include "store.h"



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



void send_string_to_keyboard(const char *str)
{
  int i = 0;
  while ( str[i] )
  {
    Keyboard.write(usToDE[str[i]]);
  }
}



void send_password(user_data *user_ptr)
{
  send_string_to_keyboard(user_ptr->pwd);
  Keyboard.write(KEY_RETURN);
  delay(500);
}



void send_user_password(user_data *user_ptr)
{
  send_string_to_keyboard(user_ptr->uid);
  delay(500);

  Keyboard.press(KEY_TAB);
  delay(100);
  Keyboard.release(KEY_TAB);
  delay(100);

  send_password(user_ptr);
}



void logout()
{
  set_led(COLOR_RED);

  // Press WINDOWS + L
  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('l');
  delay(200);
  Keyboard.releaseAll();
}



void login(user_data *user_ptr)
{
  set_led(COLOR_GREEN);

  // CTRL-ALT-DEL:
  Keyboard.press(KEY_LEFT_CTRL);
  Keyboard.press(KEY_LEFT_ALT);
  Keyboard.press(KEY_DELETE);
  delay(100);
  Keyboard.releaseAll();
  delay(1000);
  send_password(user_ptr);
}



byte get_button_action()
{
  static long startPressed;
  static long endPressed;
  static byte buttonState, oldButtonState;

  int timeHold;
  int timeStamp = millis();

  if (timeStamp < START_DELAY) {
    startPressed = 0;
    endPressed = 0;

    return NONE;
  }

  if (buttonState != oldButtonState) {
    if (digitalRead(BUTTON_PIN) == LOW)
    {
      startPressed = timeStamp;
    }
    else
    {
      endPressed = timeStamp;
      timeHold = endPressed - startPressed;

      if (timeHold >= 50 && timeHold < 500) //short pressed
      {
        return SHORT;
      }
      else if (timeHold >= 500)              //long pressed
      {
        return LONG;
      }
    }
  }

  return NONE;
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

  Keyboard.begin();
  Serial.begin(9600);
  Serial.println("klonk - Type \"help\" for available commands");
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

  if (lock == false) {
    switch (get_button_action())
    {
      case SHORT:
        debug("Send password");
        send_password(&user);
        break;
      case LONG:
        debug("Send user + password");
        send_user_password(&user);
        break;
      default:
        break;
    }
  }

  if (millis() < 1000) {
    token_init(&rfid);

    //    lock = true;
    lock = false;
    load_metadata(&meta);
    tag = {.bytes = {0}, .len = 0};

    delay(START_DELAY - millis());
    //    set_led(COLOR_RED);
    set_led(COLOR_GREEN);
  }

  if (meta.k_state != STATE_ACTV)
  {
    debug("State undefined; initializing");

    klonk_init();
    klonk_pw_reset(0, &rfid);

    load_metadata(&meta);
  }

  //  // handle serial input
  //  if (Serial.available() > 0)
  //  {
  //    debug("Handling input");
  //    handle_serial_input(lock, &rfid, &tag);
  //  }


  //  if (is_tag_available(&rfid))
  //  {
  //    get_tag(&rfid, &tag);
  //
  //    if (validate_tag(&tag))
  //    {
  //      load_user(userNum, &tag, &user);
  //      lock = !lock;            //toggle status
  //      if (lock)
  //      {
  //        logout();
  //      }
  //      else
  //      {
  //        login(&user);
  //      }
  //
  //      delay(500);
  //    }
  //  }
}
