#include"pit.h"
#include"type.h"
#include"port.h"
#include"irq.h"

static void pit_set_chanel_mode(u8 ch, u8 opera_mode, u8 access_mode)
{
	u8 data = 0;
	
	data =  (ch << 6) | (opera_mode  << 1) | (access_mode << 4);
	outb(PIT_MODE_PORT, data);

}
static void pit_set_reload_value(u8 ch, u16 value)
{
	u8 data;

	data = value & 0xff;
	outb(PIT_CH0_PORT + ch, data); 

	data = value >> 8;
	outb(PIT_CH0_PORT + ch, data); 
}

static u16 get_timer_interval(void)
{
	return 0;
}

static u16 pit_cal_reload_value(u16 interval)
{
	return 2000;
}

void init_pit(void) {
	u16 value;

	local_irq_disable();

	value = pit_cal_reload_value(get_timer_interval());

	pit_set_chanel_mode(PIT_CH0, PIT_OPM_RATE, PIT_ACM_LH);
	pit_set_reload_value(PIT_CH0, value);

	local_irq_enable();
	return;
}
