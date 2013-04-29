#include "regsdigctl.h"
void udelay(unsigned int us)
{
	unsigned int start , cur;
	start = cur = HW_DIGCTL_MICROSECONDS_RD();

	while (cur < start+us) {

		cur = HW_DIGCTL_MICROSECONDS_RD();
		/*printf("0x%x\r\n",cur);*/
	}

}

inline long micros() {
	return (volatile)HW_DIGCTL_MICROSECONDS_RD();
}

inline void set_micros(long val) {
	 HW_DIGCTL_MICROSECONDS_WR(val);
}
