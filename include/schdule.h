#ifndef _H_SCHDULE
#define _H_SCHDULE
#include"type.h"
#include"gate.h"
#include"list.h"
#include"fs.h"
#include"inode.h"

#define TASK_RUNNABLE (1u<<0)
#define TASK_UNINTERRUPT (1u<<1)
#define TASK_INTERRUPT (1u<<2)
typedef struct tss_reg
{
	u32 previous_link;
	u32 esp0;
	u32 ss0;
	u32 esp1;
	u32 ss1;
	u32 esp2;
	u32 ss2;
	u32 cr3;
	u32 eip;
	u32 eflags;
	u32 eax;
	u32 ecx;
	u32 edx;
	u32 ebx;
	u32 esp;
	u32 ebp;
	u32 esi;
	u32 edi;
	u32 es;
	u32 cs;
	u32 ss;
	u32 ds;
	u32 fs;
	u32 gs;
	u32 ldt_sel;
	u32 io;
	u32 tss;
}tss_reg;

struct task_struct
{
	struct tss_reg task_reg;//这样就可以直接对这里进行操作了
	struct seg_desc ldt[3];//这里对应的是任务的ldt需要重新进行处理
	u32 pid;
	u32 cr3;
	u32 status;
	struct file_struct file[20];
	struct m_inode *root;
	struct m_inode *pwd;
	struct task_struct *next;
	struct list_head list;
	struct list_head wait_list;
};
extern struct task_struct *current;
void init_task(struct task_struct *);
void pre_init_task(void );
void test_switch_task(void);
void switch_to_test(struct task_struct *t);
#define TASK_RUN (1 << 0)
#define TASK_WAIT (1 << 1)
#endif
