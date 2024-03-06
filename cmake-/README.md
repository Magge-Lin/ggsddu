
学习CMake后的笔记，以及用来练习CMake.

执行方式：
1、./go.sh
2、./build/src/helloworld 

文件结构：

├── build
├── CMakeLists.txt
├── README.md
└── src
    ├── CMakeLists.txt
    ├── hello
    │   ├── CMakeLists.txt
    │   ├── hello.c
    │   └── hello.h
    ├── main.c
    └── world
        ├── CMakeLists.txt
        ├── world.c
        └── world.h

Tips:
分别使用了aux将所有文件放一起编译的方式，以及使用链接动态库/静态库的编译方式

