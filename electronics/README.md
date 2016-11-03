#Electronics 
This folder contains schematics and board files. To create these file we used Fritzing (http://www.fritzing.org/).
One need to add these Parts to the Library:
* MFRC522 (RFID-Board): https://github.com/miguelbalboa/rfid/tree/master/doc/fritzing
* ProMicro: https://github.com/unorthodox-engineers/unorthodox-arduino/tree/master/Unorthodox/fritzing/parts

##Files

###klonk.fzz
Fritzing Project File. Fritzing is an open-source hardware initiative that makes electronics accessible as a creative material for anyone. You may download the software form: http://fritzing.org/

###klonk_Breadboard.png
Exported breadboard view

###klonk_PCB.pdf / klonk_PCB.png
Exported PCB view

###klonk_Schematics.pdf / klonk_Schematics.png
Exported Schematics view

###klonk_copper.gtl / klonk_drill.txt
Exported Gerber (RS-274X) files. You may use them to order your fabricated PCB or if you prefer to mill your PCB yourself to generate gcode on: http://rapid-pcb.com/ . If you like to lear more about PCB milling start with this intro: http://blog.nyl.io/hello-world-milling-pcbs/

##Bill of Materials
* Pro Micro1: Sparkfun Pro Micro input voltage 5.0 V; Prozessor MK20DX256; Variante variant 2; i/o voltage 5.0 V (or compatible)
* RFID-Module: RFID-RC522-v2
* D1 + D2i: Rectifier Diode Paket 300 mil THT Typ 1N4001
* LED1: Green (565nm) LED Paket 5 mm THT
* R1: 220Ω Resistor THT Tolerance ±5%
