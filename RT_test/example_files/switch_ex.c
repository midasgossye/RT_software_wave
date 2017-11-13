// 09September 2005
//******************************************************************************************************
// Performs basic I/O for the Omega PCI-DAS1602 
//
// Demonstration routine to demonstrate pci hardware programming
// Demonstrated the most basic DIO and ADC and DAC functions
// - excludes FIFO and strobed operations 
//
// Note :
//
//			22 Sept 2016 : Restructured to demonstrate Sine wave to DA
//
// G.Seet - 26 August 2005
//******************************************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <hw/pci.h>
#include <hw/inout.h>
#include <sys/neutrino.h>
#include <sys/mman.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>
																
#define	INTERRUPT		iobase[1] + 0				// Badr1 + 0 : also ADC register
#define	MUXCHAN			iobase[1] + 2				// Badr1 + 2
#define	TRIGGER			iobase[1] + 4				// Badr1 + 4
#define	AUTOCAL			iobase[1] + 6				// Badr1 + 6
#define 	DA_CTLREG		iobase[1] + 8				// Badr1 + 8

#define	 AD_DATA			iobase[2] + 0				// Badr2 + 0
#define	 AD_FIFOCLR		iobase[2] + 2				// Badr2 + 2

#define	TIMER0				iobase[3] + 0				// Badr3 + 0
#define	TIMER1				iobase[3] + 1				// Badr3 + 1
#define	TIMER2				iobase[3] + 2				// Badr3 + 2
#define	COUNTCTL			iobase[3] + 3				// Badr3 + 3
#define	DIO_PORTA		iobase[3] + 4				// Badr3 + 4
#define	DIO_PORTB		iobase[3] + 5				// Badr3 + 5
#define	DIO_PORTC		iobase[3] + 6				// Badr3 + 6
#define	DIO_CTLREG		iobase[3] + 7				// Badr3 + 7
#define	PACER1				iobase[3] + 8				// Badr3 + 8
#define	PACER2				iobase[3] + 9				// Badr3 + 9
#define	PACER3				iobase[3] + a				// Badr3 + a
#define	PACERCTL			iobase[3] + b				// Badr3 + b

#define 	DA_Data			iobase[4] + 0				// Badr4 + 0
#define	DA_FIFOCLR		iobase[4] + 2				// Badr4 + 2
	
int badr[5];															// PCI 2.2 assigns 6 IO base addresses
float frequency_gen = 1.0	;
int sleep_for;
float amplitude_value = 2.5;
float mean_value = 2.5;
unsigned short chan;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

int main() {
bool running = true;
int leds = 0x0;
int resolution = 50;
int msec;
clock_t start, diff;
struct pci_dev_info info;
void *hdl;

uintptr_t iobase[6];
uintptr_t dio_in;
uint16_t adc_in;
int output_x;


int i;
int j;
unsigned int count;
unsigned short chan;

unsigned int data[100];
float delta,dummy;

printf("\fDemonstration Routine for PCI-DAS 1602\n\n");

memset(&info,0,sizeof(info));
if(pci_attach(0)<0) {
  perror("pci_attach");
  exit(EXIT_FAILURE);
  }

																		/* Vendor and Device ID */
info.VendorId=0x1307;
info.DeviceId=0x01;

if ((hdl=pci_attach_device(0, PCI_SHARE|PCI_INIT_ALL, 0, &info))==0) {
  perror("pci_attach_device");
  exit(EXIT_FAILURE);
  }
  																		// Determine assigned BADRn IO addresses for PCI-DAS1602			

printf("\nDAS 1602 Base addresses:\n\n");
for(i=0;i<5;i++) {
  badr[i]=PCI_IO_ADDR(info.CpuBaseAddress[i]);
  printf("Badr[%d] : %x\n", i, badr[i]);
  }
 
printf("\nReconfirm Iobase:\n");  						// map I/O base address to user space						
for(i=0;i<5;i++) {												// expect CpuBaseAddress to be the same as iobase for PC
  iobase[i]=mmap_device_io(0x0f,badr[i]);	
  printf("Index %d : Address : %x ", i,badr[i]);
  printf("IOBASE  : %x \n",iobase[i]);
  }													
																		// Modify thread control privity
if(ThreadCtl(_NTO_TCTL_IO,0)==-1) {
  perror("Thread Control");
  exit(1);
  }																											
   													    //******************************************************************************
	
  														// Unreachable code
	out8(DIO_CTLREG,0x10);		//Sets PORTA as INPUT and PORTB and C as OUTPUTS								
	leds = 0x1;																		
	dio_in=in8(DIO_PORTC); printf("Port C : %02x\n",dio_in);	// Read back		
	
//******************************************************************************
// ADC Port Functions
//******************************************************************************
							// Initialise Board								
out16(INTERRUPT,0x60c0);				// sets interrupts	 - Clears			
out16(TRIGGER,0x2081);// sets trigger control: 10MHz, clear, Burst off,SW trig. default:20a0
out16(AUTOCAL,0x007f);		// sets automatic calibration : default

out16(AD_FIFOCLR,0); 		// clear ADC buffer
out16(MUXCHAN,0x0900);		// Write to MUX register-SW trigger,UP,DE,5v,ch 0-0 	

					// x x 0 0 | 1  0  0 1  | 0x 7   0 | Diff - 8 channels
					// SW trig |Diff-Uni 5v| scan 0-7| Single - 16 channels	
	count = 0;
	chan= ((count & 0x0f)<<4) | (0x0f & count);
	out16(MUXCHAN,0x0900|chan);

	while(running) {
	 		// Set channel	 - burst mode off.
  delay(1);					// allow mux to settle
  out16(AD_DATA,0); 				// start ADC 
  while(!(in16(MUXCHAN) & 0x4000));
  adc_in=in16(AD_DATA);   
  printf("ADC Chan: %02x Data [%3d]: %4x \n", chan, (int)count, (unsigned int)adc_in);							// print ADC													
  fflush( stdout );
  delay(5);			// Write to MUX register - SW trigger, UP, DE, 5v, ch 0-7 	
		dio_in=in8(DIO_PORTA); printf("Port A : %02x\n",dio_in);	// Read back
		if(dio_in > 0xf0) running = false;
		
		//if(adc_in < 0x1000) output_x = 0x1;
		//else if(adc_in < 0x2000) output_x = 0x2;
		//else if(adc_in < 0x3000) output_x = 0x4;
		//else if(adc_in > 0x3000) output_x= 0x8;
		printf("%d \n",adc_in);
		delay(5);																
		out8(DIO_PORTB,leds);						// Write to Port B  		
		
		
		leds*=2;
		if(leds > 0xf) leds = 0x1;

	
	}
}
