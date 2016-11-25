#Firmware
This folder contains the software files for klonk.

There are several versions of klonk available - depending on the way of authentication, method to administer klonk and support for automatic logout.

Authentication:
* RFID/NFC
* Fingerprint sensor

Administration:
* USB - via serial terminal application e.g. HypeTerminal, Arduino IDE serial monitor,...
* Bluetooth: via bluetooth terminal app of your cell phone

Automatic logout:
* Bluetooth: logout and lock klonk as soon as your paired klonk and cell mobile are out of range


The RFID-Library and examples for the MFRC522 board by Miguel (Miki) Balboa may be found here:
https://github.com/miguelbalboa/rfid

The optical fingerprint sensor libray and examples can be found here:
https://github.com/adafruit/Adafruit-Fingerprint-Sensor-Library

The RGB-LED (APA-106) library and examples can be found here:
https://github.com/adafruit/Adafruit_NeoPixel

##Version

The following directories contain firmware for several versions of klonk:

* klonk_de_fp: klonk with optical fingerprint reader sensor authentication
* klonk_de_rfid_pn532: klonk with RFID/NFC PN532 authenticaion (MIFARE)
* klonk_de_rfid_rc522: klonk with RFID RC522 authentication (MIFARE)
* klonk_de_reset: Initial eeprom memory setup of Arduino Pro Micro (ATmega32U4) clone
