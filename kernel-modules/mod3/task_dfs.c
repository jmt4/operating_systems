#include <linux/init.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/slab.h>

/* this value depends on environment. can be found
 *  here"/proc/sys/kernel/pid_max"
 */
#define MAX_PID 32768

#ifndef CHAR_BIT
#define CHAR_BIT 8
#endif
#ifndef BITSHIFT
#define BITSHIFT 3
#endif
#ifndef BITMASK
#define BITMASK 0x7
#endif

#define WORDSIZE (sizeof(char)*CHAR_BIT)

/*
 * The purpose of this module is to explore the process
 * tree in depth-first search fashion. Specifically, 
 * pre-order traversal starting from the init task.
 */

/* initialize stack as struct list_head */
LIST_HEAD(task_stack);
unsigned char *visited;

struct stack_item {
	struct task_struct *task;
	pid_t ppid;
	struct list_head list;
};

static inline void pid_set_bit(unsigned char *bv, int n)
{
	bv[n >> BITSHIFT] |= 1 << (n & BITMASK);
}

static inline int pid_check_bit(unsigned char *bv, int n)
{
	return bv[n >> BITSHIFT] & 1 << (n & BITMASK);
}

static inline void 
init_stack_item(struct stack_item *item, struct task_struct *task, int pid)
{
	item->task = task;
	item->ppid = pid;
	INIT_LIST_HEAD(&item->list);
	list_add(&item->list, &task_stack);
}

static inline void print_stack_item(struct stack_item *item)
{
	/* 
	 * considered bad practice to split strings
	 * onto multiple lines -- cannot grep them
	 */
	char *yaynay[2] = { "Yes", "No" };

	printk(KERN_INFO "Name: %20s  PID: %5d  PPID: %5d Children:%4s\n",
		item->task->comm, item->task->pid, item->ppid,
		yaynay[list_empty(&(item->task->children))]);
}

static struct stack_item *pop_head(void)
{
	struct stack_item *ptr = list_first_entry(&task_stack, 
		struct stack_item, list);

	list_del(task_stack.next);
	return ptr;
}

int task_dfs_init(void)
{
	struct stack_item *ptr, *sitem, *tmp;
	struct task_struct *tsk;
	struct list_head *lst;

	printk(KERN_INFO "Loading Module\n");

	/* use bit vector to mark explored tasks */
	visited = kzalloc(MAX_PID/WORDSIZE + 1, GFP_KERNEL);
	sitem = kmalloc(sizeof(*sitem), GFP_KERNEL);
	tsk = list_first_entry(	&(init_task.children), 
				struct task_struct, sibling);

	/* Push init task onto stack */
	init_stack_item(sitem, tsk, init_task.pid);

	list_for_each_entry(ptr, &task_stack, list) {
		/* unhook first stack_item from stack linked-list */
		tmp = pop_head();
		print_stack_item(tmp);
		if (!pid_check_bit(visited, tmp->task->pid)) {		

			pid_set_bit(visited, tmp->task->pid);

			/* push children onto stack */
			list_for_each(lst, &tmp->task->children) {
				tsk = list_entry(lst, struct task_struct, sibling);
				sitem = kmalloc(sizeof(*sitem), GFP_KERNEL);
				init_stack_item(sitem, tsk, tmp->task->pid);
			}
		} else {
			/* unreachable in theory -- process tree is out-tree */
			printk(KERN_INFO "Unreachable... Apparently not");
		}
		/* delete popped node */
		kfree(tmp);
		/* set ptr to head of list */
		ptr = container_of(task_stack.next, struct stack_item, list);
	}
	return 0;
}

void task_dfs_exit(void)
{
	struct stack_item *ptr, *next;
	list_for_each_entry_safe(ptr, next,  &task_stack, list) {
		list_del(&ptr->list);
		kfree(ptr);
	}

	kfree(visited);
	printk(KERN_INFO "Removing Module\n");
}

/* Macros for registering module entry and exit points. */
module_init(task_dfs_init);
module_exit(task_dfs_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("DFS on task tree-like structure");
MODULE_AUTHOR("JT");