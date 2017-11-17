/*
Wave Generator created by Midas Gosye, Jan Verheyen, Benyamin de Leeuw and Frederic Dupon

Part of the course MA4830: Realtime software for mechatronic systems
Professor Seet Gim Lee, Gerald
Nanyang Technological University, Singapore
*/

#include "pcicard.h"

static void *hdl;               // MUST be global variable
static uintptr_t iobase[6];     // MUST be global variable

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void init_PCI_device(void){
    unsigned short i;
    int badr[5];								// PCI 2.2 assigns 6 IO base addresses
    struct pci_dev_info info;
    memset(&info,0,sizeof(info));
    if(pci_attach(0)<0){
        perror("pci_attach");
        exit(EXIT_FAILURE);
    }

    /* Vendor and Device ID */
    info.VendorId=0x1307;
    info.DeviceId=0x01;

    if ((hdl=pci_attach_device(0, PCI_SHARE|PCI_INIT_ALL, 0, &info))==0){
        perror("pci_attach_device");
        exit(EXIT_FAILURE);
    }

    // Another printf BUG ? - Break printf to two statements
    for(i=0;i<6;i++){
        if(info.BaseAddressSize[i]>0){
            //printf("Aperture %d  Base 0x%x Length %d Type %s\n", i,
            //PCI_IS_MEM(info.CpuBaseAddress[i]) ?  (int)PCI_MEM_ADDR(info.CpuBaseAddress[i]) :
            //(int)PCI_IO_ADDR(info.CpuBaseAddress[i]),info.BaseAddressSize[i],
            //PCI_IS_MEM(info.CpuBaseAddress[i]) ? "MEM" : "IO");
        }
    }

    //printf("IRQ %d\n",info.Irq);

	// Assign BADRn IO addresses for PCI-DAS1602
    if(DEBUG){
        //printf("\nDAS 1602 Base addresses:\n\n");
        for(i=0;i<5;i++){
            badr[i]=PCI_IO_ADDR(info.CpuBaseAddress[i]);
            if(DEBUG);//printf("Badr[%d] : %x\n", i, badr[i]);
        }
        //printf("\nReconfirm Iobase:\n");  			// map I/O base address to user space
        for(i=0;i<5;i++){								// expect CpuBaseAddress to be the same as iobase for PC
            iobase[i]=mmap_device_io(0x0f,badr[i]);
            //printf("Index %d : Address : %x ", i,badr[i]);
            //printf("IOBASE  : %x \n",iobase[i]);
        }
    }

    // Modify thread control privity
    if(ThreadCtl(_NTO_TCTL_IO,0)==-1){
        perror("Thread Control");
        exit(1);
    }

    out8(DIO_CTLREG,0x90);					// Port A : Input,  Port B : Output,  Port C (upper | lower) : Output | Output
    out16(INTERRUPT,0x60c0);				// sets interrupts	 - Clears
    out16(TRIGGER,0x2081);					// sets trigger control: 10MHz, clear, Burst off,SW trig. default:20a0
    out16(AUTOCAL,0x007f);					// sets automatic calibration : default

    out16(AD_FIFOCLR,0); 					// clear ADC buffer
    out16(MUXCHAN,0x0D00);				    // Write to MUX register - SW trigger, UP, SE, 5v, ch 0-0
                                            // x x 0 0 | 1  0  0 1  | 0x 7   0 | Diff - 8 channels
                                            // SW trig |Diff-Uni 5v| scan 0-7| Single - 16 channels
}

//read the switches @address of PortA
uintptr_t read_switches(void){
    uintptr_t PORTA_data;
    PORTA_data=in8(DIO_PORTA); 					// Read Port A
    return PORTA_data;
}

//write LEDoutput to PortB
void LED_out(uintptr_t LED_output){
	out8(DIO_PORTB, LED_output);					// output Port A value -> write to Port B
}

//read the potentiometer input
uint16_t read_ADC(unsigned char count){
    unsigned short chan;
    chan= ((count & 0x0f)<<4) | (0x0f & count);
    out16(MUXCHAN,0x0D00|chan);		            // Set channel	 - burst mode off.
    delay(1);									// allow mux to settle
    out16(AD_DATA,0); 							// start ADC
    while(!(in16(MUXCHAN) & 0x4000));
    return(in16(AD_DATA));
}

//write to the DAC
void write_DAC(unsigned int i, unsigned char DAC_chan){
    unsigned short chan;
	if(DAC_chan == 0){
        out16(DA_CTLREG,0x0a23);			// DA Enable, #0, #1, SW 5V unipolar		2/6
	}
	else{
		out16(DA_CTLREG,0x0a43);			// DA Enable, #1, #1, SW 5V unipolar		2/6
	}
   	out16(DA_FIFOCLR, 0);					// Clear DA FIFO  buffer
   	out16(DA_Data,(short) i);
}

//reset the PCI device
void clean_up_PCI_device(void){
    // Reset DAC to 5v
    out16(DA_CTLREG,(short)0x0a23);
    out16(DA_FIFOCLR,(short) 0);
    out16(DA_Data, 0x8fff);
    // Mid range - Unipolar
    out16(DA_CTLREG,(short)0x0a43);
    out16(DA_FIFOCLR,(short) 0);
    out16(DA_Data, 0x8fff);

    pci_detach_device(hdl);
}
