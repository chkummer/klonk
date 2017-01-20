#ifndef CONFIG_H
#define CONFIG_H 1

#define DEBUG 0

#define BOARD 1         // Possible values 1=BCP, 2=breadboard
#define LED   5         // LED Type: 5=RGB APA-106, F5;  8=RGB APA-106, F8
#define TOKEN MFRC522

#define PWD_MAX_LEN 20
#define UID_MAX_LEN 10
#define TAG_LEN     10

#define MAX_USERS 2

#define PRESSED LOW

#define LONG_PRESS  500
#define LANG_PRESS 2500

#endif
