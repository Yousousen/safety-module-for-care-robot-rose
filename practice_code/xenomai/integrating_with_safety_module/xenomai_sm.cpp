#include <stdio.h>
#include <unistd.h>

#include <signal.h>
#include <sys/mman.h>
#include <native/task.h>
#include <native/timer.h>
#include <rtdk.h>

class SafetyModule {
    public:
        void roll() {
            rt_printf("Neat...\n");
        }
};

RT_TASK sm_task;

void init_safety_module(void *arg) {
	RT_TASK *curtask;
	RT_TASK_INFO curtaskinfo;


	// inquire current task
	curtask=rt_task_self();
	rt_task_inquire(curtask,&curtaskinfo);
	// print task name
	rt_printf("Task name : %s \n", curtaskinfo.name);

    // Start the safety module.
    SafetyModule* sm = new SafetyModule();
    sm->roll();
    delete sm;
}


int main(int argc, char* argv[]) {
	char  task_name[10];
	// Perform auto-init of rt_print buffers if the task doesn't do so
	rt_print_auto_init(1);
	// Lock memory : avoid memory swapping for this program
	mlockall(MCL_CURRENT|MCL_FUTURE);

	rt_printf("start task\n");
	/*
	 * Arguments: &task,
	 *            name,
	 *            stack size (0=default),
	 *            priority,
	 *            mode (FPU, start suspended, ...)
	 */
	sprintf(task_name,"sm");
	rt_task_create(&sm_task, task_name, 0, 50, 0);
	/*
	 * Arguments: &task,
	 *            task function,
	 *            function argument
	 */
	rt_task_start(&sm_task, &init_safety_module, 0);
}
