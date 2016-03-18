#include <linux/init.h>
#include <linux/sched.h>
#include <linux/module.h>

/* called when kernel module is loaded */
int task_lin_iter_init(void)
{
	struct task_struct *task;

	printk(KERN_INFO "Loading Module\n");

	for_each_process(task) {
		/* Processes in Linux are stored in a doubly linked list */
		/**
		 *	Useful fields for this exercise are process name,
		 *  state, and process id. They are the following fields
		 *  char comm[TASK_COMM_LEN] -- process name 
		 *  volatile long state 	 -- state
		 *  pid_t pid 				 -- process id
		 */
		 printk(KERN_INFO 
		 		"Task Name:		%s\n"
		 		"Task State:	%ld\n"
		 		"Process ID:	%d\n", 
		 		task->comm,
		 		task->state,
		 		task->pid
		 );
	}
	return 0;
}

void task_lin_iter_exit(void)
{
	printk(KERN_INFO "Removing Module\n");
	//return 0;
}

/* Macros for registering module entry and exit points. */
module_init(task_lin_iter_init);
module_exit(task_lin_iter_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("DFS on task tree-like structure");
MODULE_AUTHOR("JT");