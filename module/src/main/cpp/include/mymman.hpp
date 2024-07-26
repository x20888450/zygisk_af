#ifndef _MY_SYS_MMAN_HPP_
#define _MY_SYS_MMAN_HPP_

#include <sys/mman.h>   // 包含 mincore 函数的定义
#include <unistd.h>     // 包含 getpagesize 函数的定义
#include <cstdint>      // 包含 uintptr_t 的定义

namespace ying {

// 判断指定地址是否在物理内存中
bool my_mincore(uintptr_t addr) {
    int pageSize = getpagesize();  // 获取页面大小
    unsigned char vec = 0;
    uintptr_t start = addr & (~(pageSize - 1));  // 获取页面起始地址

    // 调用 mincore 函数检查内存页面
    if (mincore(reinterpret_cast<void*>(start), pageSize, &vec) == -1) {
        return false;  // mincore 调用失败时返回 false
    }

    return (vec & 1);  // 返回页面是否在物理内存中的结果
}

} // namespace ying

#endif // _MY_SYS_MMAN_HPP_
