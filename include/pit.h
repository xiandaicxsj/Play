#ifndef _H_PIT
#define _H_PIT

#define PIT_CH0_PORT  0x40
#define PIT_CH1_PORT  0x41
#define PIT_CH2_PORT  0x42

#define PIT_MODE_PORT 0x43 

#define PIT_CH0 0
#define PIT_CH1 1
#define PIT_CH2	2
#define PIT_RD  3

#define PIT_OPM_IOTC 0
#define PIT_OPM_HRTO 1
#define PIT_OPM_RATE 2
#define PIT_OPM_SQ_WAVE 3
#define PIT_OPM_SF_STROBE 4
#define PIT_OPM_HD_STROBE 5
#define PIT_OPM_RATE2 6
#define PIT_OPM_SQ_WAVE2 7

#define PIT_ACM_LO 1
#define PIT_ACM_HO 2
#define PIT_ACM_LH 3
void init_pit(void);
#endif
