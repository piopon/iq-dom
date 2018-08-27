#include "RTClib.h"
#include <avr/pgmspace.h>

////////////////////////////////////////////////////////////////////////////////
// DateTime implementation - ignores time zones and DST changes
DateTime::DateTime (uint32_t t) {
  t -= SECONDS_FROM_1970_TO_2000;    // bring to 2000 timestamp from 1970

    ss = t % 60;
    t /= 60;
    mm = t % 60;
    t /= 60;
    hh = t % 24;
    uint16_t days = t / 24;
    uint8_t leap;
    for (yOff = 0; ; ++yOff) {
        leap = yOff % 4 == 0;
        if (days < 365 + leap)
            break;
        days -= 365 + leap;
    }
    for (m = 1; ; ++m) {
        uint8_t daysPerMonth = pgm_read_byte(daysInMonth + m - 1);
        if (leap && m == 2)
            ++daysPerMonth;
        if (days < daysPerMonth)
            break;
        days -= daysPerMonth;
    }
    d = days + 1;
}

DateTime::DateTime (uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec) {
    if (year >= 2000) year -= 2000;
    yOff = year;
    m = month;
    d = day;
    hh = hour;
    mm = min;
    ss = sec;
}

DateTime::DateTime (const DateTime& copy):
  yOff(copy.yOff),
  m(copy.m),
  d(copy.d),
  hh(copy.hh),
  mm(copy.mm),
  ss(copy.ss)
{}

static uint8_t conv2d(const char* p) {
    uint8_t v = 0;
    if ('0' <= *p && *p <= '9')
        v = *p - '0';
    return 10 * v + *++p - '0';
}

// A convenient constructor for using "the compiler's time":
// This version will save RAM by using PROGMEM to store it by using the F macro.
//   DateTime now (F(__DATE__), F(__TIME__));
DateTime::DateTime (const __FlashStringHelper* date, const __FlashStringHelper* time) {
    // sample input: date = "Dec 26 2009", time = "12:34:56"
    char buff[11];
    memcpy_P(buff, date, 11);
    yOff = conv2d(buff + 9);
    // Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec
    switch (buff[0]) {
        case 'J': m = buff[1] == 'a' ? 1 : m = buff[2] == 'n' ? 6 : 7; break;
        case 'F': m = 2; break;
        case 'A': m = buff[2] == 'r' ? 4 : 8; break;
        case 'M': m = buff[2] == 'r' ? 3 : 5; break;
        case 'S': m = 9; break;
        case 'O': m = 10; break;
        case 'N': m = 11; break;
        case 'D': m = 12; break;
    }
    d = conv2d(buff + 4);
    memcpy_P(buff, time, 8);
    hh = conv2d(buff);
    mm = conv2d(buff + 3);
    ss = conv2d(buff + 6);
}

////////////////////////////////////////////////////////////////////////////////
// RTC_DS1307 implementation

static uint8_t bcd2bin (uint8_t val) { return val - 6 * (val >> 4); }
static uint8_t bin2bcd (uint8_t val) { return val + 6 * (val / 10); }

uint8_t RTC_DS1307::begin(void) {
  return 1;
}

uint8_t RTC_DS1307::isrunning(void) {
  uint8_t result;
	//ustawiamy flage ze bedziemy nadawac
  I2C.transmitting = 1;
  //czyscimy bufory wysylu
  I2C.txBufferIndex = 0;
  I2C.txBufferLength = 0;
  //uzupelniamy bufory
  if(I2C.twi_writeBuffer(0)) {
    //przesylamy dane do odbiorcy i czyscimy bufor
	  result = I2C.twi_writeTo(DS1307_ADDRESS, I2C.txBuffer, I2C.txBufferLength, (byte)1);
	  I2C.txBufferIndex = 0;
	  I2C.txBufferLength = 0;
	  //zerujemy flage transmisji
	  I2C.transmitting = 0;
  }
  //czekamy na odpowiedz
  if(I2C.twi_readFrom(DS1307_ADDRESS,I2C.rxBuffer,1,true)){
    //teraz w buforze mamy odebrane dane
    I2C.rxBufferIndex = 0;
    I2C.rxBufferLength = 1;
    //odczytujemy dane
    result = I2C.twi_readBuffer();
  }
  return !(result>>7);
}

void RTC_DS1307::adjust(const DateTime& dt) {
	//ustawiamy flage ze bedziemy nadawac
  I2C.transmitting = 1;
  //czyscimy bufory wysylu
  I2C.txBufferIndex = 0;
  I2C.txBufferLength = 0;
  //uzupelniamy bufory
  if(I2C.twi_writeBuffer(0) && I2C.twi_writeBuffer(bin2bcd(dt.minute())) && I2C.twi_writeBuffer(bin2bcd(dt.hour()))
	   												&& I2C.twi_writeBuffer(bin2bcd(0)) && I2C.twi_writeBuffer(0)) {
    //przesylamy dane do odbiorcy i czyscimy bufor
	  I2C.twi_writeTo(DS1307_ADDRESS, I2C.txBuffer, I2C.txBufferLength, (byte)1);
	  I2C.txBufferIndex = 0;
	  I2C.txBufferLength = 0;
	  //zerujemy flage transmisji
	  I2C.transmitting = 0;
  }
}

DateTime RTC_DS1307::now() {
	uint8_t ss,mm,hh,d,m,y;
	//ustawiamy flage ze bedziemy nadawac
  I2C.transmitting = 1;
  //czyscimy bufory wysylu
  I2C.txBufferIndex = 0;
  I2C.txBufferLength = 0;
  //uzupelniamy bufory
  if(I2C.twi_writeBuffer(0)) {
    //przesylamy dane do odbiorcy i czyscimy bufor
	  if (I2C.twi_writeTo(DS1307_ADDRESS, I2C.txBuffer, I2C.txBufferLength, (byte)1)==0) {
		  I2C.txBufferIndex = 0;
		  I2C.txBufferLength = 0;
	  }
	  //zerujemy flage transmisji
	  I2C.transmitting = 0;	  
  }
  //czekamy na odpowiedz
  if(I2C.twi_readFrom(DS1307_ADDRESS,I2C.rxBuffer,7,true)){
    //teraz w buforze mamy odebrane dane
    I2C.rxBufferIndex = 0;
    I2C.rxBufferLength = 7;
    //odczytujemy dokladna godzine
		ss = bcd2bin(I2C.twi_readBuffer() & 0x7F);
		mm = bcd2bin(I2C.twi_readBuffer());
		hh = bcd2bin(I2C.twi_readBuffer());
		//tutaj sa jakies smieci
		d = I2C.twi_readBuffer();
		//teraz odczytujemy date
		d = bcd2bin(I2C.twi_readBuffer());
		m = bcd2bin(I2C.twi_readBuffer());
		y = bcd2bin(I2C.twi_readBuffer()) + 2000;
  }
  return DateTime (y, m, d, hh, mm, ss);
}