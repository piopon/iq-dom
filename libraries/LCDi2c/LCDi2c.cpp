#include "LCDi2c.h"

//KONSTRUKTOR 
LCDi2c::LCDi2c(uint8_t addr, uint8_t col,uint8_t row) {
  _address = addr;
  _cols = col;
  _rows = row;
}

//INICJALIZACJA     
void LCDi2c::init(void){
  //inicjalizujemy ekspander
  expander.begin(_address);
  //ustawiamy transmisje na 4 bitowa i wiecej niz 1 linia
  _displayfunction = LCD_4BITMODE | LCD_2LINE;
  //dokumentacja - czekanie min 40ms az zasilanie przekroczy 3V
  delay(50);
  //resetujemy ekspander (bity RS i RW na zero)
  expanderWrite(LCD_NOBACKLIGHT);	
  delay(1000);
  //przelaczamy sie z transmisji 8-bit na 4-bit
  //dokumentacja = musimy to powtorzyc trzykrotnie (!!)
  write4bits(0x03);
	delay(4); // czekaj 4ms
	write4bits(0x03);
	delay(4); 
	write4bits(0x03); 
	delay(2);
	write4bits(0x02);
	//wysylamy do sterownika jaki mamy wyswietlacz
	command(LCD_FUNCTIONSET | _displayfunction);
	//wyswietlacz ON, bez migajacego kursora
	_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
	//tryb wyswietlania (od lewej do prawej, bez przewijania)
	_displaycontrol = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
	command(LCD_ENTRYMODESET | _displaycontrol);
	//wlaczamy podswietlenie
	_backlightval=LCD_BACKLIGHT;
	expanderWrite(0);	
	//czyscimy wyswietlacz
	command(LCD_CLEARDISPLAY);// clear display, set cursor position to zero
	delayMicroseconds(2000);  // this command takes a long time!
}

void LCDi2c::setCursor(uint8_t col, uint8_t row){
	int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
	if ( row > _rows ) {
		row = _rows-1;    
	}
	command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

void LCDi2c::createChar(uint8_t location, uint8_t charmap[]) {
	location &= 0x7; // we only have 8 locations 0-7
	command(LCD_SETCGRAMADDR | (location << 3));
	for (int i=0; i<8; i++) {
		write(charmap[i]);
	}
}

// FUNKCJE NISKIEGO POZIOMU
inline void LCDi2c::command(uint8_t value) {
	send(value, 0);
}

inline size_t LCDi2c::write(uint8_t value) {
	send(value, Rs);
}

// write either command or data
void LCDi2c::send(uint8_t value, uint8_t mode) {
  //wyslanie ostatnich 4 bitow (maska 0xf0)
  write4bits((value & 0xf0) | mode);
  //wyslanie pierwszych 4 bitow (przesuwamy)
  write4bits(((value<<4)&0xf0) | mode); 

}

void LCDi2c::write4bits(uint8_t value) {
  //ustawiamy+wysylamy sygnaly w ekspanderze
  if (expanderWrite(value)==0){
    //pulsujemy bitem enable 
     //(zatwierdzenie danych)
    pulseEnable(value);
  } else {
    //stopujemy transmisje I2C
    I2C.twi_stop();
  }

}

byte LCDi2c::expanderWrite (byte dat) {
  byte result;
  //ustawiamy piny ekspandera zgodnie z danymi
  expander._pinsSet = dat | _backlightval;
  //wysylamy dane do PCF8574
  result = expander.updateGPIO();
  return result;
}

void LCDi2c::pulseEnable(byte data){
  //bit enable w stanie wysokim
  expanderWrite(data | En);
  //dlugosc impulsu enable >450ns
  delayMicroseconds(1);
  //bit enable w stanie niskim
  expanderWrite(data & ~En);
  //przesylanie komendy>37us
  delayMicroseconds(50);
}


