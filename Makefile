main: main.c lru_cache.h lru_cache_impl.h lru_cache_impl.c 
	g++ -g -o main main.c lru_cache.h lru_cache_impl.h lru_cache_impl.c -lpthread 
clean:
	rm test example main