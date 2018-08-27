#include "KpadI2C.h"

// Let the user define a keymap - assume the same row/column count as defined in constructor
void KpadI2C::begin(char *userKeymap) {
	Kpad::begin(userKeymap);
	//czyscimy bufory zapisu i odczytu
	I2C.rxBufferIndex = 0;
  I2C.rxBufferLength = 0;
  I2C.txBufferIndex = 0;
  I2C.txBufferLength = 0;
  //inicjalizacja TWI
  I2C.twi_init();
  //odczytujemy stan klawiszy
	pinState = pinState_set();
	i2cwidth = PCF8574;
}

//Inicjalizacja klawiatury 4x4 z I2C
void KpadI2C::begin(void) {
	//czyscimy bufory zapisu i odczytu
	I2C.rxBufferIndex = 0;
  I2C.rxBufferLength = 0;
  I2C.txBufferIndex = 0;
  I2C.txBufferLength = 0;
  //inicjalizacja TWI
  I2C.twi_init();
  //odczytujemy stan klawiszy
	pinState = pinState_set( );
	i2cwidth = PCF8574;
}

//zapis stanu klawisza
void KpadI2C::pin_write(byte pinNum, boolean level) {
	byte mask = 1<<pinNum;
	if( level == HIGH ) {
		pinState |= mask;
	} else {
		pinState &= ~mask;
	}
	port_write(pinState);
} 

//odczyt stanu klawisza
int KpadI2C::pin_read(byte pinNum) {
	byte pinVal,mask = 0x1<<pinNum;
	if(I2C.twi_readFrom(i2caddr,rxBuffer,i2cwidth,1)){
    //odczytujemy od pocz±tku bufora
    I2C.rxBufferIndex = 0;  
    I2C.rxBufferLength = i2cwidth;
    //wyznaczamy wartosc koncowa
    pinVal = I2C.twi_readBuffer();
  }
	pinVal &= mask;
	if( pinVal == mask ) {
		return 1;
	} else {
		return 0;
	}
}

//zapisz caly port
void KpadI2C::port_write(byte i2cportval) {
  // indicate that we are transmitting
  I2C.transmitting = 1;
  // set address of targeted slave
  I2C.txBufferIndex = 0;
  I2C.txBufferLength = 0;
  //uzupelnamy bufor do wyslania
  if(I2C.twi_writeBuffer(i2cportval & 0xFF)){
    //wysylamy dane z bufora
    I2C.txBufferIndex = 0;
    //wysylamt dane
    if(I2C.twi_writeTo(i2caddr,txBuffer,1,1)==0){
	    //czyscimy bufor po wyslaniu      
	    I2C.txBufferIndex = 0;    
	    I2C.txBufferLength = 0;
    } 
	}
  I2C.transmitting = 0;  
	pinState = i2cportval;
} 

//odczyt stanu pinow ekspandera
byte KpadI2C::pinState_set( ) {
	byte pinState;
	if(I2C.twi_readFrom(i2caddr,rxBuffer,i2cwidth,1)){
    //odczytujemy od pocz±tku bufora
    I2C.rxBufferIndex = 0;  
    I2C.rxBufferLength = i2cwidth;
    //wyznaczamy wartosc koncowa
    pinState = I2C.twi_readBuffer();
  }
	return pinState;
} 
