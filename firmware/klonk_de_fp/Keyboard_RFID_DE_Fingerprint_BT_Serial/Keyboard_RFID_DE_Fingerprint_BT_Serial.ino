//
// This sketch pretends to be a keyboard and prints a stored password by pressing a button. Furthermore you can login/logout by registered fingerprints. 
// There are 4 commands available to administer the user and password.
// 
// Author: Ming-Che Lee <ming-che.lee@gmx.ch
//
// Licence: You may use, alternate and re-distribute it as you wish. Use at own risk!
// 

#include <Keyboard.h>
#include <EEPROM.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
#include "usToDE.h"

#define OLED 5
#define NUMPIXELS 1
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, OLED, NEO_GRB + NEO_KHZ800);
int getFingerprintIDez(), fingerID = 0;
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial1);
SoftwareSerial BTserial(9, 10);

String entryTAG, serialString = "", serialString2 = "", newPWD = "", cmpPWD = "", replySave = "", PW, myUSER;
byte LOCK = 1, i, PosPW = 0, PosTAG = 25, PosUSER = 50;
const byte BTstate = 6;
boolean BTconnected = false, LBTconnected = false;
unsigned long previousMillis = 0, interval = 5000;

byte randomValue;
char randPW[25], inChar;
byte randNumBytes = 20;
int charsRead;

const int EEPROM_MIN_ADDR = 0;
const int EEPROM_MAX_ADDR = 1023;
const int BUFSIZE = 25;
char buf[BUFSIZE];

const byte buttonPin = 4;
const byte buttonSel = 7;
byte buttonState = 0; 
byte lastButtonState = 0;
int startPressed = 0;
int endPressed = 0;
int timeHold = 0;    
int timeReleased = 0;

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

  while (waitPWD == 0)
  {
    if (BTserial.available())
    {
      tmpStrg = BTserial.readString();
      if ( tmpStrg.length() < BUFSIZE)
      {
        waitPWD = 1;
      }
      else
      {
        BTserial.println("Input > 25 chars, try again");
      }
    }
  }
  return tmpStrg;
}

boolean def_USER()
{
  PosPW = 0;
  eeprom_read_string(PosPW, buf, BUFSIZE);
  PW = buf;
  eeprom_read_string(50, buf, BUFSIZE);
  myUSER = buf;
}

boolean setUSER (byte pos, String user)
{
  BTserial.println("Please enter new "+user+" user: ");
  replySave = readSerialStrg();
  myUSER = "";
  myUSER = saveStrgEEP(replySave,pos);
  BTserial.print("New User ");
  BTserial.print(myUSER);
  BTserial.println(" is set\n");
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
    PW = saveStrgEEP(newPWD,PosPW);
    BTserial.print("New password ");
    BTserial.print(newPWD);
    BTserial.println(" is saved\n");
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

void stat_led_orange()
{
  pixels.setPixelColor(0, pixels.Color(35, 75, 0));
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

void show_user_led ()
{
  switch (PosPW)
  {
    case 0:
      stat_led_green();
      break;
    case 75:
      stat_led_orange();
      break;
  }
}

void logout()
{
  stat_led_red();
  def_USER();
  
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
  def_USER();
      
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

int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;
  
  return finger.fingerID; 
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
      show_user_led();
    }
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
        show_user_led();
      }
    }
  }
  
  if ( LOCK == 0 && BTconnected == false && LBTconnected == true )
  { 
    LOCK = 1; 
    logout();
  }
}

void setup()
{
  pixels.begin();
  finger.begin(57600);

  pinMode(BTstate, INPUT);   
  delay(5000);

  pinMode(buttonPin, INPUT);
  pinMode(buttonSel, INPUT);
  digitalWrite(buttonPin, HIGH);
  digitalWrite(buttonSel, HIGH);

  stat_led_red();

  Keyboard.begin();
  Serial.begin(9600);
  Serial.println("klonk - avail. cmds via bluetooth terminal");
  BTserial.begin(38400);
  BTserial.println("klonk");
  
  randomSeed(analogRead(0));
    
  if (finger.verifyPassword())
  {
    Serial.println("FP sensor OK!");
    Serial.println();
  } 
  else
  {
    Serial.println("FP sensor NOK");
    Serial.println();
    while (1);
  }
  
  def_USER();
}

void loop()
{
 while(1)
 {

  if ( digitalRead(buttonSel) == LOW && myUSER != "FFFFFFFFFFFFFFFFFFFF" && LOCK == 0 )
  {
    if ( PosPW == 0)
    {
      PosPW = 75;
      PosUSER = 125;
      stat_led_orange();
    }
    else
    {
      PosPW = 0;
      PosUSER = 50;
      stat_led_green();
    }
    
    eeprom_read_string(PosPW, buf, BUFSIZE);
    PW = buf;
    eeprom_read_string(PosUSER, buf, BUFSIZE);
    myUSER = buf;

    digitalWrite(buttonSel, HIGH);
    delay(500);
  }
  
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
    BTserial.print("Generated password: ");
    BTserial.println(randPW);
    BTserial.println("Replace old password? <y/n>");
    replySave = readSerialStrg();
    
    if (replySave == "y")
    {
      PW = "";
      PW = saveStrgEEP(randPW,PosPW);
      BTserial.println("Old password replaced by generated password\n");
    }
    else
    {
      BTserial.println("Old password not replaced\n");
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
    BTserial.println("Reset user and password");
    BTserial.println("Please enter password: ");
    replySave = readSerialStrg();

    if ( replySave == PW )
    {
      myUSER = "FFFFFFFFFFFFFFFFFFFF";
      BTserial.println();
    }
    else
    {
      BTserial.println("Wrong password - reset not started\n");
    }
  }

  if ( serialString == "help" )
  {
    BTserial.println("help     List of commands");
    BTserial.println("passwd   Change password");
    BTserial.println("pwgen    Generate random password");
    BTserial.println("passwin  Change windows password");
    BTserial.println("reset    Reset user and password");
    BTserial.println();
    delay(100);
  }

  serialString = "";
  replySave = "";

  if ( myUSER == "FFFFFFFFFFFFFFFFFFFF" && PW == "FFFFFFFFFFFFFFFFFFFF" )
  {
    stat_led_blue();
    
    while (serialString2 != "go")
    {
      delay (1000);
      BTserial.println("Please enter \"go\" to init user and password");
      if (BTserial.available()) 
      {
        // read incoming serial data:
        serialString2 = BTserial.readString();
      }
    } 
    serialString2 = "";
    BTserial.println();
  }

  if ( myUSER == "FFFFFFFFFFFFFFFFFFFF" )
  {
    delay(2000);
    stat_led_blue();
    
    BTserial.println("Init sequence started");
    setUSER(50,"login");
    PosPW = 0;

    if ( chgPW() == true )
    {
      LOCK = 0;
      stat_led_green();
      
      BTserial.println("Init seq. done - your user and passwd are set.\n");

      BTserial.println("Set 2nd user? <y/n>");
      replySave = readSerialStrg();
        
      if (replySave == "y")
      {
        stat_led_orange();
        setUSER(125,"second");
        PosPW = 75;
        chgPW();
      }
      else
      {
        BTserial.println("-> 2nd user not set\n");
      }

      stat_led_green();
      def_USER();
      BTserial.println("Type 'help' for admin commands\n");
      delay(1300);
    }
    else
    {
      return;
    }
  }

  BT_tracker();

  fingerID = getFingerprintIDez();
  
  if ( fingerID > 0 )
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
    delay(50);
  }
 } 
}

