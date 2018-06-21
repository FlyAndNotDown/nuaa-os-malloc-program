# malloc-simulator
一个C++编写的模拟内存分配程序(使用BF算法)，NUAA 2018 OS小作业

# 使用
## 编译
**该程序应该在Unix上运行，Windows兼容性不确定**

在项目根目录下使用cmake进行编译：
```
cmake .
```

make target：
```
make
```

## 运行
```
./malloc_simulator
```
如果不行，尝试先：
```
chmod 777 ./malloc_simulator
```

## 指令
基于cli的思想制作，相当于一个专用的小型终端，你可以直接在里面进行操作：
```
welcome to malloc-simulator(best-fit), enter 'help' to get some help!
malloc-simulator$
```
诸如这样的，就是你的用户界面，这里给出所有指令的一览：
```
// 设置总内存大小 [内存大小]
setMemSize [memSize]

// 显示当前内存使用情况
showMemInfo

// 显示当前进程信息
showProcess

// 新建进程 [进程名] [分配内存大小]
newProcess [processName] [processMemSize]

// 删除进程 [进程编号pid]
deleteProcess [pid]

// 寻求帮助
help

// 关于
about

// 重置当前内存
reset
```

# 说明
## BF算法
优先匹配最适合当前线程的内存块并使用之，避免浪费，但BF算法并不是最优算法

## 程序思想
使用三个链表，一个是已分配内存链表，一个是空闲内存链表，还有一个是进程表
