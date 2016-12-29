/*
 * Handle serial IO
 */

#include <Arduino.h>
#include <Keyboard.h>
#include <WString.h>
#include <MFRC522.h>


#include "config.h"
#include "store.h"
#include "Klonk.h"


String readSerialString()
{
  if (Serial.available() > 0) {
    return Serial.readString();
  }
  
  return "";
}



String readSerialStringLen(int len)
{
  String str;

  do
  {
    str = readSerialString();
    if (str.length() > len)
    {
      return ""; 
    }
  } while (str.length() == 0);


  return str;
}


boolean chk_passwd(int id, RFID_TAG *tag_ptr)
{
  
}


boolean get_password(char *pwd) {
  String pw_str;

  memset(pwd, 0, PWD_MAX_LEN+1);
  Serial.println("Please enter new password: ");
  pw_str = readSerialStringLen(PWD_MAX_LEN);
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
  pw_str = readSerialStringLen(PWD_MAX_LEN);

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



void klonk_init()
{
  MetaData  meta = {.k_state = STATE_INIT, .lang = 0};
  UserData  user = {.uid = {0}, .pwd = {0}};
  RFID_TAG  tag = {.bytes = {0}, .len = 0};

  set_led(COLOR_BLUE);

  user.sum = get_user_crc32(&user);  
  for (int i = 0; i < MAX_USERS; i++)
  {
    store_user(i, &tag, &user);
  }
}



void klonk_chpw(int id, MFRC522 *rfid_ptr)
{
  
}



void klonk_pw_reset(int id, MFRC522 *rfid_ptr)
{
  RFID_TAG tag;
  char     uid[UID_MAX_LEN + 1];
  char     pwd[PWD_MAX_LEN + 1];
  UserData user;

  set_led(COLOR_BLUE);

  Serial.println("Reset user, password and RF-Tag");
  Serial.println("===============================");
  Serial.println("Please register tag:");

  while (! is_tag_available(rfid_ptr))
  {
    delay(100);
  }
  get_tag(rfid_ptr, &tag);
  Serial.println("Got Tag");

  set_led(COLOR_GREEN);

  do
  {
    Serial.print("Please enter new ");
    Serial.print(id + 1);
    Serial.println(" user: ");
    strncpy(uid, readSerialStringLen(UID_MAX_LEN).c_str(), UID_MAX_LEN);
  } while (strlen(uid) == 0);

  strncpy(user.uid, uid, UID_MAX_LEN); 
  if (get_password(pwd))
  {
    user.sum = get_user_crc32(&user);
    store_user(id, &tag, &user);
  }
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



void handle_serial_input(boolean lock, MFRC522 *rfid_ptr, RFID_TAG *tag_ptr)
{
  String command = readSerialString();

  if (command == 'passwd')
  {
    Serial.println("Changing existing password");
    Serial.println("==========================");

    if (lock)
    {
//      if (! chk_passwd(0, tag_ptr))
//      {
//        return;
//      }
    }

    klonk_chpw(0, rfid_ptr);
  }
  else if (command == 'pwgen')
  {
  }
  else if (command == 'passwin')
  {
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
    klonk_pw_reset(0, rfid_ptr); 
  }
  else if (command == 'init')
  {
    klonk_init();
    klonk_pw_reset(0, rfid_ptr); 
  }
  else if (command == 'help')
  {
    klonk_print_help();
  } else {
    
  }

}



/*void initialize_user_data() {
  delay(2000);
  set_led(COLOR_BLUE);
  
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
    set_led(COLOR_GREEN);
      
    Serial.print("Registered TAG: ");
    Serial.println(myTAG);
    Serial.println();
    Serial.println("Init seq. done - your login user and passwd are set and RFID-TAG is registered.\n");

    Serial.println("Set 2nd user? <y/n>");
    replySave = readSerialString();
        
    if (replySave == "y")
    {
      set_led(COLOR_ORANGE);
      setUSER(125,"second");
      PosPW = 75;
      chgPW();
    }
    else
    {
      Serial.println("-> 2nd user not set\n");
    }

    set_led(COLOR_GREEN);
    def_USER();
    Serial.println("Type 'help' for admin commands\n");
    delay(1300);
  }
  else
  {
    return;
  }
}
*/

