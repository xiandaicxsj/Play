#ifndef _H_SYSCALL
#define _H_SYSCALL

#include"linkage.h"
/* we know only five data is used */
#define MAX_SYS_CALL_NR 300

#define _SYS_OPEN	(1<<0)
#define _SYS_READ	(1<<1)
#define _SYS_WRITE	(1<<2)
#define _SYS_CLOSE	(1<<3)
#define _SYS_SEEK	(1<<4)
#define _SYS_IOCTL	(1<<5)

#define _SYS_FORK 	(1<<6)
#define _SYS_EXEC 	(1<<7)

#define P(t,s) t s
#define VAR_5 VAR_4, P(t5, s5)
#define VAR_4 VAR_3, P(t4, s4)
#define VAR_3 VAR_2, P(t3, s3)
#define VAR_2 VAR_1, P(t2, s2)
#define VAR_1 P(t1, s1)

#define SYS_CALL_DEF(func, nr_param) extern asmlinkage do_##func(VAR_##nr_param)

/*
#define SYS_CALL(i, name) extern asmlinkage do_sys_##name();
*/

#endif
