#klonk: Optical fingerprint sensor

This version of klonk is using an optical fingerprint sensor for authentication. Authentication is done by placing your finger on the fingerprint sensor that has your fingerprints stored.
If authentication is successful klonk is unlocked and it will print "CNTRL-ALT-DEL" and stored password to unlock the Windows Desktop. To lock the Windows Desktop 
just place your finger again on the fingerprint sensor. klonk will be locked and print "CNTRL-ALT-DEL" and "RETURN" to lock the Windows Desktop. 

Once authenticated by matching your registered fingerprint the stored password can be printed by pressing a button. 

Pressing button short -> print stored password
Pressing and holding button longer than 1 second -> print stored user and password

A RGB-LED shows the operation status of klonk:

* RED: klonk is locked, authentication needed to unlock - printing of password or user/password is not working
* GREEN: klonk is unlocked, authenticated - printing of password or user/password by pressing button is working
* BLUE: klonk is in init mode e.g. initital setup of user and password or resetting passwords
* BLINK: klonk is not paired with your cell phone - only for bluetooth enabled version

There are commands available for administering klonk:

* help        list of commands
* passwd      change password
* pwgen       generate random password
* passwin     change windows password
* reset       reset user and password

passwd, pwgen and passwin are only working when klonk is unlocked. help and reset work in lock and unlocked mode. You have to enter your stored password before you can use reset command.

You can enter the above commands via serial terminal application or bluetooth terminal app:

* USB - via serial terminal application e.g. HypeTerminal, Arduino IDE serial monitor,...
* Bluetooth: via bluetooth terminal app of your cell phone - only for bluetooth enabled version

Bluetooth enabled klonk version also offers an automatic Windows Desktop logout. klonk and your cell phone is paired by building a connection with your bluetooth terminal app. klonk
will logout of Windows Desktop and enter locked mode when the bluetooth pairing is lost e.g. out of range. This is useful when you move away from your computer and forget to lock klonk.

The following directories contain firmware for the described versions of klonk:

* Keyboard_RFID_DE: USB serial
* Keyboard_RFID_DE_BT: USB serial, automatic lock and logout
* Keyboard_RFID_DE_BT_Serial: Bluetooth serial, automatic lock and logout

#Bill of Materials
* Pro Micro1: Sparkfun Pro Micro input voltage 5.0 V; Prozessor MK20DX256; Variante variant 2; i/o voltage 5.0 V (or compatible)
* Fingerprint: optical fingerprint sensor module for Arduino, 5V
* RGB-LED: APA-106, 5V
* Button: tactile push button switch
* Bluetooth: HC-05, voltage divider for HC-05 RX -> Pro Micro TX.

