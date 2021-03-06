#include "regsclkctrl.h"
#include "regsemi.h"
#include "regsdram.h"
#include "regspower.h"
#include "regsuartdbg.h"
#include "regspinctrl.h"
#include "regsdigctl.h"
#include "regsocotp.h"
#include <stdarg.h>
#include "memory.h"
#include "timers.h"
#include "duart.h"
#include "leds.h"


void set_input_port(void) {
        // set port0 pins 0-7 as GPI
	HW_PINCTRL_MUXSEL0_SET(0x00FF);
	HW_PINCTRL_MUXSEL1_SET(0x0003);
	HW_PINCTRL_DOE0_CLR(0x0100FF);
}


int _start(int arg)
{
	unsigned int tmp;
	int i,k,l;
	unsigned int counter = 0;
	unsigned int Last_Port_Time;
	unsigned int This_Port_Time;
	unsigned int Last_Port;
	unsigned int This_Port;
	volatile *storage; 
	unsigned int memsize = 0x10000000;
//unsigned int volatile * const storage = (unsigned int *) 0x40000000; 
unsigned int volatile * const tmr = (unsigned int *) 0x8001c0c0; 
unsigned int volatile * tmrwr = (unsigned int *) 0x8001c0c8; 


        init_DUART();
	init_PWRLED();
	PWRLED_ON();
       // memsize = get_ram_size((long *)0x40000000, 0x10000000);
	while(1) {
	storage = 0x40000000;

        // if we receive an S we will start a capture
	printf("Press s to start capture\r\n"); 
	while( 's' != getchar()) printf(" bad key try 's'\r\n");
	printf("Starting capture\r\n");
        tmrwr = 0;

	// loop until we see another character

        //Seed our previous readings
        Last_Port=((volatile unsigned int)HW_PINCTRL_DIN0.U & 0xFF);
        Last_Port_Time = (volatile) *tmr;

	while(((volatile unsigned int)HW_PINCTRL_DIN0.U & 0x10000))
          {
          This_Port=((volatile unsigned int)HW_PINCTRL_DIN0.U & 0xFF);
          This_Port_Time = (volatile) *tmr;
 
          toggle_PWRLED();

       	  if(!(This_Port==Last_Port))
            {
            //Got a record
            //Save the delta time
            //*storage++ = This_Port_Time-Last_Port_Time;
            //Save the port data
            *storage++ = Last_Port;
            //Save Absolute Time 
            *storage++ = Last_Port_Time;
//printf(" this time %X %X\r\n",This_Port_Time,storage);

            //Save the port data
            *storage++ = Last_Port;
//printf(" this time %X %X\r\n",This_Port_Time,storage);
            //Update our previous readings
            Last_Port=This_Port;
            Last_Port_Time=This_Port_Time;
            //Remember that we took a sample
            counter++;
            }
         toggle_PWRLED();
         if(counter*4 >= memsize)
           {
	   printf("out of memory!!\r\n");
	   break;
           }
         }
	printf("Capture Stopped\r\n");

        // if we receive an R we will start to report
	printf("Press r to start reporting %X records.\r\n",counter); 
	while( 'r' != getchar());
	printf("Reporting Now\r\n");

	storage = 0x40000000;
	printf("Dumping Records\r\n");
	for(i = 0; i < counter; i+=2) {
           
           printf("%X%X\r\n",*storage++,*storage++);
	
	}


}
	return 0;
}

/* kiss gcc's ass to make it happy */
void __aeabi_unwind_cpp_pr0() {}
void __aeabi_unwind_cpp_pr1() {}
