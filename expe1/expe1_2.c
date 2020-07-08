#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

int p1, p2;

void ppdo();
void p1do();
void p2do();

int main() {
    // 监听中断信号 Ctrl + C
    signal(SIGINT, ppdo);

    // 创建子进程1
    p1 = fork();
    if (p1 == 0) {
        // 监听信号1
        signal(SIGUSR1, p1do);
        for (;;);
    } else {
        // 创建子进程2
        p2 = fork();
        if (p2 == 0) {
            // 监听信号2
            signal(SIGUSR2, p2do);
            for (;;);
        }
    }

    // 等待两个子进程结束
    wait(0);
    wait(0);

    // 输出结束提示
    printf("\nParent process is killed!\n");
    exit(0);
}

void ppdo() {
    // 向两个子进程发送信号
    kill(p1, SIGUSR1);
    kill(p2, SIGUSR2);
}

void p1do() {
    printf("\nChild process p1 is killed by parent!\n");
    exit(0);
}

void p2do() {
    printf("\nChild process p2 is killed by parent!\n");
    exit(0);
}
