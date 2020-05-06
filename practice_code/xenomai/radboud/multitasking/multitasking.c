#define NTASKS 5
RT_TASK task[NTASKS];

void demo(void *arg) {
	RT_TASK_INFO curtaskinfo;
	rt_task_inquire(NULL, &curtaskinfo);
	rt_printf("Task name: %s ", curtaskinfo.name);
}
