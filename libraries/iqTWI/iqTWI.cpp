#include "iqTWI.h"

//wartosc poczatkowa zmiennej statycznej
iqTWI* iqTWI::iterruptHandler = NULL;
//------------------------------------------------------------------------------
/*
 *  KONSTRUKTOR DOMY¦LNY
 */ 
iqTWI::iqTWI() {
  //niezbedne do obslugi przerwania
  iterruptHandler = this;
  //zerujemy bufor odbiorczy
  rxBuffer[TWI_BUFFER_LENGTH];
  rxBufferIndex = 0;
	rxBufferLength = 0;
  //zerujemy bufor nadawczy
	txAddress = 0;
	txBuffer[TWI_BUFFER_LENGTH];
	txBufferIndex = 0;
	txBufferLength = 0;
	//zerujemy zmienne pomocnicze
	transmitting = 0;
	twi_inRepStart = false;
}
//------------------------------------------------------------------------------
/*
 *  INICJALIZACJA MAGISTRALI
 */
void iqTWI::twi_init(void) {
   //w³±cz wewnêtrzne rezystory podci±gaj±ce 
   digitalWrite(SDA, 1);
   digitalWrite(SCL, 1); 
   //ustaw preskaler = 1
   TWSR &= ~(1 << TWPS0);
   TWSR &= ~(1 << TWPS1);
   //ustaw prêdko¶ci transmisji
   TWBR = ((F_CPU/TWI_FREQ)-16)/2;
   //w³±cz magistralê TWI
   TWCR = (1 << TWEN);
   //w³±cz przerwania 
   TWCR = (1 << TWIE);
   //w³±cz auto potwierdzenia
   TWCR = (1 << TWEA);
   //w³±cz zasilanie zegara TWI (bit 7 = 0)
   PRR0 &= ~(1 << PRTWI);
}
//------------------------------------------------------------------------------
/*
 *  WYSLANIE POLECENIA START TRANSMISJI
 */
byte iqTWI::twi_sendStart() {
  //sprawdzamy czy jest START czy RESTART
  if (twi_inRepStart == true) {
    //RESTART
    twi_inRepStart = false;	
    TWDR = twi_slarw;		
    TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWEN)  | _BV(TWIE);  
  } else {
    //START
    TWCR =_BV(TWINT) | _BV(TWEN) | _BV(TWEA)  | _BV(TWIE) | _BV(TWSTA);	
  }
  //po wyslaniu start przechodzimy do ICR
}
//------------------------------------------------------------------------------
/*
 *  SPRAWDZENIE BLEDU
 */
 byte iqTWI::twi_errorState(byte twi_error){
   //sprawdzamy stan flagi bledu
   if (twi_error == 0xFF) {
      //wszystko ok
      return 0;	
   } else if (twi_error == 0x20) {
      //wyslano adres otrzymano NACK
      return 1;	
   } else if (twi_error == 0x30) {
      //wyslano dane otrzymano NACK
      return 2;	
   } else {
      //inny blad
      return 3;	
   }
 }
//------------------------------------------------------------------------------
/*
 *  WYSLANIE DANYCH
 */
byte iqTWI::twi_writeTo(byte address, byte* data, byte length, byte sendStop){ 
	//czy ilosc danych zmiesci sie do bufora
  if(TWI_BUFFER_LENGTH<length) return -1;
  //czekamy na wolna linie (przerwanie)
  while(twi_state!=TWI_READY) continue;
  //uzupe³niamy zmienne wewnêtrzne
  twi_state    = TWI_MTX;
  twi_sendStop = sendStop;
  twi_error    = 0xFF;
  //inicjalizacja bufora danych Mastera
  mBufferIndex = 0;
  mBufferLength = length;
  //uzupelnienie bufora Mastera
  for(byte i=0; i<length; ++i){ 
    mBuffer[i] = data[i];
  }
	//budujemy pole adresowe oraz bit zapisu
	twi_slarw  = TW_WRITE;
	twi_slarw |= address<<1;
	//rozpoczynamy transmisje
	twi_sendStart();
	//czekamy wyslanie danych (przerwanie)
	while(twi_state == TWI_MTX){
	  continue;
	}
	//zwracamy status wyslania
	return twi_errorState(twi_error);

}
//------------------------------------------------------------------------------
/*
 *  PONOWNA PROBA WYSLANIA
 */
void iqTWI::twi_reply(uint8_t ack) {
    //kasowanie flagi TWINT w celu kontynuacji
    if(ack){
      //jest OK – ustawiamy bit ACK
      TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWINT)| _BV(TWEA);
    } else {
      //jest NOK – kasujemy bit ACK
      TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWINT);
    }
  }
//------------------------------------------------------------------------------
/*
 *  ODCZYTANIE DANYCH
 */
bool iqTWI::twi_readFrom(byte address, byte* data, byte length, byte sendStop){   
	//czy ilosc danych zmiesci sie do bufora
  if(TWI_BUFFER_LENGTH<length) return 0;
  //czekamy na wolna linie (przerwanie)
  while(twi_state!=TWI_READY) continue;
  //uzupe³niamy zmienne wewnêtrzne
  twi_state    = TWI_MRX;
  twi_sendStop = sendStop;
  twi_error    = 0xFF;
  //inicjalizacja bufora danych Mastera
  mBufferIndex = 0;
  mBufferLength = length-1;
  //budujemy pole adresowe oraz bit odczytu
  twi_slarw  = TW_READ;
  twi_slarw |= address<<1;
	//rozpoczynamy transmisje
	twi_sendStart();
	//czekamy wyslanie danych (przerwanie)
	while(twi_state == TWI_MRX){
		continue;
	}
	//uzupelnienie danych z bufora Mastera
	for(byte i=0; i<length; ++i){ 
		data[i] = mBuffer[i];
	}
	return twi_errorState(twi_error)==0;
} 
//------------------------------------------------------------------------------
/*
 *  ODCZYT DANYCH Z BUFORA
 */
int iqTWI::twi_readBuffer(void) {
  int result = -1;

  //kolejne wywolanie funkcji
  //oznacza odczytranie kolejnego bajtu
  if(rxBufferIndex < rxBufferLength){
    result = rxBuffer[rxBufferIndex];
    ++rxBufferIndex;
  }
  return result;
}
//------------------------------------------------------------------------------
/*
 *  ZAPIS DANYCH DO BUFORA
 */
bool iqTWI::twi_writeBuffer(byte data){
  boolean result = false;

  if(txBufferLength>=TWI_BUFFER_LENGTH){
    return false;
  }
  //uzupelniamy bufor danych 
  txBuffer[txBufferIndex++] = data;
  //uaktualniamy dlugosc bufora
  txBufferLength = txBufferIndex;
  return true;
}
 
//------------------------------------------------------------------------------
/*
 *  ZATRZYMANIE TRANSMISJI
 */
void iqTWI::twi_stop(void) {
  // wysylamy flage stop
  TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWEA) | _BV(TWINT) | _BV(TWSTO);
  //po poleceniu STOP nie ma przerwania
  //flaga TWINT kasowana jest automatycznie
  while(TWCR & _BV(TWSTO)){
    continue;
  }
  //uaktualniamy stan magistrali
  twi_state = TWI_READY;
}
//------------------------------------------------------------------------------
/*
 *  ZWALNIANIE MAGISTRALI
 */
void iqTWI::twi_releaseBus(void)
{
  //zwalniamy magistrale
  TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWEA) | _BV(TWINT);
  //ustawiamy stan logiczny na wolny
  twi_state = TWI_READY;
}
//------------------------------------------------------------------------------
/*
 *  OBSLUGA PRZERWANIA MAGISTRALI TWI
 */
void iqTWI::handleISR() {
  switch(TW_STATUS){
    case TW_START:     
    case TW_REP_START: 
      // po starcie wyslamy adres (z rejestru danych)
      TWDR = twi_slarw;
      twi_reply(1);
      break;
    case TW_MT_SLA_ACK:  
    case TW_MT_DATA_ACK: 
		 //czy jest cos do wyslania?
     if(mBufferIndex<mBufferLength){
       //sa dane – uzupelnij rejestr i wysylaj
       TWDR=mBuffer[mBufferIndex++];
       twi_reply(1);
     } else {
       // nic do wyslania
       if (twi_sendStop) {
         twi_stop();
       } else {
         twi_inRepStart = true;
         TWCR=_BV(TWINT)|_BV(TWSTA)|_BV(TWEN);
         twi_state = TWI_READY;
       }
     }
     break; 
    case TW_MT_SLA_NACK:  
      //adres wyslany – nie odebrano ACK
      twi_error = TW_MT_SLA_NACK;
      twi_stop();
      break;
    case TW_MT_DATA_NACK: 
      //dane wyslane – nie odebrano ACK
      twi_error = TW_MT_DATA_NACK;
      twi_stop();
      break;
    case TW_MT_ARB_LOST: 
      //brak
      twi_error = TW_MT_ARB_LOST;
      twi_releaseBus();
      break;
    case TW_MR_DATA_ACK: 
			//uzupelniamy bufor Mastera
			mBuffer[mBufferIndex++]=TWDR;
			//SPECJALNIE NIE DAJEMY break zeby wykonac ponizsze
    case TW_MR_SLA_ACK:  
			//spr czy sa jakies dane do wyslania
			if(mBufferIndex<mBufferLength){
				//wysylamy ACK
				twi_reply(1);
			} else {
				//wysylamy NACK
				twi_reply(0);
			}
			break;
    case TW_MR_DATA_NACK: 
			//uzupelniamy bufor ostatnim bajtem
			mBuffer[mBufferIndex++]=TWDR;
			//sprawdzamy czy konczyc transmisje
			if (twi_sendStop) {
			  twi_stop();
			} else {
			  //restart transmisji
			  twi_inRepStart = true;
			  TWCR=_BV(TWINT)|_BV(TWSTA)|_BV(TWEN);
			  twi_state = TWI_READY;
			}
			break;
    case TW_MR_SLA_NACK: 
			//adres wyslany – nie odebrano ACK
			twi_error = TW_MR_SLA_NACK;
			twi_stop();
			break;
    case TW_SR_SLA_ACK:   
    case TW_SR_GCALL_ACK: 
      // stan magistrali slave potwierdza swoj adres
      twi_state = TWI_SRX;
      rxBufferIndex = 0;
      twi_reply(1);
      break;
    case TW_SR_DATA_ACK:       
    case TW_SR_GCALL_DATA_ACK: 
      //sprawdzamy czy jest miejsce w buforze
      if(rxBufferIndex < TWI_BUFFER_LENGTH){
        //uzupelniamy bufor - wysyl ACK
        rxBuffer[rxBufferIndex++] = TWDR;
        twi_reply(1);
      } else {
        //brak miesca - NACK
        twi_reply(0);
      }
      break;
    case TW_SR_STOP: 
		  //otrzymano polecenie zakonczenia transmisji - "zamykamy" bufor NULLem
      if(rxBufferIndex < TWI_BUFFER_LENGTH){
        rxBuffer[rxBufferIndex] = '\0';
      }
      //zatrzymujemy transmisje, czyscimy bufor i zwalniamy magsitrale
      twi_stop();
      rxBufferIndex = 0;
      twi_releaseBus();
      break;
    case TW_SR_DATA_NACK:       
    case TW_SR_GCALL_DATA_NACK: 
      // brak ACK po odebraniu danych - wysylamy NACK
      twi_reply(0);
      break;
    case TW_NO_INFO:   
      //nieznany stan magistrali?
      break;
    case TW_BUS_ERROR: 
      //blad magistrali (najprawdopodobniej brak START/STOP)
      twi_error = TW_BUS_ERROR;
      twi_stop();
      break;
  }
}

ISR(TWI_vect) {
	iqTWI::TWIisr();
}