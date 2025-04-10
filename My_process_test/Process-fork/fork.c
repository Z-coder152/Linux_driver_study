#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sched.h>
#include <signal.h>


#define STACK_SIZE 1024 * 1024

// clone 函数的子进程执行函数
static int child_func(void *arg) {
    printf("Clone child process is running.\n");
    return 0;
}

int main() {
    pid_t pid;
    char *stack;
    char *stack_top;

    // 使用 fork 创建子进程
    pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    } else if (pid == 0) {
        printf("Fork child process is running.\n");
        exit(0);
    } else {
        wait(NULL); // 等待子进程结束
    }

    // 使用 vfork 创建子进程
    pid = vfork();
    if (pid < 0) {
        perror("vfork");
        return 1;
    } else if (pid == 0) {
        printf("Vfork child process is running.\n");
        _exit(0);
    } else {
        wait(NULL); // 等待子进程结束
    }

    // 使用 clone 创建子进程
    stack = (char *)malloc(STACK_SIZE);
    if (stack == NULL) {
        perror("malloc");
        return 1;
    }
    stack_top = stack + STACK_SIZE;
    pid = clone(child_func, stack_top, CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND, NULL);
    if (pid < 0) {
        perror("clone");
        free(stack);
        return 1;
    } else {
        waitpid(pid, NULL, 0); // 等待子进程结束
        free(stack);
    }

    // 使用 exec 执行新程序
    pid = fork();
    if (pid < 0) {
        perror("fork for exec");
        return 1;
    } else if (pid == 0) {
        char *args[] = {"/bin/ls", "-l", NULL};
        execvp(args[0], args);
        perror("execvp");
        _exit(1);
    } else {
        wait(NULL); // 等待子进程结束
    }

    return 0;
}    