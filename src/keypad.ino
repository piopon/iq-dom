#include <Keypad_I2C.h>

#define keypadAddress 0x21
const byte rows = 4;
const byte cols = 4;

char hexaKeys[rows][cols] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[rows] = {7, 6, 5, 4}; //connect to the row pinouts of the keypad
byte colPins[cols] = {0, 1, 2, 3}; //connect to the column pinouts of the keypad

Keypad_I2C myKeypad( makeKeymap(hexaKeys), rowPins, colPins, rows, cols, keypadAddress); 

void setUpKeypad(void) {
  myKeypad.begin(); 
}

char getKeypadKey(void) {
  char result = myKeypad.getKey();  
}
