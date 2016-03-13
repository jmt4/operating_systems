#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/list.h>
#include <linux/types.h>
#include <linux/slab.h> // for kmalloc/kzalloc

struct birthday {
	int day;
	int month;
	int year;
	struct list_head list;
};

/* initialize var birthday_list as struct list_head */
LIST_HEAD(birthday_list);

/* This function is called when the module is loaded. */
int simple_init(void)
{
	printk(KERN_INFO "Loading Module\n");

	int i;
	for (i = 0; i < 5; i++) {
		struct birthday *person;
		person = kmalloc(sizeof(*person), GFP_KERNEL); // tell kernel standard kmalloc
		person->day = 1 + i;
		person->month = 1+ i;
		person->year = 1988 + i;
		INIT_LIST_HEAD(&person->list);
		list_add_tail(&person->list, &birthday_list);
	}

	struct birthday *ptr;
	list_for_each_entry(ptr, &birthday_list, list) {
		printk(KERN_INFO "\nDay: %d\nMonth: %d\nYear: %d\n",
			ptr->day, ptr->month, ptr->year);
	}
	return 0;
}

/* This function is called when the module is removed. */
int simple_exit(void)
{
	printk(KERN_INFO "Deallocating memory for birthday list\n");
	struct birthday *ptr, *next;
	list_for_each_entry_safe(ptr, next,  &birthday_list, list) {
		list_del(&ptr->list);
		kfree(ptr);
	}
	printk(KERN_INFO "Removing Module\n");
}

/* Macros for registering module entry and exit points. */
module_init(simple_init);
module_exit(simple_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Module");
MODULE_AUTHOR("SGG");
