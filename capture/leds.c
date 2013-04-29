#include "regspinctrl.h"
//#include "types.h"


// PWRLED on BANK3_PIN4, MUXSEL6, HW_PINCTRL_DRIVE12
#define PWRLED_MUX	 (3<<8)
#define PWRLED_PIN	 (1<<4)

void init_PWRLED(void) {


//Enable pin GPIO3_4 LED is active low 
        HW_PINCTRL_MUXSEL6_SET(PWRLED_MUX);
        HW_PINCTRL_DOUT3_CLR(PWRLED_PIN);
        HW_PINCTRL_DOE3_SET(PWRLED_PIN);
       
}

inline void PWRLED_ON() {
                HW_PINCTRL_DOE3_SET(PWRLED_PIN);
}
inline void PWRLED_OFF() {
                HW_PINCTRL_DOUT3_CLR(PWRLED_PIN);
}


inline void set_PWRLED(int state) {
        if(state) {
                HW_PINCTRL_DOE3_SET(PWRLED_PIN);
        }
        else {
                HW_PINCTRL_DOUT3_CLR(PWRLED_PIN);
        }
}

inline void toggle_PWRLED(void) {

                HW_PINCTRL_DOUT3_TOG(PWRLED_PIN);
}

