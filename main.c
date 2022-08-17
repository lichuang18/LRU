#include <stdio.h>
#include <string.h>

#include "lru_cache.h"
#include "lru_cache_impl.h"
#include <map>
#include <iostream>

#define N 8*1024//创建N个缓存
#define NUM_KEY 1024*1024*1024
//随机1000万个key
//第一层8K个
//算一下miss和hit
int hit = 0;
using namespace std;
int main(int argc, char **argv){
    void *LruCache;
    /*
    std::map<int,char> oneMap;
    //三种插入方式
    oneMap.insert({1,'A'});
    oneMap.insert({2,'B'});
    oneMap.insert({3,'c'});
    for(auto it=oneMap.begin();it!=oneMap.end();it++){
		std::cout<<it->first<<"->"<<it->second<<std::endl; 
    }*/

    //创建缓存器
    if (0 == LRUCacheCreate(N, &LruCache))
        printf("缓存器创建成功,容量为%d\n",N);

    //向缓存器中添加数据
    u64 a_key = 123;
    u64 a_value = 1234;
    u64 *key = (u64 *)malloc(NUM_KEY * sizeof(u64));
    if (key == NULL) {
        printf("malloc error\n");
        exit(-1);
    }

    for (int i = 0; i < NUM_KEY; i++) {
        key[i] = (u64)random();
        u64 value = key[i]+1;
        if (0 != LRUCacheSet(LruCache, key[i], value))
            printf("%dth insert, put (%lld,%lld) failed\n",i,key[i],value);
        //printf("测试[%d]\n",i);
        if(i == N-1)
            printf("测试结束\n");
    }
    LRUCacheS *cache = (LRUCacheS *)LruCache;
    double hit= (double)cache->hit;
    printf("hit 次数 =%d,total insert %lld\n",hit,NUM_KEY);
    double hitrate=(double)(hit/(double)(1024*1024*1024));
    printf("hit_rate = %llf\n",hitrate);

/*
    if (0 != LRUCacheSet(LruCache, 123, 1234))
        printf("put (123, 1234) failed!\n");
    a_value = LRUCacheGet(LruCache,123);
    printf("a_value :%lld\n",a_value);
    if (0 != LRUCacheSet(LruCache, 5, 54))
        printf("put (key2, value2) failed!\n");
    if (0 != LRUCacheSet(LruCache, 3, 28))
        printf("put (key3, value3) failed!\n");
    LRUCachePrint(LruCache);
    a_value = LRUCacheGet(LruCache,123);
    printf("a_value :%lld\n",a_value);
    LRUCachePrint(LruCache);

    if (0 != LRUCacheSet(LruCache, 7, 68))
        printf("put (key4, value4) failed!\n");

    if (0 != LRUCacheSet(LruCache, 9, 46))
        printf("put (key5, value5) failed!\n");
    if (0 != LRUCacheSet(LruCache, 123, 46))
        printf("put (key5, value5) failed!\n");
    LRUCachePrint(LruCache);
    if (0 != LRUCacheSet(LruCache, 44444, 46))
        printf("put (key5, value5) failed!\n");
    LRUCachePrint(LruCache);
    if (0 != LRUCacheSet(LruCache, 77777, 46))
        printf("put (key5, value5) failed!\n");
    LRUCachePrint(LruCache);
    a_value = LRUCacheGet(LruCache,123);
    printf("a_value :%lld\n",a_value);
    LRUCachePrint(LruCache);
*/
    //销毁缓存器
    //if (0 != LRUCacheDestory(LruCache))
    //    printf("destory error");
}
