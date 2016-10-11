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
#define ALLOC_COPY_CR3
#define test_proc

#ifdef test_proc
#include"test_process.h"
extern test_process();
extern test_process1();
#endif

#ifndef TEST_KERNEL
#define TASK_SS 4048
#define LDT_CS 0
#define LDT_DS 1
#define HW_TASK_SWITCH
#define TASK_VECTOR_BASE 6
#define gdt_tss_vec(n) (((n) << 1) + TASK_VECTOR_BASE)
#define gdt_ldt_vec(n) (((n) << 1) + TASK_VECTOR_BASE + 1)
#define gdt_tss_sel(n) ((((n) << 1) + TASK_VECTOR_BASE) << 3)
#define gdt_ldt_sel(n) ((((n) << 1) + TASK_VECTOR_BASE + 1) << 3)
#define LDT_SEL(n)  ((n << 3) | TI_LDT)
#define LDT_SEL_RING3(n)  (LDT_SEL(n) | RPL3)
#define LDT_SEL_RING0(n)  (LDT_SEL(n) | RPL0)
struct task_struct *current;
static u32 *pid_bit_map;
struct task_strcut *create_task(struct task_struct *parent, task_fn func, u32 flags);
#define PID_MAX 1000
int init_pid_bitmap()
{
	pid_bit_map = kmalloc(PID_MAX/4 + 1, 0, MEM_KERN);
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

static struct task_struct task_run_list;

static struct task_struct *get_next_task()
{
	/* */
	struct list_head *head =  task_run_list.next;
	struct list_head *pos = NULL;
	struct task_struct *task;

	list_for_each(head, pos) {
		task = container_of(pos, struct task_struct, wait_list);
		/*
		if (task->status == TASK_INTERRUPT && there is singnal)
			wake up this ?
		*/
			
		if (task->status == TASK_WAITING)
			return task;
	} 
}

static void insert_task(struct task_struct *_task)
{
	list_add(&_task->list, &task_run_list.list);
	return; 
}

void init_task_file_struct(struct task_struct *task)
{
	/* do we really need this */
	task->file[0] = get_stdio_file_struct();
	task->file[1] = get_stdout_file_struct();
	task->file[2] = get_stderr_file_struct();
	task->fd_count = 3;
}

void switch_to(struct task_struct *prev, struct task_struct *next)
{
	u32 ldt_sec = gdt_ldt_sel(next->pid);

	struct tmp{
	long a;
	long b;
	};
	struct tmp _tmp;
	_tmp.b = gdt_tss_sel(next->pid);

	current = next;
	next->status = TASK_RUNNING;
	prev->status = TASK_WAITING;
	asm volatile(//" movl $1f, %[next_ip] \n\t"
		     " lldt %%ax\n\t"
		     " ljmp %[task_sec] \n\t"
		     " 1: "
		     ://[next_ip] "=m" (prev->task_reg.eip)
		     :[task_sec] "m" (*&_tmp.a),  "a" (ldt_sec));
}

/* bug
 * when call s_t_r3  whether we need to ltr the task ?
 * if we ltr, the des of the tss > type field will be 11
 * not 9. Then load will error
 */
static void switch_to_ring3(struct task_struct *task) 
{ 
	u32 lldt_sel = gdt_ldt_sel(task->pid);
	u32 ts_sel = gdt_tss_sel(task->pid);
	asm volatile("ltr %[TS_SEL]\n\t"
		     "lldt %[LDT] \n\t"
		     " pushl %[SS] \n\t" 
		     " pushl %[ESP] \n\t" 
		     " pushfl \n\t"
		     //" pushw %[EFLAGS] \n\t"
		     " pushl %[CS] \n\t"
		     //" pushl $1f \n\t"
		     " pushl %[IP] \n\t"
		     " iret \n\t" 
		     " 1: \n\t"
		     " cli\n\t"
		     " movw %[DS] ,%%ax\n\t"
		     " movw %%ax, %%ds\n\t"
		     " movw %%ax, %%es\n\t"
		     " movw %%ax, %%fs\n\t"
		     " movw %%ax, %%gs\n\t"
		     " jmp ."
		     ::[SS] "m" (task->task_reg.ss2), [ESP] "m"(task->task_reg.esp2),
		       [EFLAGS] "m" (task->task_reg.eflags), [CS] "m"(task->task_reg.cs),
		       [IP] "m" (task->task_reg.eip), [DS] "m" (task->task_reg.ds), [LDT] "m"(lldt_sel), [TS_SEL] "m"(ts_sel):);
}

void test_switch_task()
{
	struct task_struct *next = get_next_task(); 
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

/* this is used to create kernel thread */
int create_ktask(task_fn func)
{
	struct task_struct *task;

	task = create_task(NULL, func, 0);

	return task->pid;
}

/*
 * task
 * parent : parent task
 * func: func to excute
 * flags: indicate what to do with clone
 */
struct task_strcut *create_task(struct task_struct *parent, task_fn func, u32 flags)
{
	struct task_struct *task;
	u32 task_cs = LDT_CS;
	u32 task_ds = LDT_DS;
	u32 pid = -1;

	task = kmalloc(sizeof(*task), 0, MEM_KERN);
	if (!task)
		return NULL;
	//u32 sys_ds = (0x18 );
	//u32 sys_cs = (0x10 );

	pid = alloc_pid();

	task->pid = pid;
	task->task_reg.ss0 = sys_ds;
	task->task_reg.esp0 = (u32) task + PAGE_SIZE - 1 ;

	if (!parent)
		task->parent = parent;

/* copy mem */

#ifdef ALLOC_COPY_CR3
	task->pgt = copy_page_table(parent);

	task->task_reg.cr3 = virt_to_phy(task->pgt);
#else 
	task->task_reg.cr3 = virt_to_phy((u32)&init_page_dir);  
#endif
	task->task_reg.esp = (u32)task + PAGE_SIZE - 1;

	/* just test */
	if (func)
		task->task_reg.eip =  (u32)func; 

	/* task ldt */
	task->ldt[LDT_CS].lo = 0x0000ffff; /* ldt cs */
	task->ldt[LDT_CS].hi = 0x00cffa00;
	task->ldt[LDT_DS].lo = 0x0000ffff; /* ldt ds */
	task->ldt[LDT_DS].hi = 0x00cff200;
	set_ldt(gdt_ldt_vec(pid), task->ldt, X86_GDT_LIMIT_FULL); /* not sure about the limit */ 
	task->task_reg.ldt_sel = gdt_ldt_sel(pid);
	
	task->task_reg.ss2 = LDT_SEL_RING3(task_ds);
	task->task_reg.es = LDT_SEL_RING3(task_ds); /* index in the ldt */
	task->task_reg.cs = LDT_SEL_RING3(task_cs);
	task->task_reg.ds = LDT_SEL_RING3(task_ds);
	task->task_reg.fs = LDT_SEL_RING3(task_ds);
	task->task_reg.gs = LDT_SEL_RING3(task_ds);
	task->task_reg.ss = LDT_SEL_RING3(task_ds);
	task->task_reg.eflags = IOPL_RING3 | IF;
	
	set_tss(gdt_tss_vec(pid), task); 

	/* related with file sys */
	if (flags & CLONE_FS)
		copy_task_file_struct(task, parent);
	else
		init_task_file_struct(task);

	/* init signal set */
	init_task_sig_set(task);

	task->status = TASK_IDLE;
	list_init(&task->list);
	list_init(&task->wait_list);
	insert_task(task);

	return task;
}

int init_schduler(void)
{
	list_init(&task_run_list);

	if (init_pid_bitmap())
		return -1;
	/* do we need to add idle */
}

void schdule(void)
{
	struct task_struct *next;
	/* this need to find next task */ 
	/* */
	next = get_next_task();
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
		insert_task(task);
	}

	schdule();
}
#endif


