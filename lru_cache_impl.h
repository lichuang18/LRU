

#ifndef LRUCACHEIMPL_H   //先测试是否定义过，如果没有 接下来定义这个
#define LRUCACHEIMPL_H


#include <semaphore.h>
#include <map>

typedef unsigned long long u64; 
/*定义LRU缓存的缓存单元*/
typedef struct cacheEntryS{
    u64 key;   /* 数据的key,缓存容量的大小*/
    u64 data;  /* 数据的data*/

    //struct cacheEntryS *hashListPrev;   /* 缓存哈希表指针， 指向哈希链表的前一个元素 */
    //struct cacheEntryS *hashListNext;   /* 缓存哈希表指针， 指向哈希链表的后一个元素 */

    struct cacheEntryS *lruListPrev;    /* 缓存双向链表指针， 指向链表的前一个元素 */
    struct cacheEntryS *lruListNext;    /* 缓存双向链表指针， 指向链表后一个元素 */
}cacheEntryS;

/* 定义LRU缓存 */
typedef struct LRUCacheS{
    u64 cacheCapacity;  /*缓存的容量*/
    u64 lruListSize;    /*缓存的双向链表节点个数*/
    int hit;
    //std::map<u64,cacheEntryS *> hashMap;
    cacheEntryS *lruListHead;   /*缓存的双向链表表头*/
    cacheEntryS *lruListTail;   /*缓存的双向链表表尾*/
}LRUCacheS;
#endif 
