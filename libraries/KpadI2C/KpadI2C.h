#ifndef KPADI2C_H
	#define KPADI2C_H
	#include "Kpad.h"
	#include <iqTWI.h>
	
	#define	PCF8574	1	// PCF8574 I/O expander device is 1 byte wide
	#define PCF8575 2	// PCF8575 I/O expander device is 2 bytes wide
	
	class KpadI2C : public Kpad, public iqTWI {
		public:
			KpadI2C(char* userKeymap, byte* row, byte* col, byte numRows, byte numCols, byte address, byte width = 1) :
			Kpad(userKeymap, row, col, numRows, numCols) { i2caddr = address; i2cwidth = width;}	
			// Keypad function
			void begin(char *userKeymap);
			// Wire function
			void begin(void);
			void pin_mode(byte pinNum, byte mode) {}
			void pin_write(byte pinNum, boolean level);
			int  pin_read(byte pinNum);
			// read initial value for pinState
			byte pinState_set( );
			// write a whole byte or word (depending on the port expander chip) to i2c port
			void port_write(byte i2cportval);
		private:
			iqTWI I2C;
			// I2C device address
			byte i2caddr;
			// I2C port expander device width in bytes (1 for 8574, 2 for 8575)
			byte i2cwidth;
			// I2C pin_write state persistant storage
			// least significant byte is used for 8-bit port expanders
			byte pinState;
	};
#endif // KPADI2C_H