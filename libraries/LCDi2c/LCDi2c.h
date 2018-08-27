#ifndef LCDi2c_h
#define LCDi2c_h
  #include <Print.h>
  #include <iqTWI.h>
	#include <PCF8574.h>
	// komendy do sterownika HD44780
	#define LCD_CLEARDISPLAY 0x01
	#define LCD_RETURNHOME 0x02
	#define LCD_ENTRYMODESET 0x04
	#define LCD_DISPLAYCONTROL 0x08
	#define LCD_FUNCTIONSET 0x20
	#define LCD_SETCGRAMADDR 0x40
	#define LCD_SETDDRAMADDR 0x80
	// tryb wyswietlania (lewo - prawo)
	#define LCD_ENTRYRIGHT 0x00
	#define LCD_ENTRYLEFT 0x02
	#define LCD_ENTRYSHIFTINCREMENT 0x01
	#define LCD_ENTRYSHIFTDECREMENT 0x00	
	// kontrola wyswietlacza i podswietlenia
	#define LCD_DISPLAYON 0x04
	#define LCD_DISPLAYOFF 0x00
	#define LCD_CURSORON 0x02
	#define LCD_CURSOROFF 0x00
	#define LCD_BLINKON 0x01
	#define LCD_BLINKOFF 0x00
	#define LCD_BACKLIGHT 0x08
	#define LCD_NOBACKLIGHT 0x00
	// tryb pracy sterownika
	#define LCD_8BITMODE 0x10
	#define LCD_4BITMODE 0x00
	#define LCD_2LINE 0x08
	#define LCD_1LINE 0x00
	// bity kontrolne
	#define En B00000100  // bit ENA
	#define Rw B00000010  // bit RW
	#define Rs B00000001  // bit RS

  class LCDi2c : public Print {
    public: 
      LCDi2c(uint8_t addr, uint8_t col, uint8_t row);
      void init(void);
      void setCursor(uint8_t col, uint8_t row);
      void createChar(uint8_t location, uint8_t charmap[]);
    private:
      //funkcje nizszego poziomu
      inline void command(uint8_t value);
      inline size_t write(uint8_t value);
      void send(uint8_t value, uint8_t mode);
      void write4bits(uint8_t value);
      byte expanderWrite (byte dat);
      void pulseEnable(byte data);
      //zmienne pomocnicze
      PCF8574 expander;
      iqTWI I2C;
      uint8_t _address;
      uint8_t _cols;
      uint8_t _rows;
      uint8_t _backlightval;    
      uint8_t _displayfunction;
  		uint8_t _displaycontrol;      
  };

#endif

