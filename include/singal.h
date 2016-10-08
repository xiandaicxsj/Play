#ifndef _H_SINGAL
#define _H_SINGAL
#define SIG_STOP (1 << 0)
#define SIG_WAIT (1 << 2)
#include"schdule.h"

int init_task_sig_set(struct task_struct *task);

#endif
