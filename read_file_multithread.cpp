#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_THREADS 8

// 线程参数结构体
typedef struct {
    int id; // 线程编号
    FILE *fp; // 文件指针
    long start_pos; // 起始位置
    long length; // 读取长度
    char *buf; // 缓冲区
} thread_param_t;

// 线程函数
void* read_file_thread(void *arg)
{
    thread_param_t *param = (thread_param_t*)arg;
    fseek(param->fp, param->start_pos, SEEK_SET);
    fread(param->buf, 1, param->length, param->fp);
    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: %s <file path>\n", argv[0]);
        return 0;
    }

    char *file_path = argv[1];
    FILE *fp;
    long file_size;
    int num_threads = MAX_THREADS;
    pthread_t threads[MAX_THREADS];
    thread_param_t params[MAX_THREADS];
    char *buf;
    int i, rc;

    // 打开文件，获取文件大小
    fp = fopen(file_path, "rb");
    if (fp == NULL) {
        perror("open file error");
        return 1;
    }
    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    rewind(fp);

    // 计算每个线程读取的长度和起始位置
    long block_size = (file_size + num_threads - 1) / num_threads;
    buf = (char*)malloc(file_size);
    for (i = 0; i < num_threads; i++) {
        params[i].id = i;
        params[i].fp = fp;
        params[i].start_pos = i * block_size;
        params[i].length = (i+1) * block_size <= file_size ? block_size : file_size - i * block_size;
        params[i].buf = buf + i * block_size;
    }

    // 创建并等待所有线程
    for (i = 0; i < num_threads; i++) {
        rc = pthread_create(&threads[i], NULL, read_file_thread, &params[i]);
        if (rc != 0) {
            fprintf(stderr, "Create thread %d failed\n", i);
            return 1;
        }
    }
    for (i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    // 合并缓冲区数据
    // TODO

    fclose(fp);
    free(buf);
    return 0;
}
