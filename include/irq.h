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

static inline void local_irq_save(u32 *rf) 
{
	u32 flags;
	asm volatile ("pushfl\n\t"
		      "popl %0\n\t"
		      :"=rm"(flags)::"memory");
	*rf = flags;
	local_irq_disable();
}

static inline void local_irq_restore(u32 *rf)
{
	u32 flags;
	flags = *rf;
	asm volatile ("pushl %%eax\n\t"
		      "popfl\n\t"
		      ::"a"(flags):);
}
#endif
