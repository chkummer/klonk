//
// This sketch pretends to be a keyboard and prints a stored password by pressing a button. Furthermore you can login/logout by registered RFID-Tag. 
// There are 4 commands available to administer the user and password.
// 
// Author: Ming-Che Lee <ming-che.lee@gmx.ch
//
// Licence: You may use, alternate and re-distribute it as you wish - it is complete free without any warranty. Use at own risk!
// 

#include <SPI.h>
#include <MFRC522.h>
#include <Keyboard.h>
#include <EEPROM.h>
#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>
#include "usToDE.h"

#define SDA_PIN 2
#define RST_PIN 3
#define OLED 5
#define NUMPIXELS 1

MFRC522 mfrc522(SDA_PIN, RST_PIN);
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, OLED, NEO_GRB + NEO_KHZ800);
SoftwareSerial BTserial(9, 10);

String entryTAG, serialString = "", serialString2 = "", newPWD = "", cmpPWD = "", replySave = "", PW, myUSER, myTAG;
byte LOCK = 1, i;
const byte BTstate = 6;
boolean BTconnected = false, LBTconnected = false;

byte randomValue;
char randPW[25], inChar;
byte randNumBytes = 20;
int charsRead;

const int EEPROM_MIN_ADDR = 0;
const int EEPROM_MAX_ADDR = 1023;
const int BUFSIZE = 25;
char buf[BUFSIZE];

const byte buttonPin = 4;
byte buttonState = 0; 
byte lastButtonState = 0;
int startPressed = 0;
int endPressed = 0;
int timeHold = 0;    
int timeReleased = 0;
unsigned long previousMillis = 0, interval = 10000;

boolean eeprom_is_addr_ok(int addr) 
{
  return ((addr >= EEPROM_MIN_ADDR) && (addr <= EEPROM_MAX_ADDR));
}

boolean eeprom_write_bytes(int startAddr, const byte* array, int numBytes) 
{
  int i;
  if (!eeprom_is_addr_ok(startAddr) || !eeprom_is_addr_ok(startAddr + numBytes)) 
  {
    return false;
  }
  for (i = 0; i < numBytes; i++) 
  {
    EEPROM.write(startAddr + i, array[i]);
  }
  return true;
}

boolean eeprom_write_string(int addr, const char* string) 
{
  int numBytes;
  numBytes = strlen(string) + 1;
  return eeprom_write_bytes(addr, (const byte*)string, numBytes);
}

boolean eeprom_read_string(int addr, char* buffer, int bufSize) 
{
  byte ch;
  int bytesRead;
  if (!eeprom_is_addr_ok(addr)) 
  { 
    return false;
  }

  if (bufSize == 0) 
  { 
    return false;
  }
  
  if (bufSize == 1) 
  {
    buffer[0] = 0;
    return true;
  }
  
  bytesRead = 0;
  ch = EEPROM.read(addr + bytesRead);
  buffer[bytesRead] = ch;
  bytesRead++;
  
  while ( (ch != 0x00) && (bytesRead < bufSize) && ((addr + bytesRead) <= EEPROM_MAX_ADDR) ) 
  {
    ch = EEPROM.read(addr + bytesRead);
    buffer[bytesRead] = ch;
    bytesRead++;
  }
  
  if ((ch != 0x00) && (bytesRead >= 1)) 
  {
    buffer[bytesRead - 1] = 0;
  }
  return true;
}

char* saveStrgEEP(String myString, byte addr)
{
  char myStringChar[BUFSIZE];

  myString.toCharArray(myStringChar, BUFSIZE);
  strcpy(buf, myStringChar);
  eeprom_write_string(addr, buf); 

  eeprom_read_string(addr, buf, BUFSIZE);
  return buf;
}

String readSerialStrg ()
{
  byte waitPWD = 0;
  String tmpStrg = "";

  i = 0;
  while (waitPWD == 0)
  {
    while (BTserial.available() > 0)
    {
      inChar = BTserial.read();
      if ( inChar != '\r' && inChar != '\n' && i < BUFSIZE)
      {
        tmpStrg += inChar;
      }
      waitPWD = 1;
      i++;
    }
  }
  return tmpStrg;
}

boolean chgPW ()
{
  newPWD = "123456789012345618901";
  while ( newPWD.length() > 20 )
  {
    BTserial.println("Pleae enter new password: ");
    newPWD = readSerialStrg();
    if ( newPWD.length() > 20 )
    {
      BTserial.println("Password too long - max length are 20 chars, please retry\n");
      newPWD = "123456789012345618901";
    }
  }
    
  BTserial.println("Pleae enter new password again: ");
  cmpPWD = readSerialStrg();

  if (newPWD == cmpPWD)
  {
    PW = "";
    PW = saveStrgEEP(newPWD,0);
    BTserial.print("New password ");
    BTserial.print(newPWD);
    BTserial.println(" is saved - please test before use\n");
    return true;
  }
  else
  {
    BTserial.println("Passwords do not match - please start over\n");
    return false;
  }
}  

void prtKbdStrg(String tmpStrg)
{
  byte strgLen = tmpStrg.length();
  for (i = 0; i < strgLen; i++)
  {
    inChar = tmpStrg[i];
    unsigned char key = (usToDE[inChar]);
    Keyboard.write(key);
  }
}

void stat_led_red()
{
  pixels.setPixelColor(0, pixels.Color(0, 25, 0));
  pixels.show();
}

void stat_led_green()
{
  pixels.setPixelColor(0, pixels.Color(25, 0, 0));
  pixels.show();
}

void stat_led_blue()
{
  pixels.setPixelColor(0, pixels.Color(0, 0, 25));
  pixels.show();
}

void logout()
{
  stat_led_red();
  
  // CTRL-ALT-DEL:
  Keyboard.press(KEY_LEFT_CTRL);
  Keyboard.press(KEY_LEFT_ALT);
  Keyboard.press(KEY_DELETE);
  delay(200);
  Keyboard.releaseAll();
  delay(1500);
  Keyboard.write(KEY_RETURN);
}

void login()
{
  stat_led_green();
      
  // CTRL-ALT-DEL:
  Keyboard.press(KEY_LEFT_CTRL);
  Keyboard.press(KEY_LEFT_ALT);
  Keyboard.press(KEY_DELETE);
  delay(100);
  Keyboard.releaseAll();
  delay(1000);
  Keyboard.print(PW);
  Keyboard.write(KEY_RETURN);
}

void BT_tracker()
{
  LBTconnected = BTconnected;

  if ( digitalRead(BTstate) == HIGH)  
  { 
    BTconnected = true;
    if ( LOCK == 1 )
    {
      stat_led_red();
    }
    else
    {
      stat_led_green();
    }
    delay(100);
  }
  else
  {
    BTconnected = false;
    if (millis() - previousMillis > interval) 
    {
      previousMillis = millis();
      stat_led_blue();
    }
    else
    {
      if ( LOCK == 1 )
      {
        stat_led_red();
      }
      else
      {
        stat_led_green();
      }
    }
    delay(100);
  }
  
  if ( LOCK == 0 && BTconnected == false && LBTconnected == true )
  { 
    LOCK = 1; 
    logout();
  }
}

void setup()
{
  SPI.begin();
  pixels.begin();
  mfrc522.PCD_Init();

  pinMode(BTstate, INPUT);   
  delay(1000);

  pinMode(buttonPin, INPUT);
  digitalWrite(buttonPin, HIGH);

  stat_led_red();
  
  Keyboard.begin();
  Serial.begin(9600);
  Serial.println("klonk - Type \"help\" for available commands");
  BTserial.begin(38400);
  BTserial.println("klonk - Type \"help\" for available commands");

  randomSeed(analogRead(0));
 
  eeprom_read_string(0, buf, BUFSIZE);
  PW = buf;
  eeprom_read_string(25, buf, BUFSIZE);
  myTAG = buf;
  eeprom_read_string(50, buf, BUFSIZE);
  myUSER = buf;
}

void loop()
{
  
  buttonState = digitalRead(buttonPin);
  if (buttonState != lastButtonState) 
  {
      if (buttonState == LOW) 
      {
          startPressed = millis();
          timeReleased = startPressed - endPressed;
      } 
      else 
      {
        endPressed = millis();
        timeHold = endPressed - startPressed;

        if (timeHold >= 50 && timeHold < 500 && LOCK == 0) 
        {
          prtKbdStrg(PW);
          Keyboard.write(KEY_RETURN);
          delay(500);
        }

        if (timeHold >= 500 && LOCK == 0) 
        {
          prtKbdStrg(myUSER);
          delay(500);

          Keyboard.press(KEY_TAB);
          Keyboard.release(KEY_TAB);
          delay(100);

          prtKbdStrg(PW);
          Keyboard.write(KEY_RETURN);
          delay(500);
        }
        digitalWrite(buttonPin, HIGH);
      }
  }
  lastButtonState = buttonState;
  
  if (BTserial.available()) 
  {
    // read incoming serial data:
    serialString = BTserial.readString();
  }

  if ( serialString == "passwd" && LOCK == 0)
  {
    BTserial.println("Changing existing password");
    BTserial.println("==========================");
    chgPW();
  }

  if ( serialString == "pwgen" && LOCK == 0)
  {
    randomValue = 0;
    memset(randPW, 0, sizeof(randPW));
    for(i = 0; i < randNumBytes; i++) 
    {
      randomValue = random(0, 35);
      if (randomValue >= 0 && randomValue <= 12)
      {
        randPW[i] = randomValue + 'a';
      }
      if (randomValue >= 13 && randomValue <= 25)
      {
        randPW[i] = randomValue + 'A';
      }
        
      if (randomValue >= 26) 
      {
        randPW[i] = (35 - randomValue) + '0';
      }
    }

    BTserial.println("Generate random password");
    BTserial.println("========================");
    BTserial.print("Generated password: ");
    BTserial.println(randPW);
    BTserial.println("Replace old password and save it? <y/n>");
    replySave = readSerialStrg();
    
    if (replySave == "y")
    {
      PW = "";
      PW = saveStrgEEP(randPW,0);
      BTserial.println("Old password replaced by generated password\n");
    }
    else
    {
      BTserial.println("Nothing saved - current password not replaced\n");
    }
  }

  if ( serialString == "passwin" && LOCK == 0)
  {
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.press(KEY_LEFT_ALT);
      Keyboard.press(KEY_DELETE);
      delay(100);
      Keyboard.releaseAll();
      delay(500);
      Keyboard.press(KEY_DOWN_ARROW);
      Keyboard.release(KEY_DOWN_ARROW);
      delay(100);
      Keyboard.press(KEY_DOWN_ARROW);
      Keyboard.release(KEY_DOWN_ARROW);
      delay(100);
      Keyboard.press(KEY_DOWN_ARROW);
      Keyboard.release(KEY_DOWN_ARROW);
      delay(100);
      Keyboard.write(KEY_RETURN);
  }

  if ( serialString == "reset")
  {
    BTserial.println("Reset user, password and RF-Tag");
    BTserial.println("===============================");
    BTserial.println("Please enter password: ");
    replySave = readSerialStrg();

    if ( replySave == PW )
    {
      myTAG = "FFFFFFFF";
      BTserial.println();
    }
    else
    {
      BTserial.println("Wrong password - reset not started, please try again\n");
    }
  }

  if ( serialString == "help" )
  {
    BTserial.println("help        List of commands");
    BTserial.println("passwd      Change password");
    BTserial.println("pwgen       Generate random password");
    BTserial.println("passwin     Change windows password");
    BTserial.println("reset       Reset user, password and tag");
    BTserial.println();
    delay(100);
    //Serial.println("\nThe commands are only available in authenticated mode - except help, reset and upload!");
    //Serial.println("\nSaved passwords will be printed in English keyboard layout -");
    //Serial.println("so test the saved password by pressing the 'print-password' button!\n");
  }

  serialString = "";
  replySave = "";

  if ( myTAG == "FFFFFFFF" && myUSER == "FFFFFFFFFFFFFFFFFFFF" && PW == "FFFFFFFFFFFFFFFFFFFF" )
  {
    stat_led_blue();
  
    while (serialString2 != "go")
    {
      delay (1000);
      BTserial.println("Please enter \"go\" to init user, password and RFID-tag");
      if (BTserial.available()) 
      {
        // read incoming serial data:
        serialString2 = BTserial.readString();
      }
    } 
    serialString2 = "";
    BTserial.println();
  }

  if ( myTAG == "FFFFFFFF" )
  {
    delay(2000);
    stat_led_blue();
  
    BTserial.println("Init sequence started");
    BTserial.println("=====================");
    BTserial.println("Please enter new user: ");
    replySave = readSerialStrg();
    myUSER = "";
    myUSER = saveStrgEEP(replySave,50);
    BTserial.print("New User ");
    BTserial.print(myUSER);
    BTserial.println(" is set\n");
    
        
    if ( chgPW() == true)
    {
      delay(1000);
      BTserial.println("Hold your RFID-Tag close to RFID-Receiver for at least one second to register...");

      i = 0;
      while ( i == 0 )
      {
        while ( ! mfrc522.PICC_IsNewCardPresent() )
        {
          // wait until card is detected...
        }

        if ( mfrc522.PICC_ReadCardSerial() )
        {
          i = 1; // wailt until RIFD sender is choosen...
        }
      }

      delay(1000);
      
      entryTAG = "";
      for (byte i = 0; i < mfrc522.uid.size; i++)
      {
        entryTAG = entryTAG + String (mfrc522.uid.uidByte[i], HEX);
      }

      myTAG = "";
      myTAG = saveStrgEEP(entryTAG,25);
      LOCK = 0;
      stat_led_green();
      
      BTserial.print("Registered TAG: ");
      BTserial.println(myTAG);
      BTserial.println();
      BTserial.println("Init seq. done - your user and passwd are set and RFID-TAG is registered.\n");
      //Serial.println("Type 'help' for admin commands\n");
      delay(1300);
    }
    else
    {
      return;
    }
  }

  BT_tracker();
 
  if ( ! mfrc522.PICC_IsNewCardPresent()) // Wenn eine Karte in Reichweite ist...
  {
    return; // gehe weiter...
  }

  if ( ! mfrc522.PICC_ReadCardSerial()) // Wenn ein RFID-Sender ausgewählt wurde
  {
    return; // gehe weiter...
  }

  entryTAG = "";
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    entryTAG = entryTAG + String (mfrc522.uid.uidByte[i], HEX);
  }

  if ( entryTAG == myTAG )
  {
     
    if ( LOCK == 1) 
    { 
      LOCK = 0; 
      login();
    } 
    else 
    { 
      LOCK = 1; 
      logout();
    }
  delay(500);
  }
}

