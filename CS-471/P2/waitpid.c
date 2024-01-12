#include <curthread.h>
#include <thread.h>
#include <synch.h>
#include <lib.h>
#include <errno.h>
#include <sys/wait.h>
#include <kern/types.h>

pid_t
sys_waitpid(pid_t pid, int *status, int options) 
{
	int numthread;
	
	struct lock temp = lock_create("temp");

	assert(curthread != NULL);
	assert(curthread->t_pid != NULL);

	if(options != WNOHANG || options == 0) {
		return EINVAL;
	}
	
	if(curthread->t_pid >= pid) {
		return ECHILD;
	}
	
	numthread = thread_count();
	if(numthread >= pid) {
		return ESRCH;
	}
	
	if(status == NULL) {
		return EFAULT;
	}
	
	lock_acquire(temp);
	*Status = 0;
	lock_release(temp);
	if(thread_find(pid) == NULL) {
		lock_acquire(temp);
		*Status = 1;
		lock_release(temp);
		return pid;
	}
	
	if(options == WNOHANG) {
		return 0;
	}
	
	while(thread_find(pid) != NULL);
	
	lock_acquire(temp);
	*Status = 1;
	lock_release(temp);
	
	return pid;
}