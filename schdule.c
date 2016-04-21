#include"schdule.h" 
#include"gate.h"
#include"kernel.h"
#include"global.h"
#include"page.h"
#include"string.h"
#include"debug.h"
#include"print.h"
#define TASK_SS 4048
#define LDT_CS 0
#define LDT_DS 1
#define HW_TASK_SWITCH
#define TASK_VECTOR_BASE 5
#define gdt_tss_vec(n) (((n) << 1) + TASK_VECTOR_BASE)
#define gdt_ldt_vec(n) (((n) << 1) + TASK_VECTOR_BASE + 1)
#define gdt_tss_sel(n) ((((n) << 1) + TASK_VECTOR_BASE) << 3)
#define gdt_ldt_sel(n) ((((n) << 1) + TASK_VECTOR_BASE + 1) << 3)
#define LDT_SEL(n)  ((n << 3) | TI_LDT)
#define LDT_SEL_RING3(n)  (LDT_SEL(n) | RPL3)

#define switch_to_ring3(task) \
{ \
	u32 lldt_sel = gdt_ldt_sel(task->pid);\
	asm volatile(" lldt %%ax\n\t"\
		     " pushw %[SS] \n\t" \
		     " pushw %[ESP] \n\t" \
		     " pushw %[EFLAGS] \n\t" \
		     " pushw %[CS] \n\t" \
		     " pushw %[IP] \n\t" \
		     " iret " \
		     ::[SS] "m" (task->ss2), [ESP] "m"(task->esp2), \
		       [EFLAGS] "m" (task->eflags), [CS] "m"(task->cs), \
		       [IP] "m" (task->ip), [LDT] "a"(lldt_sel):); \

}
struct task_head_t
{
	struct task_struct *task_list;
	u32  task_num;
};

static struct task_head_t task_head;
static void insert_task(struct task_struct *_task)
{
	if (!task_head.task_list)
	{
		task_head.task_list = _task;
		return ;
	}
	_task->next = task_head.task_list;
	task_head.task_list = _task;
	return; 
}
void test_process()
{
	enable_interrupt();
	int a=0;		
	int b=0;
	int c;
	c= a+b+1;
	print('a');
	while(1);

}

void switch_to_test(struct task_struct *t)
{

	/*struct tmp{
	unsigned int a;
	unsigned int b;
	};*/
	struct tmp{
	long a;
	long b;
	};
	unsigned int task_vec;
	struct tmp _t;
	_t.b = gdt_tss_sel(0);
	//_t.b = (TASK_VECTOR_BASE << 3);
	//_t.b = TASK_VECTOR;
	task_vec = _t.b ;
	u32 ldt_sec = gdt_ldt_sel(t->pid);
	//asm volatile(/*"  movw %0, %%ax\n\t"
	//	           "  movw %1, %%dx\n\t"*/
	//	           "ltr %%ax"::"a"(task_vec), "m" (sys_ds) :);

	
	//asm volatile(" ljmp %0 ":: "m" (ask_vec):);
	asm volatile( " lldt %%ax\n\t"
		      " ljmp %0 "
		     :: "m" (*&_t.a), "a"(ldt_sec):);
	
}
void pre_init_task(void )
{
	/*unsigned int  task_vec = TASK_VECTOR_EMP << 3;
	set_tss(TASK_VECTOR_EMP, 0); 
	asm volatile ("ltr %%ax"::"a"(task_vec) :);
	while(1);
	*/
}
/*
 *
 * ss[0-3] static
 * ss/cs/ds/eip/should be set.
 */
u32 alloc_pid()
{
		return 0;

}
void init_task(struct task_struct *task)
{
	//u32 sys_ds = (0x18 );
	//u32 sys_cs = (0x10 );
	u32 task_cs = LDT_CS;
	u32 task_ds = LDT_DS;
	u32 pid = 0;

	pid = alloc_pid();
	task->pid = pid;

	task->task_reg.ss0 = sys_ds;
	task->task_reg.esp0 = (u32) task + PAGE_SIZE - 1 ;

	task->task_reg.cr3 = virt_to_phy((u32)&init_page_dir);  
	task->task_reg.esp = (u32) task + PAGE_SIZE - 1;

	/* just test */
	task->task_reg.eip = (u32) test_process;

	/* task ldt */
	task->task_reg.ldt_sel = gdt_ldt_sel(pid);
	task->ldt[LDT_CS].lo = 0x0000ffff; /* ldt cs */
	task->ldt[LDT_CS].hi = 0x00cffa00;
	task->ldt[LDT_DS].lo = 0x0000ffff; /* ldt ds */
	task->ldt[LDT_DS].hi = 0x00cff200;
	set_ldt(gdt_ldt_vec(pid), task->ldt, X86_GDT_LIMIT_FULL); /* not sure about the limit */ 
	
	task->task_reg.ss2 = LDT_SEL_RING3(task_ds);
	task->task_reg.es = LDT_SEL_RING3(task_ds); /* index in the ldt */
	task->task_reg.cs = LDT_SEL_RING3(task_cs);
	task->task_reg.ds = LDT_SEL_RING3(task_ds);
	task->task_reg.fs = LDT_SEL_RING3(task_ds);
	task->task_reg.gs = LDT_SEL_RING3(task_ds);
	task->task_reg.ss = LDT_SEL_RING3(task_ds);
	task->task_reg.eflags = 0x200;
	
	insert_task(task);
	set_tss(gdt_tss_vec(pid), task); 
	switch_to_test(task);
	switch_to_ring3(task);
}

void switch_to(struct task_struct *pre, struct task_struct *next)
{
	struct tmp{
	long a;
	long b;
	};
	unsigned int task_vec;
	struct tmp _t;
	_t.b = gdt_tss_sel(next->pid);
	task_vec = _t.b ;
	asm volatile(" movl $1f, %[next_ip] \n\t"
		     " ljmp %[task_sec] \n\t"
		     " 1: "
		     :[next_ip] "=m" (pre->task_reg.eip)
		     :[task_sec] "m" (*&_t.a) );
}

/* do we need to setup the NT in eflags here */
