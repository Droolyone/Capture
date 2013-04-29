/*
 * Copyright 2009-2010 Freescale Semiconductor, Inc. All Rights Reserved.
 */

/*
 * The code contained herein is licensed under the GNU General Public
 * License. You may obtain a copy of the GNU General Public License
 * Version 2 or later at the following locations:
 *
 * http://www.opensource.org/licenses/gpl-license.html
 * http://www.gnu.org/copyleft/gpl.html
 */

/*
 * This "boot applet" assists the boot ROM by initializing the hardware platform
 * to facilitate and optimize booting. For example, it initializes the external
 * memory controller, so applications can be loaded directly to external SDRAM.
 * This work is not done in the ROM, because it is platform dependent.
 */

////////////////////////////////////////////////////////////////////////////////
#include "types.h"
#include "error.h"

//#include "registers/regsrtc.h"
//#include "registers/regspower.h"
#include "registers/regsdigctl.h"
#include "registers/regsicoll.h"
#include "registers/hw_irq.h"
#include "hw/power/hw_power.h"
#include "hw/icoll/hw_icoll.h"
#include "hw/core/hw_core.h"
#include "hw/lradc/hw_lradc.h"
#include "hw/digctl/hw_digctl.h"
#include "drivers/power/ddi_power.h"
#include "registers/regsuartdbg.h"
#include "registers/regsclkctrl.h"
#include "registers/regspwm.h"
#include "registers/regslradc.h"

//#include <stdarg.h>
//#include "debug.h"
#include "parser.h"
#include "lss.h"
#include "cfa10049.h"
#include "duart.h"
#include "leds.h"

/* for debugging and testing */
#include "registers/regspinctrl.h"


#define FORWARD		0x01
#define REVERSE		0x00
void udelay(unsigned int microSec);
void init_STEPPERS(void) {
       // set the stepper STEP and DIR pins to GPIO output 4ma 3.3v
       HW_PINCTRL_MUXSEL0_SET(MUX0_PINS);
       HW_PINCTRL_MUXSEL1_SET(MUX1_PINS); 
//       HW_PINCTRL_DRIVE0_CLR(DRIVE0_MA_PINS);
//       HW_PINCTRL_DRIVE2_CLR(DRIVE2_MA_PINS);
//       HW_PINCTRL_DRIVE3_CLR(DRIVE3_MA_PINS);
//       HW_PINCTRL_DRIVE0_SET(DRIVE0_V_PINS);
//       HW_PINCTRL_DRIVE2_SET(DRIVE2_V_PINS);
//       HW_PINCTRL_DRIVE3_SET(DRIVE3_V_PINS);
       HW_PINCTRL_DOUT0_CLR(STEP_PINS | DIR_PINS);
       HW_PINCTRL_DOE3_SET(STEP_PINS | DIR_PINS);
}


void init_SPI_PINS(void) {
       printf("Configuring SPI pins for bitbanging");
// select the pins we need as GPIO
        HW_PINCTRL_MUXSEL1_SET(MOSI_MUX | SCK_MUX | RCK32_MUX | RCK16_MUX);
// set the drive current
        HW_PINCTRL_DRIVE3_CLR(MOSI_MA | SCK_MA | RCK32_MA);
        HW_PINCTRL_DRIVE2_CLR(RCK16_MA);
//set the drive voltage
        HW_PINCTRL_DRIVE3_SET(MOSI_V | SCK_V | RCK32_V);
        HW_PINCTRL_DRIVE2_SET(RCK16_V);
       
// set the pins as outputs and set the initial values to zero
        HW_PINCTRL_DOUT0_CLR(MOSI_PIN|SCK_PIN|RCK32_PIN|RCK16_PIN);
// enable the pins
        HW_PINCTRL_DOE0_SET(MOSI_PIN|SCK_PIN|RCK32_PIN|RCK16_PIN);
}

void set_register(uint32_t write_pin ,uint32_t value ) {

int i;
int length;

       if(write_pin == RCK32_PIN){
          printf("Setting 32 bit register: ");
          length = 32;
       }
       else {
          printf("Setting 16 bit register: ");
          length = 16;
       }
       printf("buffer= %X\r\n",value);
       for(i=0;i<length;i++) {
           if((value >> i) & 0x00000001){ 
              HW_PINCTRL_DOUT0_SET(MOSI_PIN);
 //             printf("\n\rset");
           }
           else
           {
              HW_PINCTRL_DOUT0_CLR(MOSI_PIN);
//              printf("\n\rreset");
           }
// the 74HC595 has a max transition delay of 1000 ns.

           HW_PINCTRL_DOUT0_SET(SCK_PIN);
           udelay(1);
           HW_PINCTRL_DOUT0_CLR(SCK_PIN);
           udelay(1);
	}
// clock the data to the outputs
      
           HW_PINCTRL_DOUT0_SET(write_pin);
// the 74HC595 has a max transition delay of 1000 ns.
           udelay(1);
           HW_PINCTRL_DOUT0_CLR(write_pin);
          
}

void set_direction(uint32_t motors , uint8_t direction) {
     
      if(direction == FORWARD)
         HW_PINCTRL_DOUT0_SET(motors);
      else
	 HW_PINCTRL_DOUT0_CLR(motors);
} 
/*     
void step(uint32_t motors) {
// pretty simple, just output the value for a couple of microseconds
    HW_PINCTRL_DOUT0_SET(motors);
    udelay(2);
    HW_PINCTRL_DOUT0_CLR(motors);
}
*/
void stepper_test(uint32_t motors,int steps) {

static uint8_t direction = FORWARD;
int i;
       printf("motors = %X\r\n",motors);
       set_direction((DIR_A |DIR_B | DIR_X | DIR_Y | DIR_Z),direction);
       direction ^= 1;
       for(i = 0; i < steps; i++) {
           udelay(1000);
           step(motors);
       }
}

void setup_stepper_X() {
 
motor_ctrl_t motor_port;  
       
      MOTOR_X_CLR(motor_port.U,A4988_CLEAR);
      MOTOR_X_SET(motor_port.U,(BF_N_RESET | BF_N_SLEEP | STEP_16));
	set_register(RCK32_PIN,motor_port.U);
}


void clr_heater_reg(void) {

uint16_t heaters;

//zero out the heater register  
  HW_HEAT_CTRL_CLR(heaters,(0xffff));
  set_register(RCK16_PIN,heaters);
}  

//
////////////////////////////////////////////////////////////////////////////////
//! \brief  Sets up the  .
//
//! \return SUCCESS(0) Power block ready.
////////////////////////////////////////////////////////////////////////////////

int _start( void ) {
  line_info info;
  int retval;
  
  init_DUART();
  init_PWRLED();

do {
  gets((char*)&info.buffer,80);
  retval = process_line(&info);
  } while(!retval);
    
return retval;
}



      


void udelay(unsigned int microSec)
{
	unsigned int currentTime = HW_DIGCTL_MICROSECONDS_RD();
	while ((HW_DIGCTL_MICROSECONDS_RD() - currentTime) <  microSec);
}

/* workaround gcc 4.3 link error*/
void __aeabi_unwind_cpp_pr1() {}

