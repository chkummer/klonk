//
// This sketch pretends to be a keyboard and prints a stored password by pressing a button. Furthermore you can login/logout by registered RFID-Tag. 
// There are 4 commands available to administer the user and password.
// 
// Author: Ming-Che Lee <ming-che.lee@gmx.ch
//
// Licence: You may use, alternate and re-distribute it as you wish. Use at own risk!
// 

#include <Keyboard.h>
#include <EEPROM.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_PN532.h>
#include "usToDE.h"

#define PN532_SCK  (15)
#define PN532_MOSI (16)
#define PN532_SS   (2)
#define PN532_MISO (14)
#define PN532_IRQ   (2)
#define PN532_RESET (3)
#define OLED 5
#define NUMPIXELS 1
Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, OLED, NEO_GRB + NEO_KHZ800);

String entryTAG, serialString = "", serialString2 = "", newPWD = "", cmpPWD = "", replySave = "", PW, myUSER, myTAG;
byte LOCK = 1, i, PosPW = 0, PosTAG = 25, PosUSER = 50;

byte randomValue;
char randPW[25], inChar;
byte randNumBytes = 20;

const int EEPROM_MIN_ADDR = 0;
const int EEPROM_MAX_ADDR = 1023;
const int BUFSIZE = 25;
char buf[BUFSIZE];

//const byte buttonPin = 4; // klonk breadboard
const byte buttonPin = 19; // klonk PCB
const byte buttonSel = 7;
byte buttonState = 0; 
byte lastButtonState = 0;
int startPressed = 0;
int endPressed = 0;
int timeHold = 0;    
int timeReleased = 0;

uint8_t success;
uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
uint8_t uidLength;
uint32_t cardid = uid[0];

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
    while (Serial.available() > 0)
    {
      inChar = Serial.read();
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
  Serial.println("Please enter new "+user+" user: ");
  replySave = readSerialStrg();
  myUSER = "";
  myUSER = saveStrgEEP(replySave,pos);
  Serial.print("New User ");
  Serial.print(myUSER);
  Serial.println(" is set\n");
}

boolean chgPW ()
{
  newPWD = "123456789012345618901";
  while ( newPWD.length() > 20 )
  {
    Serial.println("Please enter new password: ");
    newPWD = readSerialStrg();
    if ( newPWD.length() > 20 )
    {
      Serial.println("Password too long - max length are 20 chars, please retry\n");
      newPWD = "123456789012345618901";
    }
  }
    
  Serial.println("Please enter new password again: ");
  cmpPWD = readSerialStrg();

  if (newPWD == cmpPWD)
  {
    PW = "";
    PW = saveStrgEEP(newPWD,PosPW);
    Serial.print("New password ");
    Serial.print(newPWD);
    Serial.println(" is saved - please test before use\n");
    return true;
  }
  else
  {
    Serial.println("Passwords do not match - please start over\n");
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
  //pixels.setPixelColor(0, pixels.Color(0, 25, 0)); //RGB APA-106, F8
  pixels.setPixelColor(0, pixels.Color(25, 0, 0)); //RGB APA-106, F5
  pixels.show();
}

void stat_led_orange()
{
  //pixels.setPixelColor(0, pixels.Color(35, 75, 0)); //RGB-APA 106, F8
  pixels.setPixelColor(0, pixels.Color(75, 35, 0)); //RGB APA-106, F5
  pixels.show();
}

void stat_led_green()
{
  //pixels.setPixelColor(0, pixels.Color(25, 0, 0)); //RGB APA-106, F8
  pixels.setPixelColor(0, pixels.Color(0, 25, 0)); //RGB APA-106, F5
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
  prtKbdStrg(PW);
  Keyboard.write(KEY_RETURN);
}

void setup()
{
  pixels.begin();
  
  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("PN53x not found");
    while (1); // halt
  }
  nfc.SAMConfig();

  pinMode(buttonPin, INPUT);
  pinMode(buttonSel, INPUT);
  digitalWrite(buttonPin, HIGH);
  digitalWrite(buttonSel, HIGH);

  stat_led_red();
  
  Keyboard.begin();
  Serial.begin(9600);
  Serial.println("klonk - Type \"help\" for available commands");

  randomSeed(analogRead(0));
  
  eeprom_read_string(PosTAG, buf, BUFSIZE);
  myTAG = buf;
  def_USER();
}

void loop()
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
  
  while (Serial.available() > 0) 
  {
    // read incoming serial data:
    inChar = Serial.read();
    serialString += inChar;
  }

  if ( serialString == "passwd\r\n" && LOCK == 0)
  {
    Serial.println("Changing existing password");
    Serial.println("==========================");
    chgPW();
  }

  if ( serialString == "pwgen\r\n" && LOCK == 0)
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

    Serial.println("Generate random password");
    Serial.println("========================");
    Serial.print("Generated password: ");
    Serial.println(randPW);
    Serial.println("Replace old password and save it? <y/n>");
    replySave = readSerialStrg();
    
    if (replySave == "y")
    {
      PW = "";
      PW = saveStrgEEP(randPW,PosPW);
      Serial.println("Old password replaced by generated password\n");
    }
    else
    {
      Serial.println("Nothing saved - current password not replaced\n");
    }
  }

  if ( serialString == "passwin\r\n" && LOCK == 0)
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

  if ( serialString == "reset\r\n")
  {
    stat_led_green();
    def_USER();
    
    Serial.println("Reset user, password and RF-Tag");
    Serial.println("===============================");
    Serial.println("Please enter password: ");
    replySave = readSerialStrg();

    if ( replySave == PW )
    {
      myTAG = "FFFFFFFF";
      Serial.println();
    }
    else
    {
      Serial.println("Wrong password - reset not started, please try again\n");
    }
  }

  if ( serialString == "help\r\n" )
  {
    Serial.println("help        List of commands");
    Serial.println("passwd      Change password");
    Serial.println("pwgen       Generate random password");
    Serial.println("passwin     Change windows password");
    Serial.println("reset       Reset user, password and tag");
    Serial.println();
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
  
    while (serialString2 != "go\r\n")
    {
      delay (1000);
      Serial.println("Please enter \"go\" to init user, passowrd and RFID-tag");
      while (Serial.available() > 0) 
      {
        // read incoming serial data:
        inChar = Serial.read();
        serialString2 += inChar;
      }
    } 
    serialString2 = "";
    Serial.println();
  }

  if ( myTAG == "FFFFFFFF" )
  {
    delay(2000);
    stat_led_blue();
  
    Serial.println("Init sequence started");
    Serial.println("=====================");
    setUSER(50,"login");
    PosPW = 0;
        
    if ( chgPW() == true)
    {
      delay(1000);
      Serial.println("Hold your RFID-Tag close to RFID-Receiver for at least one second to register...");

      i = 0;
      while ( i == 0 )
      {
        success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
        if (success)
        {
          if (uidLength == 4)
          {
            cardid = uid[0];
            cardid <<= 8;
            cardid |= uid[1];
            cardid <<= 8;
            cardid |= uid[2];  
            cardid <<= 8;
            cardid |= uid[3]; 
            i = 1;
          }
        }
      }

      delay(1000);
      
      entryTAG = cardid;

      myTAG = "";
      myTAG = saveStrgEEP(entryTAG,25);
      LOCK = 0;
      stat_led_green();
      
      Serial.print("Registered TAG: ");
      Serial.println(myTAG);
      Serial.println();
      Serial.println("Init seq. done - your login user and passwd are set and RFID-TAG is registered.\n");

      Serial.println("Set 2nd user? <y/n>");
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
        Serial.println("-> 2nd user not set\n");
      }

      stat_led_green();
      def_USER();
      Serial.println("Type 'help' for admin commands\n");
      delay(1300);
    }
    else
    {
      return;
    }
  }

  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 100);

  if (success)
  {
    if (uidLength == 4)
    {
      cardid = uid[0];
      cardid <<= 8;
      cardid |= uid[1];
      cardid <<= 8;
      cardid |= uid[2];  
      cardid <<= 8;
      cardid |= uid[3]; 
      entryTAG = cardid;
    }
  }
  else
  {
    entryTAG = "";
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

