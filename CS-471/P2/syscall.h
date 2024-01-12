#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#include <kern/types.h>

/*
 * Prototypes for IN-KERNEL entry points for system call implementations.
 */

int sys_reboot(int code);

void sys_exit(int exitcode);

pid_t sys_waitpid(pid_t pid, int *status, int options);

pid_t getppid(void);

pid_t getpid(void);

pid_t fork(void);

int execv(const char *program, char **args);

#endif /* _SYSCALL_H_ */
