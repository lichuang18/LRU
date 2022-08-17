#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <semaphore.h>
#include "lru_cache.h"
#include "lru_cache_impl.h"
#include <map>
static void freeList(LRUCacheS *cache);


// LRU缓存及缓存单元相关接口及实现
// 创建一个缓存单元    创建一个节点
static cacheEntryS* newCacheEntry(u64 key, u64 data)
{  
    cacheEntryS* entry = NULL; 
    if (NULL == (entry=(cacheEntryS*)malloc(sizeof(*entry)))) {
        perror("malloc");
        return NULL;
    }
    entry->data = data;
    entry->key = key;
    return entry;
}

// 释放一个缓存单元 //
static void freeCacheEntry(cacheEntryS* entry)
{
    if (NULL == entry) return;
    free(entry);
}
// 双向链表相关接口及实现
// 从双向链表中删除指定节点 
static void removeFromList(LRUCacheS *cache, cacheEntryS *entry)
{
    //链表为空//
    if (cache->lruListSize==0) {
        return;
    }
    if (entry==cache->lruListHead && entry==cache->lruListTail) {
        // 链表中仅剩当前一个节点 //      
        cache->lruListHead = cache->lruListTail = NULL;
    } else if (entry == cache->lruListHead) {
        //欲删除节点位于表头//
        cache->lruListHead = entry->lruListNext;
        cache->lruListHead->lruListPrev = NULL;
    } else if (entry == cache->lruListTail) {
        //欲删除节点位于表尾//
        cache->lruListTail = entry->lruListPrev;
        cache->lruListTail->lruListNext = NULL;
    } else {
        //其他非表头表尾情况，直接摘抄节点
        entry->lruListPrev->lruListNext = entry->lruListNext;
        entry->lruListNext->lruListPrev = entry->lruListPrev;
    }
    //删除成功， 链表节点数减1
    cache->lruListSize--;
}

//将节点插入到链表表头
static cacheEntryS* insertToListHead(LRUCacheS *cache, cacheEntryS *entry)
{
    cacheEntryS *removedEntry = NULL;       
    ++cache->lruListSize;
    if (cache->lruListSize > cache->cacheCapacity) {
        //如果缓存满了， 即链表当前节点数已等于缓存容量， 
        //那么需要先删除链表表尾节点， 即淘汰最久没有被访问到的缓存数据单元
        removedEntry = cache->lruListTail;
        removeFromList(cache, cache->lruListTail);
    }
     //如果当前链表为空链表
    if (cache->lruListHead == NULL && cache->lruListTail == NULL) {
        cache->lruListHead = cache->lruListTail = entry;
    } else { //当前链表非空， 插入表头
        //entry 是新node  iru***head是旧node
        entry->lruListNext = cache->lruListHead;//new的next指向old
        entry->lruListPrev = NULL;              //new的prev指向null
        cache->lruListHead->lruListPrev = entry;//old的prev指向new
        cache->lruListHead = entry;             //更新list的表头为new
    }
    
    return removedEntry;
}

//辅助性接口， 用于保证最近访问的节点总是位于链表表头
static void updateLRUList(LRUCacheS *cache, cacheEntryS *entry)
{
    //将节点从链表中摘抄
    removeFromList(cache, entry);  //remove(   list  ,  node  )
    //将节点插入链表表头
    insertToListHead(cache, entry);
}

// 创建一个LRU缓存 //
int LRUCacheCreate(int capacity, void **lruCache)
{
    LRUCacheS* cache = NULL;
    //printf("size of  cache :%ld\n",sizeof(cache->hashMap));
    if (NULL == (cache=(LRUCacheS*)malloc(sizeof(*cache)))) {
        perror("molloc");
        return -1;
    }
    cache->lruListSize=0;
    cache->cacheCapacity = capacity;//缓存容量为3的LRUcache
    cache->hit=0;
    //cache->lruListHead=NULL;
    //cache->lruListTail=NULL;
    // test

    *lruCache = cache;
    return 0;
}

//从哈希表获取缓存单元


// 缓存存取接口
// 将数据放入LRU缓存中//
std::map<u64,cacheEntryS *> hashMap;

int LRUCacheSet(void *lruCache, u64 key, u64 data)
{

    LRUCacheS *cache = (LRUCacheS *)lruCache;
    //cacheEntryS *entry = getValueFromHashMap(cache, key);
    //从哈希表中查找数据是否已经存在缓存中//
    cacheEntryS *entry= NULL;
    //printf("size of  cache :%ld\n",sizeof(*entry));
    auto it = hashMap.find(key);
    if(it != hashMap.end())//缓存命中
    {
        cache->hit++;
        //更新数据， 将该数据更新到链表表头//
        entry = it->second;//节点信息
        entry->data=data;
        updateLRUList(cache, entry);
        //printf("insert key:%lld, value:%lld, node addr:%p,node->next%p\n",key,entry->data,entry,entry->lruListNext);
        //printf("cache size:%d  cache head:%p , cache tail:%p\n",cache->lruListSize,cache->lruListHead,cache->lruListTail);
    } else//缓存miss，包括第一次插入
    {
        entry = newCacheEntry(key, data);//创建新的缓存单元，也就是创建节点
        //将新建缓存单元插入链表表头//
        cacheEntryS *removedEntry = insertToListHead(cache, entry);
        if (NULL != removedEntry) {
            //将新建缓存单元插入链表表头过程中， 发生缓存满了的情况， 需要淘汰最久没有被访问到的缓存数据单元//         
            auto it = hashMap.find(removedEntry->key);
            delete it->second;
            it->second = NULL;
            hashMap.erase(it);
        }
        //将新建缓存单元插入哈希表//
        //printf("insert key:%lld, value:%lld, node addr:%p,node->next%p,full is empty:%p\n",key,entry->data,entry,entry->lruListNext,removedEntry);
        //printf("cache size:%d  cache head:%p , cache tail:%p\n",cache->lruListSize,cache->lruListHead,cache->lruListTail);
        //hashMap[key] = entry;
        hashMap.insert({key,entry});//崩了
    }
    return 0;
}

// 释放一个LRU缓存 //
int LRUCacheDestory(void *lruCache)
{
    LRUCacheS* cache = (LRUCacheS*)lruCache;
    if (NULL == cache) return 0;
    //free hashMap
    auto iter = hashMap.begin();
    while (iter != hashMap.end())
    {
        delete iter->second;
        iter->second = NULL;
        iter = hashMap.erase(iter);
    }
    //free linklist
    freeList(cache);
    free(cache);
    return 0;
}


//释放整个链表
static void freeList(LRUCacheS* cache)
{
    //链表为空
    if (0 == cache->lruListSize) {
        return;
    }
    
    cacheEntryS *entry = cache->lruListHead;
    //遍历删除链表上所有节点
    while(entry) {
        cacheEntryS *temp = entry->lruListNext;
        freeCacheEntry(entry);
        entry = temp;
    }
    cache->lruListSize = 0;
}

//从缓存中获取数据//
u64 LRUCacheGet(void *lruCache, u64 key)
{
    LRUCacheS *cache = (LRUCacheS *)lruCache;
    //从哈希表查找数据是否已经在缓存中//
    auto iter = hashMap.find(key);
    cacheEntryS* entry = NULL;
    if (iter != hashMap.end()) {
        //缓存中存在该数据， 更新至链表表头//
        entry = iter->second;
        updateLRUList(cache, entry);
        //返回数据//
        return entry->data;
    } else {
        //缓存中不存在相关数据//
        printf("cache miss\n");
        return 0;
    }
}

//调试接口


//遍历缓存列表， 打印缓存中的数据， 按访问时间从新到旧的顺序输出//
void LRUCachePrint(void *lruCache)
{
    LRUCacheS *cache = (LRUCacheS *)lruCache;
    if (NULL == cache || 0 == cache->lruListSize) {
        return ;
    }
    fprintf(stdout, "\n>>>>>>>>>>>>>>>\n");
    fprintf(stdout, "cache (key data):\n");
    cacheEntryS* entry = cache->lruListHead;
    while (entry) {
        fprintf(stdout, "(%lld, %lld)", entry->key, entry->data);
        entry = entry->lruListNext;
    }
    fprintf(stdout, "\n<<<<<<<<<<<<<<<\n");
}
