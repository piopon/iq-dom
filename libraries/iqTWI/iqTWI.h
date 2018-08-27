#ifndef iqTWI_h  
 #define iqTWI_h
 #include <Arduino.h>
 //definiujemy predkosc transmisji 100kHz
 #define TWI_FREQ 100000L
 //wielkosc buffora = 32 bity
 #define TWI_BUFFER_LENGTH 32
 //definujemy logiczny stan magistrali
 #define TWI_READY 0
 #define TWI_MRX   1
 #define TWI_MTX   2
 #define TWI_SRX   3
 #define TWI_STX   4
 //definicje stanu magistrali
 #define TW_START		0x08
 #define TW_REP_START		0x10
 #define TW_MT_SLA_ACK		0x18
 #define TW_MT_SLA_NACK		0x20
 #define TW_MT_DATA_ACK		0x28
 #define TW_MT_DATA_NACK		0x30
 #define TW_MT_ARB_LOST		0x38
 #define TW_MR_ARB_LOST		0x38
 #define TW_MR_SLA_ACK		0x40
 #define TW_MR_SLA_NACK		0x48
 #define TW_MR_DATA_ACK		0x50
 #define TW_MR_DATA_NACK		0x58
 #define TW_SR_SLA_ACK		0x60
 #define TW_SR_GCALL_ACK		0x70
 #define TW_SR_DATA_ACK		0x80
 #define TW_SR_DATA_NACK		0x88
 #define TW_SR_GCALL_DATA_ACK	0x90
 #define TW_SR_GCALL_DATA_NACK	0x98
 #define TW_SR_STOP		0xA0
 #define TW_NO_INFO		0xF8
 #define TW_BUS_ERROR		0x00
 //definicje bitu R/W jako 8 w adresie
 #define TW_READ		1
 #define TW_WRITE	0
 //bajt statusu
 #define TW_STATUS_MASK	(_BV(TWS7)|_BV(TWS6)|_BV(TWS5)|_BV(TWS4)|_BV(TWS3))
 #define TW_STATUS		(TWSR & TW_STATUS_MASK)
 
 class iqTWI
 {
 protected:
 		//obsluga przerwania
		void handleISR();
  public:		
    //wskaznik na obiekt do obslugi przerwania
    static iqTWI* iterruptHandler;
    /************************************   ZMIENNE */
		//tymczasowy bufor odbiorczy
    byte rxBuffer[TWI_BUFFER_LENGTH];
    byte rxBufferIndex;
    byte rxBufferLength;
    //tymczasowy bufor nadawczy
    byte txAddress;
    byte txBuffer[TWI_BUFFER_LENGTH];
    byte txBufferIndex;
    byte txBufferLength;
    //bufor mastera
    uint8_t mBuffer[TWI_BUFFER_LENGTH];
  	volatile uint8_t mBufferIndex;
		volatile uint8_t mBufferLength;
		//zmienne pomocnicze
    uint8_t transmitting;
		volatile uint8_t twi_state;
		volatile uint8_t twi_slarw;
		volatile uint8_t twi_sendStop;			
		volatile uint8_t twi_inRepStart;
		volatile uint8_t twi_error;
		/************************************   FUNKCJE */
    iqTWI();
    //do obslugi przerwania
    static void TWIisr(){
			if(iterruptHandler){
				iterruptHandler->handleISR(); 
			}
		}
    void twi_init(void);
    byte twi_sendStart(void);
    byte twi_errorState(byte twi_error);
    byte twi_writeTo(byte address, byte* data, byte length, byte sendStop);
    void twi_reply(uint8_t ack);
	  bool twi_readFrom(byte address, byte* data, byte length, byte sendStop);
		int twi_readBuffer(void);
		bool twi_writeBuffer(byte data);	  
    void twi_stop(void);
    void twi_releaseBus(void);		
 };
 
 
#endif