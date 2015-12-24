/*
 * CC1101.c
 *
 *  Created on: Dec 21, 2015
 *      Author: adamoam
 */

/*
 * for Arduino chips the function "digitalWrite(pin, value)"
 * refers to setting the output pin as a HIGH or LOW voltage (3.3 or 5v)
 * the function "digitalRead(pin)"
 * refers to reading the input pin for a HIGH or LOW voltage (3.3 or 5v)
 *
 * We will need to find the equivalent function on our Nordic Boards
 * for most if not all of the following functions.
 *
 * Until we know the proper function, I will keep the arduino syntax
 *
 * Pretty much all of the values to be sent from one function to another must
 * be in BYTE formatit is easiest to ensure this by writing values in hex
 * ie. 0x21 = 33
 *
 */
#include "CC1101.h"




int PaTable[8] = {0x60 ,0x60 ,0x60 ,0x60 ,0x60 ,0x60 ,0x60 ,0x60};

/****************************************************************
*FUNCTION NAME:Init
*FUNCTION     :CC1101 initialization
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void Init(void)
{
	SpiInit();										//spi initialization
	digitalWrite(SS_PIN, HIGH);
	digitalWrite(SCK_PIN, HIGH);
	digitalWrite(MOSI_PIN, LOW);
	Reset();										//CC1101 reset
	RegConfigSettings();							//CC1101 register config
	SpiWriteBurstReg(_PATABLE,PaTable,8);			//CC1101 PATABLE config
}



/*
 * the SpiInit() will be handled once we figure out SPI with the Nordic boards
 */

/****************************************************************
*FUNCTION NAME:SpiInit
*FUNCTION     :SPI communication initialization
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void SpiInit(void)
{

	// Initialize SCK_PIN  	for output
	// Initialize MOSI_PIN 	for output
	// Initialize MISO_PIN 	for output
	// Initialize SS_PIN   	for output
	// Initialize GDO0 		for input
	// Initialize GDO2 		for input

}



/****************************************************************
*FUNCTION NAME:Reset
*FUNCTION     :CC1101 reset
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void Reset (void)
{
	//delay(milliseconds)

	digitalWrite(SS_PIN, LOW);
	delay(1);
	digitalWrite(SS_PIN, HIGH);
	delay(1);
	digitalWrite(SS_PIN, LOW);

	while(digitalRead(MISO_PIN)); 	// wait for status (MISO) pin to be low
	SpiTransfer(_SRES); 	  		// write the SRES command strobe
	while(digitalRead(MISO_PIN));	// wait for status (MISO) pin to be low
	digitalWrite(SS_PIN, HIGH);
}



/*
 *
 * I have not been able to understand the process of SpiMode() and SpiTransfer().
 * I believe that Ray, having more of a handle with SPI protocols, will be
 * better suited to look at them and see what they are doing exactly and
 * write comments to describe the process.
 *
 */

/****************************************************************
*FUNCTION NAME:SpiMode
*FUNCTION     :set spi mode
*INPUT        :        config               mode
			   (0<<CPOL) | (0 << CPHA)		 0
			   (0<<CPOL) | (1 << CPHA)		 1
			   (1<<CPOL) | (0 << CPHA)		 2
			   (1<<CPOL) | (1 << CPHA)		 3
*OUTPUT       :none
****************************************************************/
void SpiMode(int config)
{
  int tmp;

  // enable SPI master with configuration byte specified
  SPCR = 0;
  SPCR = (config & 0x7F) | (1<<SPE) | (1<<MSTR);
  tmp = SPSR;
  tmp = SPDR;
}



/****************************************************************
*FUNCTION NAME:SpiTransfer
*FUNCTION     :spi transfer
*INPUT        :value: data to send
*OUTPUT       :data to receive
****************************************************************/
int SpiTransfer(int value)
{
  SPDR = value;
  while (!(SPSR & (1<<SPIF))) ;
  return SPDR;
}



/****************************************************************
*FUNCTION NAME:SpiStrobe
*FUNCTION     :CC1101 Strobe
*INPUT        :strobe: command; //refer to const values in CC1101.h//
*OUTPUT       :none
****************************************************************/
void SpiStrobe(int strobe)
{
	digitalWrite(SS_PIN, LOW); 		// tell CC1101 you are about to WRITE
	while(digitalRead(MISO_PIN));	// wait for it to accept data
	SpiTransfer(strobe);			// send the strobe you want
	digitalWrite(SS_PIN, HIGH);		// inform that the WRITE process is over
}



/****************************************************************
*FUNCTION NAME:SpiWriteReg
*FUNCTION     :CC1101 write data to register
*INPUT        :addr: register address; value: register value
*OUTPUT       :none
****************************************************************/
void SpiWriteReg(int addr, int value)
{
	digitalWrite(SS_PIN, LOW);		// tell CC1101 you are about to WRITE
	while(digitalRead(MISO_PIN));	// wait for it to accept data
	SpiTransfer(addr);				// write the address of register
	SpiTransfer(value);				// write the byte you want to store
	digitalWrite(SS_PIN, HIGH);		// inform that the WRITE process is over
}



/*
 * WriteBurst is necessary to increase data rate transmission as well as reduce
 * current consumption and likelihood of collisions with systems at similar frequency.
 * See the CC1101 data sheet for more info.
 */
/****************************************************************
*FUNCTION NAME:SpiWriteBurstReg
*FUNCTION     :CC1101 write burst data to register
*INPUT        :addr: register address; buffer:register value array; num:number to write
*OUTPUT       :none
****************************************************************/
void SpiWriteBurstReg(int addr, int *buffer, int num)
{
	int i, temp;

	temp = addr | WRITE_BURST;		// binary OR the address with the WRITE_BURST value
    digitalWrite(SS_PIN, LOW);		// tell CC1101 you are about to WRITE
    while(digitalRead(MISO_PIN));	// wait for it to accept data
    SpiTransfer(temp);				// write the address of register
    for (i = 0; i < num; i++)
 	{								// begin the for loop to write specific bits
        SpiTransfer(buffer[i]);		// into the address for a specified number of bits
    }								// from a "buffer" byte
    digitalWrite(SS_PIN, HIGH);		// tell CC1101 that the WRITE process is over
}



/****************************************************************
*FUNCTION NAME:SpiReadReg
*FUNCTION     :CC1101 read data from register
*INPUT        :addr: register address
*OUTPUT       :register value
****************************************************************/
int SpiReadReg(int addr)
{
	int temp, value;

    temp = addr|READ_SINGLE;		// binary OR the address with READ_SINGLE value
	digitalWrite(SS_PIN, LOW); 		// tell CC1101 you are about to READ
	while(digitalRead(MISO_PIN));	// wait for it to accept data
	SpiTransfer(temp);				// send SpiTransfer an empty byte
	value=SpiTransfer(0);			// set int value to the SpiTransfer "read" value
	digitalWrite(SS_PIN, HIGH);		// tell CC1101 that the READ process is over

	return value;					// return the byte you read from the register
}



/****************************************************************
*FUNCTION NAME:SpiReadBurstReg
*FUNCTION     :CC1101 read burst data from register
*INPUT        :addr: register address; buffer:array to store register value; num: number to read
*OUTPUT       :none
****************************************************************/
void SpiReadBurstReg(int addr, int *buffer, int num)
{
	int i,temp;

	temp = addr | READ_BURST;		// binary OR the address with READ_BURST value
	digitalWrite(SS_PIN, LOW);		// tell CC1101 you are about to READ
	while(digitalRead(MISO_PIN));	// wait for it to accept data
	SpiTransfer(temp);				// send SpiTransfer an empty or NULL byte
	for(i=0;i<num;i++)
	{								// begin the for loop to read specific bytes
		buffer[i]=SpiTransfer(0);	// from the address for a specified number of bits
	}								// from a "buffer" byte
	digitalWrite(SS_PIN, HIGH);		// tell CC1101 that the READ process is over
}



/****************************************************************
*FUNCTION NAME:SpiReadStatus
*FUNCTION     :CC1101 read status register
*INPUT        :addr: register address
*OUTPUT       :status value
****************************************************************/
byte SpiReadStatus(int addr)
{
	int value,temp;

	temp = addr | READ_BURST;		// binary OR the address with READ_BURST value
	digitalWrite(SS_PIN, LOW);		// tell CC1101 you are about to READ
	while(digitalRead(MISO_PIN));	// wait for it to accept data
	SpiTransfer(temp);				// send SpiTransfer an empty or NULL byte
	value=SpiTransfer(0);			// set value to the read byte from the address
	digitalWrite(SS_PIN, HIGH);		// tell CC1101 that the READ process is over

	return value;					// return the status byte you read from the register
}



/****************************************************************
*FUNCTION NAME:SendDataPACKET
*FUNCTION     :use CC1101 send a data Packet
*INPUT        :txBuffer: data array to send; size: number of bytes to send, no more than 61
*OUTPUT       :none
****************************************************************/
void SendDataPacket(int *txBuffer,int size)
{
	SpiWriteReg(_TXFIFO,size);
	SpiWriteBurstReg(_TXFIFO,txBuffer,size);			// Write data to send
	SpiStrobe(_STX);									// Start send
    while (!digitalRead(GDO0));							// Wait for GDO0 to be set -> sync transmitted
    while (digitalRead(GDO0));							// Wait for GDO0 to be cleared -> end of packet
	SpiStrobe(_SFTX);									// Flush TXfifo
}



/****************************************************************
*FUNCTION NAME:ReceiveDataPacket
*FUNCTION     :read data received from RXfifo
*INPUT        :rxBuffer: buffer to store data
*OUTPUT       :size of data received
****************************************************************/
int ReceiveDataPacket(int *rxBuffer)
{
	int size;
	int status[2];

	if(SpiReadStatus(_RXBYTES) & BYTES_IN_RXFIFO)	// Check that RXFIFO not empty
	{
		size=SpiReadReg(RXFIFO);					// Set size to the first byte in RXFIFO
		SpiReadBurstReg(RXFIFO,rxBuffer,size);		// Read from RX FIFO Write to the rxBuffer
		SpiReadBurstReg(RXFIFO,status,2);			// read the status
		SpiStrobe(_SFRX);							// Flush the RXFIFO
		return size;								// return the number of bytes received
	}
	else
	{
		SpiStrobe(_SFRX);							// If RXFIFO is empty, flush the FIFO
		return 0;									// Return 0 bytes received
	}

}



/****************************************************************
*FUNCTION NAME:CheckReceiveFlag
*FUNCTION     :check receive data or not
*INPUT        :none
*OUTPUT       :flag: 0 no data; 1 receive data
****************************************************************/
int CheckReceiveFlag(void)
{
	if(digitalRead(GDO0))			//receive data
	{
		while (digitalRead(GDO0));
		return 1;
	}
	else							// no data
	{
		return 0;
	}
}

/*
 * RF values need to be changed in CC1101.h
 * They are currently the default values from the Arduino example.
 */

/****************************************************************
*FUNCTION NAME:RegConfigSettings
*FUNCTION     :CC1101 register config
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void RegConfigSettings(void)
{
    SpiWriteReg(_FSCTRL1,  RF_FSCTRL1);
    SpiWriteReg(_FSCTRL0,  RF_FSCTRL0);
    SpiWriteReg(_FREQ2,    RF_FREQ2);
    SpiWriteReg(_FREQ1,    RF_FREQ1);
    SpiWriteReg(_FREQ0,    RF_FREQ0);
    SpiWriteReg(_MDMCFG4,  RF_MDMCFG4);
    SpiWriteReg(_MDMCFG3,  RF_MDMCFG3);
    SpiWriteReg(_MDMCFG2,  RF_MDMCFG2);
    SpiWriteReg(_MDMCFG1,  RF_MDMCFG1);
    SpiWriteReg(_MDMCFG0,  RF_MDMCFG0);
    SpiWriteReg(_CHANNR,   RF_CHANNR);
    SpiWriteReg(_DEVIATN,  RF_DEVIATN);
    SpiWriteReg(_FREND1,   RF_FREND1);
    SpiWriteReg(_FREND0,   RF_FREND0);
    SpiWriteReg(_MCSM0 ,   RD_MCSM0);
    SpiWriteReg(_FOCCFG,   RF_FOCCFG);
    SpiWriteReg(_BSCFG,    RF_BSCFG);
    SpiWriteReg(_AGCCTRL2, RF_AGCCTRL2);
	SpiWriteReg(_AGCCTRL1, RF_AGCCTRL1);
    SpiWriteReg(_AGCCTRL0, RF_AGCCTRL0);
    SpiWriteReg(_FSCAL3,   RF_FSCAL3);
	SpiWriteReg(_FSCAL2,   RF_FSCAL2);
	SpiWriteReg(_FSCAL1,   RF_FSCAL1);
    SpiWriteReg(_FSCAL0,   RF_FSCAL0);
    SpiWriteReg(_FSTEST,   RF_FSTEST);
    SpiWriteReg(_TEST2,    RF_TEST2);
    SpiWriteReg(_TEST1,    RF_TEST1);
    SpiWriteReg(_TEST0,    RF_TEST0);
    SpiWriteReg(_IOCFG2,   RF_IOCFG2); 		//serial clock.synchronous to the data in synchronous serial mode
    SpiWriteReg(_IOCFG0,   RF_IOCFG0);  	//asserts when sync word has been sent/received, and de-asserts at the end of the packet
    SpiWriteReg(_PKTCTRL1, RF_PKTCTRL1);	//two status bytes will be appended to the payload of the packet,including RSSI LQI and CRC OK
											//No address check
    SpiWriteReg(_PKTCTRL0, RF_PKTCTRL0);	//whitening off;CRC Enable£»variable length packets, packet length configured by the first byte after sync word
    SpiWriteReg(_ADDR,     RF_ADDR);		//address used for packet filtration.
    SpiWriteReg(_PKTLEN,   RF_PKTLEN); 		//61 bytes max length
}














