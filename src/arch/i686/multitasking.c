#include <arch/i686/multitasking.h>
#include <arch/i686/regs.h>
#include <arch/i686/io.h>
#include <memory.h>
#include <stdio.h>
#include <math.h>
#include <tty_old.h>
#include <stddef.h>
struct task *process_list=NULL;
struct task *running_task;
uint64_t system_idle_time = 0;
uint64_t system_in_work_time = 0;
void insertTaskAfter(struct task* a, struct task* b)
{
	b->prev = a;
	b->next = a->next;
	a->next = b;
	b->next->prev = b;
}
//What do you want?
//Arguments. Lots of arguments.
struct task *create_task(void *page_directory,
			             void *start_address,
			             enum task_priority priority)
{
	struct task *t = malloc(sizeof(struct task));
	printf("creating task, structure address=0x%x, code address=0x%x\n", t, start_address);
	t->eax         = 0;
	t->ebx         = 0;
	t->ecx         = 0;
	t->edx         = 0;
	t->esi         = 0;
	t->edi         = 0;
	t->eflags      = 0; //Interrupts are enabled.
	t->stack       = malloc(4096) + 4096; //4kb will be enough
	t->eip         = (uint32_t) start_address;
	t->cr3         = (uint32_t) page_directory;
	t->priority    = priority;
	t->alive       = 1;
	t->exit_status = 0;
	t->time_out    = priority;
	t->next        = t; //It will point to start of process list.
	//Now we will put it into our process list.
	if(process_list)
		insertTaskAfter(process_list->prev, t);
	else //This is first task. There's no process list AT ALL now, so we will create it.
	{
		t->prev = t;
		process_list = t;
	}
	//process_list->prev = t;
	return t;
}
/*
	get_cpu_usage: Return CPU usage in percents.
 */
double get_cpu_usage()
{
	return percentage_relation((double) system_idle_time, (double) system_in_work_time);
}
void pit_do_multitasking_work(regs* r)
{
	//Increment work or idle time.
	if(running_task == process_list)
		system_idle_time++;
	else
		system_in_work_time++;
	if(running_task->time_out) //Yes, it can be zero.
        running_task->time_out--;
    if(running_task->time_out == 0) //Our task's time is out, so we need to switch task.
    {
        running_task->time_out = running_task->priority;
        //Current task state saved. We will call switch_task to actually switch.
        //printf("Interrupting current task! New EIP is 0x%x\n", running_task->eip);
        if (r->int_no >= 40)
        {
            outb(0xA0, 0x20);
        }
        outb(0x20, 0x20);
        struct task *old_running_task = running_task;
        running_task = running_task -> next;
        switch_task(old_running_task, r, running_task);
    }
}
void switch_task(struct task *from, regs *r, struct task *to)
{
	if(from != NULL && r != NULL)
	{
    	from->eax    = r->eax        ;
    	from->ebx    = r->ebx        ;
    	from->ecx    = r->ecx        ;
    	from->edx    = r->edx        ;
    	from->esi    = r->esi        ;
    	from->edi    = r->edi        ;
   	 	from->eip    = r->eip        ;
    	from->eflags = r->eflags     ;
	}
	to->eflags |= 0x200;
	asm volatile("mov %0, %%eax;"
		"mov %1, %%ebx;"
		"mov %2, %%ecx;"
		"mov %3, %%edx;"
		"mov %4, %%esi;"
		"mov %5, %%edi;"
		"mov %6, %%ebp;" :
		"=g" (to->eax),
		"=g" (to->ebx),
		"=g" (to->ecx),
		"=g" (to->edx),
		"=g" (to->esi),
		"=g" (to->edi),
		"=g" (to->ebp) : : "memory");
	//asm volatile("iret"); //It will pop a lot of registers.
	//If something happened, halt the system.
	//while(1);
}
void idle_process()
{
	while(1)
		asm("sti; hlt");
}
void test_process()
{
	while(1)
		tty_putchar('a');
}
void test2_process()
{
	while(1)
		tty_putchar('b');
}
void test3_process()
{
	while(1)
		tty_putchar('c');
}
void init_multitasking()
{
	create_task(0, (void*) idle_process,  PRIORITY_IDLE);
	create_task(0, (void*) test_process,  PRIORITY_NORMAL);
	create_task(0, (void*) test2_process, PRIORITY_NORMAL);
	create_task(0, (void*) test3_process, PRIORITY_NORMAL);
	struct task *t;
	for(t = process_list; t->next!=process_list; t=t->next)
		printf("struct: 0x%x, code: 0x%x, priority: %d -> ", t, t->eip, t->priority);
	printf("struct: 0x%x, code: 0x%x, priority: %d", t, t->eip, t->priority);
	running_task = process_list; //Idle process enters the scene! Woo-hoo!
	//But... Khm, we need to switch to it.
	switch_task(NULL, NULL, running_task);   //Now it really enters the scene...for one timer tick.
}