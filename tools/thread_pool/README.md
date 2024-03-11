# 线程池
> thread_pool


## 编译
> ./build.sh

## 执行
> ./build/bin/main
> ./build/bin/test


## 目录结构
- src

```
1、基于 C 语言实现的库
		atomic.h
		spinlock.h
		thrd_pool.c
		thrd_pool.h

2、基于 C++11 线程安全的单例模式的线程池封装
		CThreadPool.h
		CThreadPool.cc
```



- test

```
简单测试：
main.c：
	打印1~1000数字，并且打印当前线程ID.

test.cc：
	对vector容器进行push和erase.
```

