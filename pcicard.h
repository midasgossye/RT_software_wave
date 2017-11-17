/*
Wave Generator created by Midas Gosye, Jan Verheyen, Benyamin de Leeuw and Frederic Dupon

Part of the course MA4830: Realtime software for mechatronic systems
Professor Seet Gim Lee, Gerald
Nanyang Technological University, Singapore
*/

//Header file for setting up and using the pcicard and passing it to the main file
//as all the includes and defines would make a big mess in the main file.

#ifndef PCICARD_H_
#define PCICARD_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <hw/pci.h>
#include <hw/inout.h>
#include <sys/neutrino.h>
#include <sys/mman.h>

#define	INTERRUPT		iobase[1] + 0				// Badr1 + 0 : also ADC register
#define	MUXCHAN			iobase[1] + 2				// Badr1 + 2
#define	TRIGGER			iobase[1] + 4				// Badr1 + 4
#define	AUTOCAL			iobase[1] + 6				// Badr1 + 6
#define DA_CTLREG		iobase[1] + 8				// Badr1 + 8

#define AD_DATA			iobase[2] + 0				// Badr2 + 0
#define AD_FIFOCLR		iobase[2] + 2				// Badr2 + 2

#define	TIMER0			iobase[3] + 0				// Badr3 + 0
#define	TIMER1			iobase[3] + 1				// Badr3 + 1
#define	TIMER2			iobase[3] + 2				// Badr3 + 2
#define	COUNTCTL		iobase[3] + 3				// Badr3 + 3
#define	DIO_PORTA		iobase[3] + 4				// Badr3 + 4
#define	DIO_PORTB		iobase[3] + 5				// Badr3 + 5
#define	DIO_PORTC		iobase[3] + 6				// Badr3 + 6
#define	DIO_CTLREG		iobase[3] + 7				// Badr3 + 7
#define	PACER1			iobase[3] + 8				// Badr3 + 8
#define	PACER2			iobase[3] + 9				// Badr3 + 9
#define	PACER3			iobase[3] + a				// Badr3 + a
#define	PACERCTL		iobase[3] + b				// Badr3 + b

#define DA_Data			iobase[4] + 0				// Badr4 + 0
#define DA_FIFOCLR		iobase[4] + 2				// Badr4 + 2

#define	DEBUG						1

//call the necessary functions that are needed in the main program
extern void init_PCI_device(void);
extern uintptr_t read_switches(void);
extern void LED_out(uintptr_t LED_output);
extern uint16_t read_ADC(unsigned char count);
extern void write_DAC(unsigned int i, unsigned char DAC_chan);
extern void clean_up_PCI_device(void);

#endif
