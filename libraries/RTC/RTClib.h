// Code by JeeLabs http://news.jeelabs.org/code/
// Released to the public domain! Enjoy!

#ifndef _RTCLIB_H_
	#define _RTCLIB_H_
	#include <iqTWI.h>
	#include <Arduino.h>
	
	#define DS1307_CONTROL  0x07
	#define DS1307_NVRAM    0x08
	#define SECONDS_PER_DAY 86400L
	#define SECONDS_FROM_1970_TO_2000 946684800
	
	const byte DS1307_ADDRESS = 0x68;
  const uint8_t daysInMonth [] PROGMEM = { 31,28,31,30,31,30,31,31,30,31,30,31 };

	
	class DateTime {
	public:
	    DateTime (uint32_t t=0);
	    DateTime (uint16_t year, uint8_t month, uint8_t day,
	                uint8_t hour =0, uint8_t min =0, uint8_t sec =0);
	    DateTime (const DateTime& copy);
	    DateTime (const char* date, const char* time);
	    DateTime (const __FlashStringHelper* date, const __FlashStringHelper* time);
	    uint8_t hour() const        { return hh; }
	    uint8_t minute() const      { return mm; }
	protected:
	    uint8_t yOff, m, d, hh, mm, ss;
	};
	
	
	class RTC_DS1307 {
	public:
	    static uint8_t begin(void);
	    void adjust(const DateTime& dt);
	    uint8_t isrunning(void);
	    DateTime now();
	private:
		  iqTWI I2C;
	};

#endif // _RTCLIB_H_
