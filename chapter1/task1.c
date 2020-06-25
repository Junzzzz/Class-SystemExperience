//
// Created by Jun on 5/31/20.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>

int pid1, pid2;

int exp_task1() {
    int fd[2];
    char out_pipe[50], in_pipe[50];
    // 创建一个管道
    pipe(fd);
    while ((pid1 = fork()) == -1);
    if (pid1 == 0) {
        lockf(fd[1], 1, 0);
        // 把串放入数组out_pipe中
        sprintf(out_pipe, "Child 1 process is sending message!");
        // 向管道写长为50字节的串
        write(fd[1], out_pipe, 50);
        // 自我阻塞5秒
        sleep(5);
        lockf(fd[1], 0, 0);
        exit(0);
    } else {
        while ((pid2 = fork()) == -1);
        if (pid2 == 0) {
            // 互斥
            lockf(fd[1], 1, 0);
            sprintf(out_pipe, "Child 2 process is sending message!");
            write(fd[1], out_pipe, 50);
            sleep(5);
            lockf(fd[1], 0, 0);
            exit(0);
        } else {
            // 同步
            wait(0);
            // 从管道中读长为50字节的串
            read(fd[0], in_pipe, 50);
            printf("%s\n", in_pipe);
            wait(0);
            read(fd[0], in_pipe, 50);
            printf("%s\n", in_pipe);
            exit(0);
        }
    }
}