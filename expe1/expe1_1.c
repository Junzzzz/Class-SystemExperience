#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    int p1, p2, fd[2];
    char buf[50], s[50];

    // 创建管道
    pipe(fd);

    // 循环至创建子进程1成功
    while ((p1 = fork()) == -1);
    if (p1 == 0) {
        // 获取锁
        lockf(fd[1], 1, 0);

        // 向字符数组写入字符串
        sprintf(buf, "Child process P1 is sending messages! \n");
        printf("Child process P1! \n");

        // 向管道写文本
        write(fd[1], buf, 50);

        // 休眠5秒
        sleep(5);

        // 释放锁
        lockf(fd[1], 0, 0);
        return (0);
    } else {
        // 与创建子进程1同理
        while ((p2 = fork()) == -1);
        if (p2 == 0) {
            lockf(fd[1], 1, 0);
            sprintf(buf, "Child process P2 is sending messages! \n");
            printf("Child process P2! \n");
            write(fd[1], buf, 50);
            sleep(5);
            lockf(fd[1], 0, 0);
            return (0);
        }

        // 等待子进程结束 并输出管道内容
        wait(0);
        if (read(fd[0], s, 50) == -1) printf("cannot read pipe! \n");
        else printf("%s", s);

        wait(0);
        if (read(fd[0], s, 50) == -1) printf("cannot read pipe! \n");
        else printf("%s", s);
        return (0);
    }
}
