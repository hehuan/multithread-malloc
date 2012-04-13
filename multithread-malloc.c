#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/time.h>
#include <pthread.h>

#define FALSE 0
#define TRUE 1

pid_t gettid(void);
void *membench(void *arg);


// スレッド引数の構造体
typedef struct {
    int mallocSize;     // 確保するメモリサイズ(MB)
    int count;          // 一回のループでの確保数
    int loop;           // ループ回数
    int verbose;       // 詳細出力
} THREAD_ARGS;


// メイン
int main(int argc, char **argv){

    int c;
    int threadNum = 1;

    THREAD_ARGS thread_args;
    thread_args.mallocSize = 1;
    thread_args.count = 1;
    thread_args.loop = 1;
    thread_args.verbose = FALSE;

    static struct option long_options[] =
    {
        {"threads", required_argument, 0, 't'},
        {"size", required_argument, 0, 's'},
        {"count", required_argument, 0, 'c'},
        {"loop", required_argument, 0, 'l'},
        {"verbose", no_argument, 0, 'v'},
        {0, 0, 0, 0}
    };

    int option_index = 0;

    while((c = getopt_long( argc, argv, "t:s:c:l:v", long_options, &option_index)) != -1) {

        switch(c) {
            case 't':
                threadNum = atoi(optarg);
                break;
            case 's':
                thread_args.mallocSize = atoi(optarg);
                break;
            case 'c':
                thread_args.count = atoi(optarg);
                break;
            case 'l':
                thread_args.loop = atoi(optarg);
                break;
            case 'v':
                thread_args.verbose = TRUE;
                break;
            case ':':
            case '?':
                fprintf(stderr, "Usage: %s --thread=THREADNUMBER --size=MALLOCSIZE --count=MALLOCCOUNT --loop=LOOPCOUNT\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    int i;
    pthread_t p[threadNum];
    struct timeval t0, t1;

    printf("Thread Number                   : %d\n", threadNum);
    printf("Malloc Size                     : %d MB\n", thread_args.mallocSize);
    printf("Malloc Concurrent Number/thread : %d\n", thread_args.count);
    printf("Loop count/Thread               : %d\n", thread_args.loop);

    // 開始時刻取得
    gettimeofday(&t0, NULL);

    // スレッド生成
    for( i = 0; i < threadNum; i++) {
        pthread_create(&p[i], NULL, membench, &thread_args);
    }

    // スレッド終了捕捉
    for( i = 0; i < threadNum; i++) {
        pthread_join(p[i], NULL);
    }

    // 終了時刻取得
    gettimeofday(&t1, NULL);

    // 経過時刻算出
    t1.tv_sec -= t0.tv_sec;
    if (t1.tv_usec < t0.tv_usec) {
      t1.tv_sec -=1;
      t1.tv_usec += 1000000 - t0.tv_usec;
    } else {
      t1.tv_usec -= t0.tv_usec;
    }

    printf("%d.%06d sec\n", (int)t1.tv_sec, (int)t1.tv_usec);

    exit(EXIT_SUCCESS);
}

// スレッドID取得用関数
pid_t gettid(void) {
    return syscall(SYS_gettid);
}

// メモリ負荷用関数
void *membench(void *arg) {

    THREAD_ARGS *thread_args = (THREAD_ARGS*)arg;

    int mallocSize = thread_args->mallocSize;
    int count      = thread_args->count;
    int loop       = thread_args->loop;
    int verbose    = thread_args->verbose;

    int *mem[count];
    int size[count];

    int i, loopcnt;

    while (1) {

        loopcnt++;

        if ( loop != 0 && loopcnt > loop) {
            break;
        }

        // メモリ確保
        for(i = 0; i < count; i++) {
            size[i] = ( rand() % (mallocSize * 10)  + 1 ) * 100 * 1000;
            mem[i]  = (int *)malloc( size[i] );

            if (!mem[i]) {
                puts("can't malloc");
                return NULL;
            }
            memset(mem[i], 1, size[i]);
            memset(mem[i], 0, size[i]);
            if (verbose) {
              printf("thread: %d, loop: %d, mem[%d] %d Bytes malloc\n", gettid(), loopcnt, i, size[i]);
            }
        }

        // メモリ解放
        for(i = 0; i < count; i++) {
            free(mem[i]);
            if (verbose) {
              printf("thread: %d, loop: %d, mem[%d] %d Bytes free\n", gettid(), loopcnt, i, size[i]);
            }
        }
    }

    return NULL;
}
