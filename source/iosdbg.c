#include "iosdbg.h"

void *death_server(void *arg){
	while(1){
		if(debuggee->pid != -1){
			mach_port_type_t type;
			kern_return_t err = mach_port_type(mach_task_self(), debuggee->task, &type);

			if(type == MACH_PORT_TYPE_DEAD_NAME){
				printf("\n[%d dead]\n", debuggee->pid);
				cmdfunc_detach(NULL, 1);
				rl_on_new_line();
				rl_redisplay();
			}
		}

		sleep(1);
	}
}

// SIGINT handler
void interrupt(int show_prompt){
	if(debuggee->pid == -1)
		return;

	if(debuggee->interrupted)
		return;

	kern_return_t err = debuggee->suspend();

	if(err){
		printf("Cannot suspend: %s\n", mach_error_string(err));
		debuggee->interrupted = 0;

		return;
	}

	debuggee->interrupted = 1;

	if(show_prompt)
		rl_printf(RL_REPROMPT, "\nSuspended\n");
}


int main(int argc, char **argv, const char **envp){
	setup_initial_debuggee();
	rl_catch_signals = 0;

	signal(SIGINT, interrupt);

	char *line = NULL;

	// Until I can figure out how to correctly implement mach notifications, this will work fine.
	// Spawn a thread to check for the termination of the debuggee.
	pthread_t dt;
	pthread_create(&dt, NULL, death_server, NULL);

	while((line = readline("(iosdbg) ")) != NULL){
		// add the command to history
		if(strlen(line) > 0)
			add_history(line);

		// update the debuggee's linkedlist of threads
		if(debuggee->pid != -1){
			thread_act_port_array_t threads;
			mach_msg_type_number_t thread_count;

			kern_return_t err = task_threads(debuggee->task, &threads, &thread_count);
			
			if(err){
				printf("We couldn't update the list of threads for %d: %s\nDetaching...\n", debuggee->pid, mach_error_string(err));
				cmdfunc_detach(NULL, 1);
				continue;
			}

			debuggee->thread_count = thread_count;	
			
			machthread_updatethreads(threads);

			// we have to set a focused thread first, so set it to the first thread
			if(!machthread_getfocused())
				machthread_setfocused(threads[0]);
		}
		
		execute_command(line);

		free(line);
	}

	return 0;
}