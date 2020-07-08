#include <stdio.h>
#include <pthread.h>
#include <unistd.h>         // usleep()使用

#define PRODUCER_NUM 5      // 生产者个数，假设5个
#define CONSUMER_NUM 2      // 消费者个数，假设2个

int data[5];                // 有界缓冲区个数，假设5
int size = 0;               // 有界缓冲区内数据个数统计
int num = 1;                // 记录生产或消费操作的顺序号 ，每次操作加1
int produce_id = 0;         // 生产者生成的产品编码(从1开始按顺序递增)，保存在data[]中

// 静态创建互斥锁
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
// 静态创建生产者端条件变量(生产者私用信号量)
pthread_cond_t full = PTHREAD_COND_INITIALIZER;
// 静态创建消费者端条件变量(消费者私用信号量)
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;

// 输出每步生产或消费操作后有界缓冲区内数据
void print() {
    int i;
    printf("\tline %2d:\t", num++);
    for (i = 0; i < size; i++)
        printf("%d   ", data[i]);
    printf("\n");
}

// 生产者函数
void *producer(void *p) {
    char c;
    int i;
    // 获取当前线程号
    long thd_pro_id = (long) pthread_self();
    // 每个生产者最多生产3件商品
    for (c = 'A'; c <= 'C'; c++) {
        // 对互斥锁mutex加锁
        pthread_mutex_lock(&mutex);
        // 有界缓冲区满，对生产者私有信号量full加锁
        while (size == 5) pthread_cond_wait(&full, &mutex);
        produce_id++;
        // 输出生产者线程id和产品编号
        printf("Producer %ld PUSH %d\n", thd_pro_id, produce_id);
        // 生产出的产品放入有界缓冲区
        data[size] = produce_id;
        // 线程暂停100微秒
        usleep(100);
        size++;
        print();
        // 给消费者线程发信号，唤醒消费者线程
        pthread_cond_broadcast(&empty);
        // 对互斥锁mutex解锁
        pthread_mutex_unlock(&mutex);
        usleep(2000);
    }
}


// 消费者函数
void *consumer(void *p) {
    int i;
    long thd_con_id = (long) pthread_self();
    // 每个消费者最多取10件商品
    for (i = 0; i < 10; i++) {
        pthread_mutex_lock(&mutex);
        while (size == 0)
            pthread_cond_wait(&empty, &mutex);
        printf("Consumer %ld POP %d\n", thd_con_id, data[size - 1]);
        size--;
        print();
        pthread_cond_broadcast(&full);
        pthread_mutex_unlock(&mutex);
        usleep(4000);
    }
}

int main() {
    // 保存PRODUCER_NUM个生产者线程id号
    pthread_t Thread_Prod[PRODUCER_NUM];
    // 保存CONSUMER_NUM个消费者线程id号
    pthread_t Thread_Cons[CONSUMER_NUM];

    int i;
    // 创建PRODUCER_NUM个生产者线程
    for (i = 0; i < PRODUCER_NUM; i++)
        pthread_create(&Thread_Prod[i], 0, producer, 0);
    // 创建CONSUMER_NUM个消费者线程
    for (i = 0; i < CONSUMER_NUM; i++)
        pthread_create(&Thread_Cons[i], 0, consumer, 0);
    // 等待生产者线程及资源的释放
    for (i = 0; i < PRODUCER_NUM; i++)
        pthread_join(Thread_Prod[i], 0);
    // 等待消费者线程及资源的释放
    for (i = 0; i < CONSUMER_NUM; i++)
        pthread_join(Thread_Cons[i], 0);
    // 注销互斥锁
    pthread_mutex_destroy(&mutex);
    // 注销条件变量
    pthread_cond_destroy(&full);
    pthread_cond_destroy(&empty);

    return 0;
}