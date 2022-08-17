## [LRU_Cache](https://github.com/lichuang18/LRU)
基于双向链表和哈希表实现的LRU缓存，C语言混合C++实现。改编自[https://github.com/lichuang18/LRU](https://github.com/Stand1210/c-LRU-)

### 描述
+ LRU缓存置换算法
+ 基于双向链表和哈希表的存储模型
+ 基于键值对，键和值的数据类型为u64和指针
+ 没有全局存储结构的写操作上锁，并发安全（原实现有多线程）

### usage
> 基本执行
```
make
./main

in main.c file
#define N        //表示缓存大小
#define NUM_KEY  //表示执行多少次插入
```
在LRUCache结构体里添加了一个hit，表示命中次数，用于统计hit比率