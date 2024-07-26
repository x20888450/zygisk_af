#ifndef YINGMEMORY_HPP
#define YINGMEMORY_HPP
#include <functional>
#include "KittyInclude.hpp"

namespace ying {
class ProcMap : public KittyMemory::ProcMap {
public:
    int range; // 内存范围
};

// 检查地址是否有效
bool is_valid_address_map(uintptr_t address);

// 从指定地址获取值
template <typename T>
T getValues(uintptr_t address);
// 向指定地址设置值
template <typename T>
bool setValues(T value, uintptr_t address);
// 获取指定文件名和偏移量的绝对地址
uintptr_t getAbsoluteAddress(const char *fileName, uintptr_t address = 0);

// 获取所有内存映射
std::vector<ProcMap> getAllMaps();

// 确定内存范围
void determine_range(ProcMap &maps, bool last_is_cd);

// 获取指定范围内的内存映射
std::vector<ProcMap> get_range(int ranges);

class yingMemory {
private:
    int range = -1; // 内存范围

public:
    std::vector<uintptr_t> result_address; // 搜索结果地址
    // 设置内存范围
    void setRanges(int ranges);

    // 获取结果计数
    int getResultsCount();

    // 清除结果
    void clearResults();

    // 过缺页
    bool my_mincore(uintptr_t addr);

    // 搜索指定范围内的数字
    template <typename T>
    void searchNumber(T number, uintptr_t start_addr, uintptr_t end_addr, bool ignore_missing_page = true);

    // 根据名称搜索数字
    template <typename T>
    void searchNumber(T number, int limit = 0, const std::string& name = "", bool ignore_missing_page = true);

    // 根据偏移搜索数字
    template <typename T>
    void searchOffset(T number, int offset);

    // 编辑所有搜索结果
    template <typename T>
    void editAll(T value, uintptr_t offset);
}; // class end
}


#endif
