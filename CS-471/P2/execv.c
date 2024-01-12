#include <thread.h>
#include <syscall.h>


int execv(const char *program, char **args) {
	thread_create(program);
}