
#include "silab.h"
#include "pcb01.h"
#include "avr/io.h"







#ifndef __GNUC__
// for CV
unsigned char data_r;
// SPI interrupt service routine
interrupt [SPI_STC] void spi_isr(void)
{
data_r=SPDR;
PORTB ^=(1<<1);
}
#else
//for gcc 
#include <avr/interrupt.h>
unsigned char data_r;
SIGNAL (SIG_SPI)
{
data_r=SPDR;
PORTB ^=(1<<1);
}
#endif


// Declare your global variables here

char SPI_MasterTransmit(int cData)
{
unsigned char low,hi;
low=(unsigned char) cData; cData = cData>>8; hi =(unsigned char) cData;
PORTB &= ~(1<<2);
SPDR = hi;
/* Wait for transmission complete */
while(!(SPSR & (1<<SPIF)));
SPDR = low;
/* Wait for transmission complete */
while(!(SPSR & (1<<SPIF)));
PORTB |= (1<<2);
return SPDR;
}


unsigned char SpiWriteRegister(unsigned char addres,unsigned char value)
{
addres |=0x80;
PORTB &= ~(1<<2);
SPDR = addres;
/* Wait for transmission complete */
while(!(SPSR & (1<<SPIF)));
SPDR = value;
/* Wait for transmission complete */
while(!(SPSR & (1<<SPIF)));
PORTB |= (1<<2);
return SPDR;
}


unsigned char SpiReadRegister(unsigned char addres)
{
PORTB &= ~(1<<2);
SPDR = addres;
/* Wait for transmission complete */
while(!(SPSR & (1<<SPIF)));
SPDR = 0x00;
/* Wait for transmission complete */
while(!(SPSR & (1<<SPIF)));
PORTB |= (1<<2);
return SPDR;
}




void send_rf(unsigned char Length)
{
unsigned char ItStatus1,ItStatus2;

//SW reset
SpiWriteRegister(0x07, 0x80);

//wait for chip ready interrupt from the radio (while the nIRQ pin is high)
loop_until_bit_is_set(PIND,3);
//read interrupt status registers to clear the interrupt flags and release NIRQ pin
ItStatus1 = SpiReadRegister(0x03); //read the Interrupt Status1 register
ItStatus2 = SpiReadRegister(0x04); //read the Interrupt Status2 register

/*set the physical parameters*/
//set the center frequency to 433 MHz
SpiWriteRegister(0x75, 0x53); //write 0x75 to the Frequency Band Select register
SpiWriteRegister(0x76, 0x4b); //write 0x4B to the Nominal Carrier Frequency1 register
SpiWriteRegister(0x77, 0x00); //write 0x80 to the Nominal Carrier Frequency0 register
//set the desired TX data rate (9.6kbps)
SpiWriteRegister(0x6E, 0x4E); //write 0x4E to the TXDataRate 1 register
SpiWriteRegister(0x6F, 0xA5); //write 0xA5 to the TXDataRate 0 register
SpiWriteRegister(0x70, 0x2C); //write 0x2C to the Modulation Mode Control 1 register
//set the desired TX deviation (+-45 kHz)
SpiWriteRegister(0x72, 0x48); //write 0x48 to the Frequency Deviation register
#ifdef ANTENNA_DIVERSITY
SpiWriteRegister(0x34, 0x14); //write 0x14 to the Preamble Length register
#else
SpiWriteRegister(0x34, 0x09); //write 0x09 to the Preamble Length register
#endif
//Disable header bytes; set variable packet length (the length of the payload is defined by the
//received packet length field of the packet); set the synch word to two bytes long
SpiWriteRegister(0x33, 0x02); //write 0x02 to the Header Control2 register +1
//Set the sync word pattern to 0x2DD4
SpiWriteRegister(0x36, 0x2D); //write 0x2D to the Sync Word 3 register
SpiWriteRegister(0x37, 0xD4); //write 0xD4 to the Sync Word 2 register

//enable the TX packet handler and disable CRC-16 (IBM) check
SpiWriteRegister(0x30, 0x08); //write 0x08 to the Data Access Control register

//enable the TX packet handler and CRC-16 (IBM) check
//SpiWriteRegister(0x30, 0x0D); //write 0x0D to the Data Access Control register (not used)

//enable FIFO mode and GFSK modulation
SpiWriteRegister(0x71, 0x63);//write 0x63 to the Modulation Mode Control 2 register

#ifdef ANTENNA_DIVERSITY
SpiWriteRegister(0x0C, 0x17); //write 0x17 to the GPIO1 Configuration(set the Antenna 1 Switch used for
antenna diversity )
SpiWriteRegister(0x0D, 0x18); //write 0x18 to the GPIO2 Configuration(set the Antenna 2 Switch used for
antenna diversity )
#endif
#ifdef ANTENNA_DIVERSITY
//enable the antenna diversity mode
SpiWriteRegister(0x08, 0x80); //write 0x80 to the Operating Function Control 2 register
#endif

//#define ONE_SMA_WITH_RF_SWITCH   (not used)

#ifdef ONE_SMA_WITH_RF_SWITCH
SpiWriteRegister(0x0C, 0x12);//write 0x12 to the GPIO1 Configuration(set the TX state)
SpiWriteRegister(0x0D, 0x15);//write 0x15 to the GPIO2 Configuration(set the RX state)
#endif

/*set the non-default Si4432 registers*/
//set VCO and PLL
//SpiWriteRegister(0x5A, 0x7F); //write 0x7F to the VCO Current Trimming register
//SpiWriteRegister(0x59, 0x40); //write 0x40 to the Divider Current Trimming register

//set VCO and PLL
//SpiWriteRegister(0x57, 0x01);//write 0x01 to the Chargepump Test register
//SpiWriteRegister(0x59, 0x40);//write 0x40 to the Divider Current Trimming register
//SpiWriteRegister(0x5A, 0x01); //write 0x01 to the VCO Current Trimming register
//set Crystal Oscillator Load Capacitance register
//SpiWriteRegister(0x09, 0xD7); //write 0xD7 to the CrystalOscillatorLoadCapacitance register



/*SET THE CONTENT OF THE PACKET*/
//set the length of the payload to 8bytes
SpiWriteRegister(0x08, 0x01);   //clear TX FIFO
SpiWriteRegister(0x08, 0x00);
SpiWriteRegister(0x3E, Length); //write 8 to the Transmit Packet Length register
SpiWriteRegister(0x7d, 1);      //TX FIFO Almost Empty Threshold.

//fill the payload into the transmit FIFO
SpiWriteRegister(0x7F, 0x42); //write 0x42 ('B') to the FIFO Access register


ItStatus1 = SpiReadRegister(0x03); //read the Interrupt Status1 register (sprawdzic )
ItStatus2 = SpiReadRegister(0x04); //read the Interrupt Status2 register (sprawdzic )
//printf("status1=%02x status2=%02x\n\r",ItStatus1,ItStatus2);

SpiWriteRegister(0x7F, 0x55); //write 0x55 ('U') to the FIFO Access register
SpiWriteRegister(0x7F, 0x54); //write 0x54 ('T') to the FIFO Access register
SpiWriteRegister(0x7F, 0x54); //write 0x54 ('T') to the FIFO Access register
SpiWriteRegister(0x7F, 0x4F); //write 0x4F ('O') to the FIFO Access register
SpiWriteRegister(0x7F, 0x4E); //write 0x4E ('N') to the FIFO Access register
SpiWriteRegister(0x7F, 0x31); //write 0x31 ('1') to the FIFO Access register
SpiWriteRegister(0x7F, 0x0D); //write 0x0D (CR) to the FIFO Access register

//Disable all other interrupts and enable the packet sent interrupt only.
//This will be used for indicating the successful packet transmission for the MCU
SpiWriteRegister(0x05, 0x04); //write 0x04 to the Interrupt Enable 1 register; Packet Sense Interrupt will be enabled.
SpiWriteRegister(0x06, 0x00); //write 0x03 to the Interrupt Enable 2 register; other int disabe
//Read interrupt status registers. It clear all pending interrupts and the nIRQ pin goes back to high.
ItStatus1 = SpiReadRegister(0x03); //read the Interrupt Status1 register
ItStatus2 = SpiReadRegister(0x04); //read the Interrupt Status2 register

//printf("status1=%02x status2=%02x\n\r",ItStatus1,ItStatus2);
/*enable transmitter*/
//The radio forms the packet and send it automatically.
SpiWriteRegister(0x07, 0x09); //write 0x09 to the Operating Function Control 1 register; TX on in Manual Transmit Mode.; READY Mode (Xtal is ON).
/*wait for the packet sent interrupt*/
//The MCU just needs to wait for the 'ipksent' interrupt.
loop_until_bit_is_set(PIND,3);
//read interrupt status registers to release the interrupt flags
ItStatus1 = SpiReadRegister(0x03); //read the Interrupt Status1 register
ItStatus2 = SpiReadRegister(0x04); //read the Interrupt Status2 register
}
