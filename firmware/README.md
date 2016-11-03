#Firmware
This folder contains the software files for klonk.

There are several versions of klonk available - depending on the way of authentication, method to administer klonk and support for automatic logout.

Authentication:
- RFID/NFC
- Fingerprint sensor

Admiminstration:
- USB - via serial terminal application e.g. HypeTerminal, Arduino IDE serial monitor,...
- Bluetooth: via bluetooth terminal app of your cell phone

Automatic logout:
- Bluetooth: logout and lock klonk as soon as your paired klonk and cell mobile are out of range


The RFID-Library and examples for the MFRC522 board by Miguel (Miki) Balboa may be found here:
https://github.com/miguelbalboa/rfid

The fingerprint libray and examples can be found here:
https://github.com/adafruit/Adafruit-Fingerprint-Sensor-Library

The RGB-LED (APA-106) library and examples can be found here:
https://github.com/adafruit/Adafruit_NeoPixel
