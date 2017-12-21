#include"schdule.h" 
#include"gate.h"
#include"kernel.h"
#include"global.h"
#include"page.h"
#include"string.h"
#include"debug.h"
#include"print.h"
#include"cpu.h"
#include"mem.h"
#include"processor.h"
#include"bitop.h"
#define ALLOC_COPY_CR3

#ifndef TEST_KERNEL
#define TASK_SS 4048
#define LDT_CS 0
#define LDT_DS 1
#define HW_SWITCH
#define TSS_VECTOR 10
#define TASK_VECTOR_BASE 6
#define gdt_tss_vec(n) (((n) << 1) + TASK_VECTOR_BASE)
#define gdt_ldt_vec(n) (((n) << 1) + TASK_VECTOR_BASE + 1)
#define gdt_tss_sel(n) ((n) << 3)
#define gdt_ldt_sel(n) ((((n) << 1) + TASK_VECTOR_BASE + 1) << 3)
#define LDT_SEL(n)  ((n << 3) | TI_LDT)
#define LDT_SEL_RING3(n)  (LDT_SEL(n) | RPL3)
#define LDT_SEL_RING0(n)  (LDT_SEL(n) | RPL0)
#define GDT_SEL_RING3(n) (n | RPL3)
#define GDT_SEL_RING0(n) (n | RPL0)
struct task_struct *current;
static u32 *pid_bit_map;
struct task_struct *create_task(struct task_struct *parent, task_fn func, u32 flags);
#define PID_MAX 1000
int init_pid_bitmap()
{
	pid_bit_map = kmalloc(PID_MAX/4 + 1, 0, MEM_KERN);
	asm volatile(""::"a"((u32)pid_bit_map):);
	memset(pid_bit_map, 0, PID_MAX/4 + 1);
	if (!pid_bit_map)
		return -1;
}

u32 alloc_pid()
{
	int idx = -1;
	idx = find_first_avail_bit(pid_bit_map, PID_MAX);
	//return 0;
	return idx;
}

static void set_task_status(struct task_struct *task, u8 status) {
	task->status = status;
}

/* first is not used */
static struct task_struct task_run_list;

static struct task_struct *get_next_task(u32 status_mask)
{
	/* this we need to write schduler */
	struct list_head *head =  &task_run_list.list;
	struct list_head *pos = NULL;
	struct task_struct *task;

	list_for_each(head, pos) {
		task = container_of(pos, struct task_struct, list);
		/*
		if (task->status == TASK_INTERRUPT && there is singnal)
			wake up this ?
		*/
			
		if (task->status & status_mask)
			return task;
	} 
	return NULL;
}

static void insert_task(struct task_struct *task)
{
	list_add(&task->list, &task_run_list.list);
	return; 
}

/* make sure param is passed by register eax, edx */
__attribute__((regparm(2)))void switch_to_sw(struct task_struct *prev, struct task_struct *next)
{
	while(1);
	set_tss(TSS_VECTOR, &(next->task_reg));
	/*
	 * this is del with ioctl/interrupt which cause stack switch
	 * if (next->type = TASK_KERN)
	 * 	NOTHING
	 * else
	 *	set_tss(TSS_VECTOR, &(next->task_reg)); 
	 *
	 *
	 */
	/* we need to chaneg the sp0 */
	set_task_status(prev, TASK_WAITING);
	set_task_status(next, TASK_RUNNING);
}

int switch_to(struct task_struct *prev, struct task_struct *next)
{
	if (!prev || !next)
		return -1;
	/* rax is the ret of task */
	asm volatile(" pushl %%ebp\n\t"
		     " movl %%esp, %%ebp\n\t"
		     " pushfl \n\t"
		     " pushl %%eax \n\t"
		     " pushl %%ebx \n\t"
		     " pushl %%ecx \n\t"
		     " pushl %%edx \n\t"
		     " pushl %%esi \n\t"
		     " pushl %%edi \n\t"
		     " movl %[PREV], %%eax\n\t"
		     " movl %[NEXT], %%edx\n\t"
		     " movl %%esp, %[PREV_ESP]\n\t"
		     " movl %[NEXT_ESP], %%esp\n\t"
		     " call switch_to_sw\n\t"
		     " popl %%edi \n\t"
		     " popl %%esi \n\t"
		     " popl %%edx \n\t"
		     " popl %%ecx \n\t"
		     " popl %%ebx \n\t"
		     " pushl %%eax \n\t"
		     " popfl\n\t"
		     " popl %%ebp"
		     ::[NEXT_ESP] "m"(next->task_reg.esp0) ,[PREV_ESP] "m" (prev->task_reg.esp0), [PREV] "m"(prev), [NEXT] "m"(next):);
	return 0;
}

/* bug
 * when call s_t_r3  whether we need to ltr the task ?
 * if we ltr, the des of the tss > type field will be 11
 * not 9. Then load will error
 */
/* switch to ring3 will cause error because
 * 1. when return to ring3, ebp/esp register remain the kernel side. this will cause page fault, but
 *    we didn't handle page fault at this point. 
 * 2. We should thing a better way to do this, as all the ring3 process's userspace stack should not be
 *    the same with kernel space
 */
void switch_to_ring3(struct task_struct *task) 
{ 

	int ts_sel = 0;
	set_tss(TSS_VECTOR, &(task->task_reg));
	ts_sel = gdt_tss_sel(TSS_VECTOR);
	current = task;
	set_task_status(task, TASK_RUNNING);
	asm volatile( "ltr %[TS_SEL]\n\t"
                    " pushl %[SS] \n\t" 
                    " pushl %[ESP] \n\t" 
                    " pushl %[EFLAGS] \n\t"
                    " pushl %[CS] \n\t"
                    " pushl %[IP] \n\t"
		     " movw %[SS], %%ax\n\t"
		     " movw %%ax, %%ds\n\t"
		     " movw %%ax, %%es\n\t"
		     " movw %%ax, %%fs\n\t"
		     " movw %%ax, %%gs\n\t"
		     " iret \n\t" 
		     " 1: \n\t"
		     " cli\n\t"
		     //" movw %[DS] ,%%ax\n\t"
		     " movw %%ax, %%ds\n\t"
		     " movw %%ax, %%es\n\t"
		     " movw %%ax, %%fs\n\t"
		     " movw %%ax, %%gs\n\t"
		     " jmp ."
		     ::[SS] "m" (task->task_reg.ss2), [ESP] "m"(task->task_reg.esp2),
		       [EFLAGS] "m" (task->task_reg.eflags), [CS] "m"(task->task_reg.cs),
		       [IP] "m" (task->task_reg.eip), [DS] "m" (task->task_reg.ds), [TS_SEL] "m"(ts_sel):); 
}

void test_switch_task()
{
	struct task_struct *next = get_next_task(TASK_WAITING); 
	/* we should set current to porper place */
	while(next->pid == current->pid)
	{
		next = container_of(next->list.next, struct task_struct, list);
	}

	switch_to(current, next);
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
	struct tmp _tmp;
	_tmp.b = gdt_tss_sel(t->pid);
	//_t.b = (TASK_VECTOR_BASE << 3);
	//_t.b = TASK_VECTOR;
	u32 ldt_sec = gdt_ldt_sel(t->pid);
	//asm volatile(/*"  movw %0, %%ax\n\t"
	//	           "  movw %1, %%dx\n\t"*/
	//	           "ltr %%ax"::"a"(task_vec), "m" (sys_ds) :);

	
	//asm volatile(" ljmp %0 ":: "m" (ask_vec):);
	//
	current = t;

	asm volatile( " lldt %%ax\n\t"
		      " ljmp %0 "
		     :: "m" (*&_tmp.a), "a"(ldt_sec):);
	
}
void pre_init_task(void )
{
	/*unsigned int  task_vec = TASK_VECTOR_EMP << 3;
	set_tss(TASK_VECTOR_EMP, 0); 
	asm volatile ("ltr %%ax"::"a"(task_vec) :);
	while(1);
	*/
}

static int copy_task_file_struct(struct task_struct *task, struct task_struct *p)
{

	return 0;
}

void idle_task_func(void *idle)
{
	while(1);
}

/* only used for first ide task */
union task_frame {
	struct task_struct idle_task_struct;
	u8 stack[PAGE_SIZE];
};

static union task_frame idle_task;

void prepare_kernel_thread_stack(struct task_struct *task)
{
	return;
}

void init_idle_task(void) {
	struct task_struct *task;

	task = &idle_task.idle_task_struct;

	task->pid = 0;
	task->task_reg.ss0 = sys_ds;
	task->task_reg.esp0 = (u32) task + PAGE_SIZE - 1 ;
	task->task_reg.cr3 = virt_to_phy(get_global_page_dir());  
	task->task_reg.eip =  idle_task_func; 
	task->task_reg.cs = GDT_SEL_RING0(sys_cs);

	set_task_status(task, TASK_WAITING);
	list_init(&task->list);
	list_init(&task->wait_list);
	insert_task(task);
}

/* this is used to create kernel thread */
int create_ktask(task_fn func)
{
	struct task_struct *task;

	task = create_task(NULL, func, KERNEL_THREAD);

	return task->pid;
}

/*
 * task
 * parent : parent task
 * func: func to excute
 * flags: indicate what to do with clone
 */
struct task_struct *create_task(struct task_struct *parent, task_fn func, u32 flags)
{
	struct task_struct *task;
	struct page *pg;
	u32 pid = -1;

	task = (struct task_strcut *)kmalloc(PAGE_SIZE, 0, MEM_KERN);
	if (!task)
		return NULL;

	pid = alloc_pid();
	task->pid = pid;

	task->task_reg.ss0 = sys_ds;
	task->task_reg.esp0 = (u32) task + PAGE_SIZE - 1 ;

	if (parent)
		task->parent = parent;

#ifdef ALLOC_COPY_CR3
	task->pgt = copy_page_table(parent);

/* this is not useable for we don't use hardware switch */
	task->task_reg.cr3 = virt_to_phy(task->pgt);
#else 
	task->task_reg.cr3 = virt_to_phy(get_global_page_dir());  
#endif
	//task->task_reg.esp = (u32)task + PAGE_SIZE - 1;

	if (func) {
		/* 
		 * this part is for return_to_ring3
		 * current just for test:
		 * ss2/esp2 using the same stack of ss0/esp0 
		 * this should not be the real case
		 */
		task->task_reg.eip =  (u32)func; 
		task->task_reg.cs = GDT_SEL_RING3(user_cs);
		task->task_reg.ss2 = GDT_SEL_RING3(user_ds);
		if (!flags)
			task->task_reg.esp2 = 0x1000;
		else 
			task->task_reg.esp2 = 0x2000;
	}
	task->task_reg.eflags = EF_IF;
	/*
	if (flags & CLONE_FS)
		copy_task_file_struct(task, parent);
	else
		init_task_file_struct(task);
	*/

	/* init signal set */
	//init_task_sig_set(task);

	set_task_status(task, TASK_WAITING);
	list_init(&task->list);
	list_init(&task->wait_list);
	insert_task(task);

	return task;
}

int init_schduler(void)
{
	list_init(&task_run_list.list);

	if (init_pid_bitmap())
		return -1;
	/* do we need to add idle */
}

void schdule(void)
{
	struct task_struct *next;
	/* this need to find next task */ 
	/* */
	next = get_next_task(TASK_WAITING);
	if (!next) {
		return;
	}
	switch_to(current, next);
}

void wait_on(struct list_head *wait_list, struct task_struct *next, u32 flags)
{
	next->status = flags;
	list_add(&next->wait_list, wait_list);
	schdule();
}

void wake_up(struct list_head  *wait_list)
{
	struct list_head * head = wait_list;
	struct list_head * pos = NULL;
	struct task_struct *task;

	if (list_empty(wait_list))
		return ;

	list_for_each(head, pos) {
		task =container_of(pos, struct task_struct, wait_list);
		task->status = TASK_WAITING;
		list_del(&task->wait_list);
		insert_task(task);
	}

	schdule();
}
#endif


