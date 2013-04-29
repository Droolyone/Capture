//This program is an interactive comand processor for driving the functions
//on the Crystalfontz CFA-10049 board

#include <stdio.h>
#include <string.h>
#include "parser.h"

int main( void ) {
  line_info info;
  int retval;
  

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

