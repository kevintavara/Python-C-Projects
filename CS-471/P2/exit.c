#include <unistd.h>
#include <thread.h>
#include <curthread.h>
#include <syscall.h>
#include <kern/types.h>

void
sys_exit(int exitcode) 
{
	pid_t temp = waitpid(curthread->t_pid, exitcode, 0);
	
	thread_exit();
	
	panic("Thread did not exit!");
}