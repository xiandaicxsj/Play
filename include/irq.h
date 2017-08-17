#ifndef _H_IRQ
#define _H_IRQ

static inline void local_irq_disable()
{
	asm volatile ( "cli" );
}
 
static inline void local_irq_enable()
{
	asm volatile ( "sti" );
}

#endif
