
/**
 * @file join_no_lock.c
 * @author zhangm365
 * @brief 子进程未持锁修改谓词并 signal 的情况下，允许在“等待方检查谓词”和“真正进入等待”之间（且等待方还拿着锁时）把谓词修改为真并发信号。
 * 此时没有等待者，信号丢失，然后等待者就会阻塞挂起。
 * @version 0.1
 * @date 2025-09-15
 * 
 * 
 */

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "common.h"
#include "common_threads.h"

pthread_cond_t  c = PTHREAD_COND_INITIALIZER;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
int done = 0;

void *child(void *arg) {
    printf("child: begin\n");
    sleep(1);
    done = 1;
    printf("child: signal\n");
    Cond_signal(&c);
    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t p;
    printf("parent: begin\n");
    Pthread_create(&p, NULL, child, NULL);
    Mutex_lock(&m);
    printf("parent: check condition\n");
    while (done == 0) {
        sleep(2);
        printf("parent: wait to be signalled...\n");
        Cond_wait(&c, &m);    // 信号丢失，一直阻塞挂起。
    }
    Mutex_unlock(&m);
    printf("parent: end\n");
    return 0;
}

