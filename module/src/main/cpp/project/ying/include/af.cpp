#include "AllInclude.h"
#include "obfuscate.h"
#include "yinglog.hpp"
#include "KittyInclude.hpp"
#include "yingMemory.cpp"
#include "hook.hpp"
#include "pmparser.c"
#include "mymman.hpp"
#include "yckz.cpp"
#include "initil2cpp.hpp"

// libil2cpp.so
uintptr_t il2cppBaseAddress{};
// libunity.so
uintptr_t unityBaseAddress{};
// 游戏状态 
int game_state{};
// 搜索的主特征码
int searchNumber{};
// 特征值
std::vector<float> edit1 = {0.12f, 0.14f, 0.18f, 0.20f, 0.28f, 0.32f};
// 保存符合结果的Ca内存段
std::map<uintptr_t,uintptr_t> caMap;

il2cpp_func(void, SetSerState, int serState, int serSubState) {
if (_this != nullptr) {
game_state = serState;
return orig_SetSerState(_this, serState, serSubState);
}
}

constexpr float EPSILON = 0.01f;

bool isEqual(float a, float b, float c = EPSILON) {
return std::fabs(a - b) < c;
}

bool isEqual(float a, const std::vector<float>& b) {
return std::any_of(b.begin(), b.end(), [&](float it) {
return isEqual(a, it);
});
}

void searchCa(uintptr_t start, uintptr_t end) {
ying::yingMemory gg;
gg.clearResults();
gg.searchNumber<int>(searchNumber, start, end);
int count = gg.getResultsCount();
if (count <= 0) return;
LOGI("%lx-%lx",start,end);
LOGI("主特征码搜索到了%d个结果", count);

gg.searchOffset<int>(65543, 0x4);
gg.searchOffset<int>(8, 0x40);
count = gg.getResultsCount();
if (count <= 0) return;
LOGI("副特征码搜索到了%d个结果", count);

for (int i = 0; i < count; i++) {
uintptr_t addr = gg.result_address[i] + 0x78;
float v = *(float*)(addr);
if (v == 0 || v == 1.0f || isEqual(v, 0.48f)) continue;
if (!isEqual(v, edit1)) continue;
*(float*)(addr) = 1.0f;
caMap[start] = end;
}
}

void fw(bool flags) {
auto start_time = std::chrono::high_resolution_clock::now();

if (flags) {
procmaps_iterator *maps = pmparser_parse(-1);
if (maps == nullptr) return;

procmaps_struct *maps_tmp;
while ((maps_tmp = pmparser_next(maps)) != nullptr) {
if (!strstr(maps_tmp->pathname, "malloc")) continue;
searchCa((uintptr_t)maps_tmp->addr_start, (uintptr_t)maps_tmp->addr_end);
}
pmparser_free(maps);
} else {
if (caMap.empty()) return;
for (const auto& pair : caMap) {
searchCa( pair.first, pair.second);
}
}

auto end_time = std::chrono::high_resolution_clock::now();
auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
LOGI("函数执行时间 %lldms", duration);
}

void hack_thread() {
// Initialization();
// bool flags=yckz();
// if (flags) {
// LOGI(OBFUSCATE("远程控制状态:开启"));
// } else {
// LOGI(OBFUSCATE("远程控制状态:关闭"));
// return;
// }
while (true) {
if (unityBaseAddress && il2cppBaseAddress) break;
sleep(1);
unityBaseAddress = get_memory_start_address(-1, "libunity.so");
il2cppBaseAddress = get_memory_start_address(-1, "libil2cpp.so");
searchNumber = unityBaseAddress + 0xE001A0;
}
LOGI("unityBaseAddress %lx", unityBaseAddress);
LOGI("il2cppBaseAddress %lx", il2cppBaseAddress);

hook_SetSerState(reinterpret_cast<void*>(il2cppBaseAddress + 0x1bbf454));
while (true) {
if (game_state != 2) {
LOGI("不在游戏中 休眠20秒...");
sleep(20);
} else {
fw(false);
fw(true);
}
}
}