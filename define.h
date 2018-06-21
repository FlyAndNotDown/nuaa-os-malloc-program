#ifndef MALLOC_SIMULATOR_DEFINE_H
#define MALLOC_SIMULATOR_DEFINE_H

// 默认内存大小
#define DEFAULT_MEM_SIZE 2048
// 缓冲区大小
#define BUFFER_SIZE 1024
// shell提示符
#define PROMPT "malloc-simulator$"
// 欢迎文字
#define WELCOME "welcome to malloc-simulator(best-fit), enter 'help' to get some help!"

// 内存块
typedef struct {
    int start;
    int length;
} MemBlock;

// 进程
typedef struct {
    int pid;
    char name[50];
    MemBlock *memBlock;
} Process;

// 重置程序
void reset(int);
// 一系列事件函数
void setMemSize();
void showMemInfo();
void showProcessInfo();
void newProcess();
void deleteProcess();
void help();
void about();
// 输出一条分割线
void printSplitLine();
// 获取一个新的pid
int getNewPid();

#endif //MALLOC_SIMULATOR_DEFINE_H
