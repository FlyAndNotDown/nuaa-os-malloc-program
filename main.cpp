#include <iostream>
#include <list>
#include <string.h>
#include <iomanip>
using namespace std;
#include "define.h"

// 内存大小
int memSize = 0;
// 已经使用过的内存块链表
list<MemBlock *> usedMemBlockList;
// 未使用过的内存块链表
list<MemBlock *> freeMemBlockList;
// 进程链表
list<Process *> processList;

// 输入缓冲区
char buffer[BUFFER_SIZE];

// pid计数器
int pidCount = 0;

int main(int argc, char *argv[]) {
    // 重置程序
    reset(DEFAULT_MEM_SIZE);

    char *p = NULL;

    cout << WELCOME << endl;

    // 进入主循环
    while (true) {
        cout << PROMPT;
        // 清空缓冲区
        memset(buffer, 0, BUFFER_SIZE);
        // 读入一行
        cin.getline(buffer, BUFFER_SIZE);
        // 分割字符串
        p = strtok(buffer, " ");
        if (p) {
            // 根据不同的指令做不同的事情
            if (!strcmp(p, "exit")) {
                break;
            } else if (!strcmp(p, "setMemSize")) {
                setMemSize();
            } else if (!strcmp(p, "showMemInfo")) {
                showMemInfo();
            } else if (!strcmp(p, "showProcessInfo")) {
                showProcessInfo();
            } else if (!strcmp(p, "newProcess")) {
                newProcess();
            } else if (!strcmp(p, "deleteProcess")) {
                deleteProcess();
            } else if (!strcmp(p, "help")) {
                help();
            } else if (!strcmp(p, "about")) {
                about();
            } else if (!strcmp(p, "reset")) {
                reset(memSize);
            } else {
                cout << "unknown instruction!" << endl;
            }
        }
    }

    return 0;
}

void reset(int size) {
    // 首先将内存大小设置为size
    memSize = size;

    // 清空所有链表
    for (MemBlock *i : usedMemBlockList) delete i;
    for (MemBlock *i : freeMemBlockList) delete i;
    for (Process *i : processList) delete i;
    usedMemBlockList.clear();
    freeMemBlockList.clear();
    processList.clear();

    // 添加一块大小为size的空内存至链表
    MemBlock *free = new MemBlock;
    free->start = 0;
    free->length = memSize;
    freeMemBlockList.push_back(free);
}

void setMemSize() {
    // 获取参数
    char *p;
    int size = DEFAULT_MEM_SIZE;
    p = strtok(NULL, "");
    if (!p) {
        cout << "have no enough args" << endl;
        return;
    }
    sscanf(p, "%d", &size);

    // 重置
    reset(size);

    cout << "mem size changed. all of mem and process info have been reset." << endl;
}

void showMemInfo() {
    printSplitLine();
    // 先输出概况
    int allFreeSize = 0, allUsedSize = 0;
    for (MemBlock *i : freeMemBlockList) allFreeSize += i->length;
    for (MemBlock *i : usedMemBlockList) allUsedSize += i->length;
    cout << "main mem info:" << endl;
    cout << "\ttotal: " << memSize
         << "\t\tused: " << allUsedSize
         << "\t\tfree: " << allFreeSize
         << endl;
    cout << endl;
    cout << "used mem info:" << endl;
    cout << "\tfrom\tto  \tsize\townerProcess" << endl;
    // 输出已经被使用的内存块情况
    for (Process *i : processList) {
        MemBlock *used = i->memBlock;
        cout << "\t" << setiosflags(ios::left) << setw(7) << used->start
             << " " << setiosflags(ios::left) << setw(7) << used->start + used->length - 1
             << " " << setiosflags(ios::left) << setw(7) << used->length
             << " " << setiosflags(ios::left) << i->name << "(" << i->pid << ")"
             << endl;
    }
    if (processList.size() == 0) cout << "\t" << "none" << endl;
    cout << endl;
    // 输出空闲内存块情况
    cout << "free mem info:" << endl;
    cout << "\tfrom\tto  \tsize" << endl;
    for (MemBlock *i : freeMemBlockList)
        cout << "\t" << setiosflags(ios::left) << setw(7) << i->start
             << " " << setiosflags(ios::left) << setw(7) << i->start + i->length - 1
             << " " << setiosflags(ios::left) << i->length
             << endl;
    if (freeMemBlockList.size() == 0) cout << "\t" << "none" << endl;
    printSplitLine();
}

void showProcessInfo() {
    printSplitLine();
    // 先输出进程数量
    cout << "main process info:" << endl;
    cout << "\tnum: " << processList.size() << endl;
    cout << endl;
    // 输出线程详情
    cout << "details: " << endl;
    cout << "\tpid \tname\t\t\tmem" << endl;
    for (Process *i : processList)
        cout << "\t" << setiosflags(ios::left) << setw(7) << i->pid
             << " " << setiosflags(ios::left) << setw(15) << i->name
             << " " << setiosflags(ios::left) << i->memBlock->length
             << endl;
    if (processList.size() == 0) cout << "\tnone" << endl;
    printSplitLine();
}

void newProcess() {
    // 获取参数
    char name[50];
    memset(name, 0, 50);
    int mem = 0;
    char *p;
    p = strtok(NULL, " ");
    if (!p) {
        cout << "have no enough args" << endl;
        return;
    }
    strcpy(name, p);
    p = strtok(NULL, " ");
    if (!p) {
        cout << "have no enough args" << endl;
        return;
    }
    sscanf(p, "%d", &mem);

    // 将所有的空闲内存按照从小到大的顺序排序
    freeMemBlockList.sort([=](MemBlock *a, MemBlock *b) -> bool {
        return a->length < b->length;
    });

    // 寻找一块最合适的内存
    MemBlock *bestFit = nullptr;
    for (MemBlock *i : freeMemBlockList)
        if (mem <= i->length) {
            bestFit = i;
            break;
        }
    if (bestFit) {
        // 如果这一块内存刚好跟目标大小一样大
        if (mem == bestFit->length) {
            // 从空闲区域移出原来的元素
            freeMemBlockList.remove(bestFit);
            // 将之添加到已经使用区域
            usedMemBlockList.push_back(bestFit);
            // 创建一个新的进程描述d
            Process *process = new Process;
            process->pid = getNewPid();
            strcpy(process->name, name);
            process->memBlock = bestFit;
            // 将进程描述添加到进程表中
            processList.push_back(process);
            // 否则
        } else {
            // 从空闲区移出原来的元素
            freeMemBlockList.remove(bestFit);
            // 将之划分成两份
            MemBlock *free = new MemBlock;
            MemBlock *used = new MemBlock;
            used->start = bestFit->start;
            used->length = mem;
            free->start = used->start + used->length;
            free->length = bestFit->start + bestFit->length - free->start;
            // 将两份分别添加到对应区域
            freeMemBlockList.push_back(free);
            usedMemBlockList.push_back(used);
            // 删除原来的内存块
            delete bestFit;
            // 创建一个新的进程描述
            Process *process = new Process;
            process->pid = getNewPid();
            strcpy(process->name, name);
            process->memBlock = used;
            // 将进程描述添加到进程表中
            processList.push_back(process);
        }
    } else {
        cout << "can't create new process, reason: no enough mem" << endl;
    }
}

void deleteProcess() {
    // 获取参数
    int pid = 0;
    char *p = strtok(NULL, " ");
    if (!p) {
        cout << "have no enough args" << endl;
        return;
    }
    sscanf(p, "%d", &pid);
    // 在链表中寻找具有该pid的进程
    Process *process = nullptr;
    for (Process *i : processList)
        if (i->pid == pid) process = i;
    // 如果找到了
    if (process) {
        // 将process从链表中移出
        processList.remove(process);
        // 将process用过的内存块从使用过的内存链表中移出并且加入空闲内存链表
        usedMemBlockList.remove(process->memBlock);
        freeMemBlockList.push_back(process->memBlock);
        // 将空闲内存链表按照起始地址排序
        freeMemBlockList.sort([=](MemBlock *a, MemBlock *b) -> bool {
            return a->start < b->start;
        });
        // 看这些链表中有没有可以合并的项
        list<MemBlock *> temp;
        while (!freeMemBlockList.empty()) {
            MemBlock *block = freeMemBlockList.front();
            freeMemBlockList.pop_front();
            while (!freeMemBlockList.empty()) {
                MemBlock *another = freeMemBlockList.front();
                freeMemBlockList.pop_front();
                if (block->start + block->length == another->start) {
                    block->length = block->length + another->length;
                    delete another;
                } else {
                    temp.push_front(another);
                    break;
                }
            }
            temp.push_back(block);
        }
        freeMemBlockList.clear();
        for (MemBlock *i : temp) freeMemBlockList.push_back(i);
        // 删除process
        delete process;
        // 将空闲内存链表按照起始地址排序
        freeMemBlockList.sort([=](MemBlock *a, MemBlock *b) -> bool {
            return a->start < b->start;
        });
    } else cout << "no process was found with that pid!" << endl;
}

void help() {
    cout << "-- setMemSize [memSize]" << endl
         << "-- showMemInfo" << endl
         << "-- showProcessInfo" << endl
         << "-- newProcess [processName] [processMemSize]" << endl
         << "-- deleteProcess [pid]" << endl
         << "-- help" << endl
         << "-- about" << endl
         << "-- reset" << endl;
}

void about() {
    cout << "little toy by NUAA 161520311 John Kindem" << endl;
}

void printSplitLine() {
    cout << "----------------------------------------------------------" << endl;
}

int getNewPid() {
    return ++pidCount;
}