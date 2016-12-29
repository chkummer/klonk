#ifndef SERIAL_UI_H
#define SERIAL_UI_H 1

String readSerialString();
String readSerialStringLen(int len);




void klonk_init();
void klonk_pw_reset(int id, MFRC522 *rfid_ptr);

void handle_serial_input(boolean LOCK, MFRC522 *rfid_ptr, RFID_TAG *tag_ptr);

#endif
