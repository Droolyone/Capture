#include <stdarg.h>
//#include "types.h"
#include "regsuartdbg.h"
#include "regspinctrl.h"


#define DUARTRX_MUX   (2<<4)
#define DUARTTX_MUX   (2<<6)
#define DUARTRX_CLR   (3<<4)
#define DUARTTX_CLR   (3<<6)
#define DUART_CTS_CLR (3<<0)
#define DUART_RTS_CLR (3<<2)
#define DUART_CTS     (2<<0)
#define DUART_RTS     (2<<2) 
#define PORT0_PIN16   (1<<16)
#define RXE_BIT	      (1<<9)
#define RTSEN_BIT     (1<<14)
#define CTSEN_BIT     (1<<15)
void init_DUART(void) {

      /* Reroute DUART to GPOI3_2 and GPIO3_3         */
      HW_PINCTRL_MUXSEL6_CLR(DUARTRX_CLR|DUARTTX_CLR);
      HW_PINCTRL_MUXSEL6_SET(DUARTRX_MUX|DUARTTX_MUX);
      /* set up hardware handshaking */
      HW_PINCTRL_MUXSEL6_CLR(DUART_CTS_CLR|DUART_RTS_CLR);
      HW_PINCTRL_MUXSEL6_SET(DUART_CTS|DUART_RTS);   
      /* pull up pin 16 */
      HW_PINCTRL_PULL0_SET(PORT0_PIN16);
      /* set up the uart as bidirectional             */
     HW_UARTDBGCR_SET(0x00ca0000);
   
      /* set the baud rate divisor to 8 (3Mbs) */
//      HW_UARTDBGIBRD_CLR(0x0000ffff);
//      HW_UARTDBGIBRD_SET(0x00000018);
      /* set baud rate divisor to 0 */
      HW_UARTDBGFBRD_CLR(0x0000003f);
    

}
void putc(unsigned char ch)
{
	int loop = 0;
	while (HW_UARTDBGFR_RD()&BM_UARTDBGFR_TXFF) {
		loop++;
		if (loop > 10000)
			break;
	};

	if(!(HW_UARTDBGFR_RD() &BM_UARTDBGFR_TXFF)) {
	   HW_UARTDBGDR_WR(ch);
        }
}

void printhex(int data)
{
	int i = 0;
	char c;
	for (i = sizeof(int)*2-1; i >= 0; i--) {
		c = data>>(i*4);
		c &= 0xf;
		if (c > 9)
			putc(c-10+'A');
		else
			putc(c+'0');
	}
}


void printstr(char *str) {

     while(*str) {
        putc(*str);
        str++;
      }
}


void printchar(int val) {
   
     putc(val);
}

void printf(char *fmt, ...)
{
	va_list args;
	
	va_start(args, fmt);
	while (*fmt) {

		if (*fmt == '%') {
			fmt++;
			switch (*fmt) {
                        case 'c':
                        case 'C':
                              printchar(va_arg(args,int));
                              break;
                        case 's':
                        case 'S':
                              printstr(va_arg(args,char*));
                              break;
			case 'x':
			case 'X':
				printhex(va_arg(args, int));
				break;
			case '%':
				putc('%');
				break;
			default:
				break;
			}

		} else {
			putc(*fmt);
		}
		fmt++;
	}
	va_end(args);
}

int getchar(void) {
  
  	while(HW_UARTDBGFR_RD()&BM_UARTDBGFR_RXFE);      
        return (HW_UARTDBGDR_RD()& 0xff);

}      


int gets(char* buffer, int max_len) {
int i = 0;
    do {
         buffer[i] = getchar();
         if(buffer[i] == '\n') {
             break;
         }
         i++;
    }while(i < max_len);
     buffer[i] = 0x00;
   
return i;
}    


