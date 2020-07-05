//
// Created by Jun on 5/31/20.
//
#include "../experience.h"
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <wait.h>

#define NUM 2

int pid[NUM];
int child;

void signal_handler(int signal) {
    printf("child: %d\n", child);
    if (signal == SIGINT) {
        printf("this:%d\n", getpid());
        printf("kill:%d return:%d\n", pid[0], kill(pid[0], SIGUSR1));
//        printf("return:%d\n",  waitpid(pid[0], NULL, 0));

//        printf("%d\n", pid[1]);
//        kill(pid[1], SIGUSR2);
//        waitpid(pid[1], NULL, 0);
        printf("parent exit!\n");
        exit(EXIT_SUCCESS);
    } else {
        printf("waiting...\n");
        sleep(5);
        printf("child exit!\n");
        exit(EXIT_SUCCESS);
    }
}

void test(int sig) {
    printf("233\n");
}

void proc_child(int id) {
    while ((pid[id] = fork()) == -1);
    if (pid[id] == 0) {
        // 子进程
        child = 1;
    }

}

int exp_task2() {
    child = 0;
    for (int i = 0; i < NUM && !child; i++) {
        proc_child(i);
    }

    if (!child) {
        signal(SIGINT, signal_handler);
    } else {
        if (signal(SIGUSR1, signal_handler) == SIG_ERR) { //设置出错
            perror("Can't set handler for SIGUSR1\n");
        }
    }
    while (1) {
        if (!child) {
            printf("开始休眠一秒钟...\n");
        }
        sleep(1);
    }
    return 0;
}
