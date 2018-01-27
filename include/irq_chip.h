#ifndef _H_IRQCHIP
#define _H_IRQCHIP
#include"type.h"

struct irq_ops {
	//int (*) 
	u32 (*hwirq_to_irq)(u32 hwirq);
	u32 (*mask_irq)(u32 vector);

};

struct irq_chip {
	struct irq_ops *ops;
	void *owner;
};

#endif
