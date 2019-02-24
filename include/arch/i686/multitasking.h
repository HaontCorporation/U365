#ifndef MULTITASKING_H
#define MULTITASKING_H
#include <stdint.h>
#include <arch/i686/regs.h>
enum task_priority {
	PRIORITY_IDLE    = 1,  //Only for idle process.
	PRIORITY_LOW     = 5,
	PRIORITY_NORMAL  = 10, //Recommended for use by all regular processes.
	PRIORITY_HIGH    = 20,
	PRIORITY_MAXIMAL = 40, //It will slow down all processes with other priorities, but will work maximally fast.
};
/*
 * Reserved PIDs:
 * 
 * - 0 is idle process.
 * - 1 is init process. This is every process' parent.
 */
struct task
{
	struct task *prev;
	//We need to check if the process is dead, so on next PIT tick it will be removed from process list.
	uint8_t alive;      //On new process, it's set to 1.
	uint16_t time_out;  //Time before task switch
	int8_t exit_status; //Contains 0 when the process is alive. kill_process() sets it to provided value, nulls all register data, frees stack memory and makes alive field == 1.
	enum task_priority priority;
	uint32_t eax, ebx, ecx, edx, esi, edi, ebp, cr3;
	struct task *next;
	uint32_t ss;
	void *stack; //Stack needs to be separated from kernel and other processes' stack.
	uint32_t eflags;
	uint32_t cs;
	uint32_t eip;
} __attribute__((packed));
extern struct task *process_list;
extern struct task *running_task;
void init_multitasking();
void switch_task(struct task*, regs*, struct task*);
void pit_do_multitasking_work(regs *); //eeeeeeeee long function namesssss
#endif