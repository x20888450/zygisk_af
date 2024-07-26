#ifndef ALLINCLUDE_H
#define ALLINCLUDE_H

// C Standard Library
// C 标准库
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

// 将以下 C 头文件用 extern "C" 包裹
extern "C" {
#include <unistd.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <dlfcn.h>
#include <bits/getopt.h>
#include <asm/unistd.h>
#include <asm/fcntl.h>
#include "limits.h"
}

// JNI
// Java 本地接口
#include <jni.h>

// Logging
// 日志记录
#include <android/log.h>

// C++ Standard Library (when compiling as C++)
// C++ 标准库 (当作为 C++ 编译时)
#ifdef __cplusplus
#include <string>
#include <map>
#include <list>
#include <vector>
#include <set>
#include <queue>
#include <deque>
#include <stack>
#include <array>
#include <unordered_map>
#include <unordered_set>
#include <cstring>
#include <cstdio>
#include <regex>
#include <cerrno>
#include <climits>
#include <iostream>
#include <fstream>
#include <string_view>
#include <thread>

using namespace std;
#else
#include <string.h>
#endif

#endif // ALLINCLUDE_H