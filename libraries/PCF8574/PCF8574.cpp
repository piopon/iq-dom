#include "PCF8574.h"

PCF8574::PCF8574(){
	_pinsSet = 0; 
	_pinsGet = 0;
	_pinsDir = 0;
	_address = 0x00;
}

void PCF8574::begin(uint8_t address) {
  //zapamietaj adres w danej instancji
  this->_address = address;
  //inicjalizacja magistrali
  I2C.twi_init();
  //odczytujemy stan 8 pinow
  _pinsGet = readGPIO();
}

void PCF8574::pinMode(uint8_t pin, uint8_t mode) {
  switch (mode) {
    case INPUT:
      _pinsDir &= ~(1 << pin);
      _pinsSet &= ~(1 << pin);
      break;
    case INPUT_PULLUP:
      _pinsDir &= ~(1 << pin);
      _pinsSet |= (1 << pin);
      break;
    case OUTPUT:
      _pinsDir |= (1 << pin);
      _pinsSet &= ~(1 << pin);
      break;
    default:
      break;
  }
  //updateujemy ustawienia pinow
  if(updateGPIO()!=0) I2C.twi_stop();
}

void PCF8574::pinWrite(byte pin, byte v){
  //ustawiamy wartosc wybranego pinu
  if (v) {
    _pinsSet |= (1 << pin);
  } else {
    _pinsSet &= ~(1 << pin);
  }
  //updateujemy piny
  if(updateGPIO()!=0) I2C.twi_stop();
}


boolean PCF8574::pinRead(byte pin){
  //odczytujemy stan 8 pinow
  _pinsGet = readGPIO();
  //odczytujemy wartosc wybranego pinu
  return (_pinsGet&(1<<pin)) ? true:false;
}

byte PCF8574::readGPIO(void) {
  byte read = 1, pinResult = -1;
  //odczytujemy dane z magistrali 
  if(I2C.twi_readFrom(_address,I2C.rxBuffer,read,I2C.twi_sendStop)){
    //teraz w buforze mamy odebrane dane
    I2C.rxBufferIndex = 0;
    I2C.rxBufferLength = read;
    //odczytujemy ostatni bajt w buforze
    pinResult = I2C.twi_readBuffer();
  }
  return pinResult;
}

byte PCF8574::updateGPIO(void) {
  byte result, sendVal;
  sendVal=(_pinsGet & ~_pinsDir)|_pinsSet;
  //chcemy dodawac dane na poczatek bufora
  I2C.txBufferIndex = 0;
  I2C.txBufferLength = 0;
  //uzupelnamy bufor do wyslania
  if (I2C.twi_writeBuffer(sendVal)){
    //wysylamy dane z bufora
    result= I2C.twi_writeTo(_address,I2C.txBuffer,I2C.txBufferLength,1);
    //czyscimy bufor po wyslaniu      
    I2C.txBufferIndex = 0;
    I2C.txBufferLength = 0;
  }
  return result;
}

