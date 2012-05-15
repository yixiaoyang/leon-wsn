#include <cyg/kernel/kapi.h>

#include <math.h>
#include <stdio.h>
cyg_thread thread_s[2];
char stack[2][4096];
cyg_handle_t threadA, threadB;
cyg_thread_entry_t test;
cyg_mutex_t cliblock;

void cyg_user_start(void)
{
	printf("Enter cyg_user_start\n");

	cyg_mutex_init(&cliblock);
	/* Create thread A */
	cyg_thread_create(4, test, (cyg_addrword_t)0, "thread A", \
		(void *)stack[0], 4096, &threadA, &thread_s[0]);
	/* Create thread B */
	cyg_thread_create(4, test, (cyg_addrword_t)1, "thread B", \
		(void *)stack[1], 4096, &threadB, &thread_s[1]);

	cyg_thread_resume(threadA);
	cyg_thread_resume(threadB);

}

void test(cyg_addrword_t data)
{
	int message = (int)data;
	int delay;
	printf("Beginning execution; Thread data is %d\n",message);
	cyg_thread_delay(200);
	for(;;) {
		delay = 200 + rand()%50;
		cyg_mutex_lock(&cliblock);
		printf("Thread %d: now start delay of %d clock ticks\n", message, delay);
		cyg_mutex_unlock(&cliblock);
		cyg_thread_delay(delay);
	}
}
// End of twothreads.c
