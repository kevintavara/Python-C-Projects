#include <thread.h>
#include <syscall.h>

pid_t fork(void) {
	thread_create("child");
}