#ifndef KLONK_H
#define KLONK_H 1

  #include <Arduino.h>
  #include "config.h"
  
  // LED and Button pin definitions
  #if (BOARD == 1) // BCP
    #define LED_PIN     18
    #define BUTTON_PIN  19
  #else
    #define LED_PIN     5
    #define BUTTON_PIN  4
  #endif
  
  
  #define COLOR_RED    25,  0,  0
  #define COLOR_ORANGE 75, 35,  0
  #define COLOR_GREEN   0, 25,  0
  #define COLOR_BLUE    0,  0, 25
  #define COLOR_WHITE  25, 25, 25

  #ifdef LED
    #include <Adafruit_NeoPixel.h>
  
    #if (LED == 5)
      #define LED_TYPE (NEO_GRB + NEO_KHZ800)
    #elif (LED == 8)
      #define LED_TYPE (NEO_RGB + NEO_KHZ800)
    #else
      #define LED_TYPE (NEO_GRB + NEO_KHZ800)
    #endif
  #endif      // #ifdef LED

  #define NONE  0
  #define SHORT 1
  #define LONG  2

  #define START_DELAY 1000

  struct RFID_TAG {
    byte bytes[TAG_LEN];
    byte len;
  };

  void led_init();
  void set_led(uint8_t red, uint8_t green, uint8_t blue);

#endif
