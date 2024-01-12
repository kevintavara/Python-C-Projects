#include <curthread.h>
#include <synch.h>
#include <lib.h>
#include <unistd.h>
#include <syscall.h>
#include <kern/types.h>

pid_t
sys_getpid(void) {
	pid_t pid;

	assert(curthread != NULL);
	assert( curthread->t_pid != NULL);
	
	curthread->t_lock = lock_create("t_lock");
	
	lock_acquire(curthread->t_lock);
	pid = curthread->t_pid;
	lock_release(curthread->t_lock);
	
	return pid;
}