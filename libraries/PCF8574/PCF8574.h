#ifndef PCF8574_H
	#define PCF8574_H
  #include <iqTWI.h>
  
	class PCF8574 {
		public:
			PCF8574();
			void begin(byte address);
			void pinMode(byte pin, byte mode);
			void pinWrite(byte pin, byte v);
			boolean pinRead(byte pin);
			byte readGPIO(void);
			byte updateGPIO(void);
		  iqTWI I2C;
			//stan pinow do ustawienia (PORT)
			volatile byte _pinsSet;
			//stan odczytanych pinow (PIN)
			volatile byte _pinsGet;
			//kierunek pinow do ustawienia (DDR)
			volatile byte _pinsDir;
		  // adres urzadzenia
			byte _address;
	};
#endif