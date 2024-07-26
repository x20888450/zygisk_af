#ifndef YINGMEMORY_CPP
#define YINGMEMORY_CPP
#include <jni.h>
#include "yingMemory.hpp"
namespace ying {
bool is_valid_address_map(uintptr_t address) {
KITTY_LOGD("is_valid_address_map(%p)",(const void*)address);
KittyMemory::ProcMap addressMap = KittyMemory::getAddressMap((const void*)address);
bool flags=addressMap.isValid() && addressMap.readable;
if (!flags) {
KITTY_LOGE("无效的地址映射%p",(const void*)address);
}
return flags;
}

template <typename T>
T getValues(uintptr_t address)
{
if (address <= 0) return {};
if (is_valid_address_map(address))
return *(T*)(address);
else
return T(0);
}
template <typename T>
bool setValues(T value,uintptr_t address) {
if (address <= 0) return {};
const void *addr = (const void *)(address);
KittyMemory::ProcMap addressMap = KittyMemory::getAddressMap(addr);
if (!addressMap.isValid()) {
KITTY_LOGE("在maps文件中找不到这个%p地址对应的内存段", address);
return false;
}
if (addressMap.protection & PROT_WRITE) {
*(T*)(address)=value;
} else {
auto start = addressMap.startAddress;
auto end = addressMap.endAddress;
auto length = addressMap.length;
mprotect(reinterpret_cast<void *>(start), length, addressMap.protection | PROT_WRITE);
*(T*)(address)=value;
mprotect(reinterpret_cast<void *>(start), length, addressMap.protection);
}
return true;
}
uintptr_t getAbsoluteAddress(const char *fileName, uintptr_t address) {
std::vector<KittyMemory::ProcMap> map = KittyMemory::getMapsContain(fileName);
if (map.empty()) {
return 0;
}
return map[0].startAddress + address;
}
void determine_range(ProcMap &maps, bool last_is_cd) {
int A = 32;
int AS = 524288;
int BAD = 131072;
int XA = 16384;
int XS = 32768;
int CA = 4;
int CB = 16;
int CD = 8;
int CH = 1;
int J = 65536;
int JH = 2;
int OTHER = -2080896;
int PS = 262144;
int S = 64;
int V = 1048576;
int ALL = -1;

auto *module_name = maps.pathname.c_str();
if (maps.executable) {
if (module_name[0] == '\0' || (strstr(module_name, "/data/app") != nullptr) || (strstr(module_name, "/data/user") != nullptr)) {
maps.range = XA;
} else {
maps.range = XS;
}
return;
}
if (module_name[0] != '\0') {
if ((strncmp(module_name, "/dev/", 5) == 0)
&& ((strstr(module_name, "/dev/mali") != nullptr)
|| (strstr(module_name, "/dev/kgsl") != nullptr)
|| (strstr(module_name, "/dev/nv") != nullptr)
|| (strstr(module_name, "/dev/tegra") != nullptr)
|| (strstr(module_name, "/dev/ion") != nullptr)
|| (strstr(module_name, "/dev/pvr") != nullptr)
|| (strstr(module_name, "/dev/render") != nullptr)
|| (strstr(module_name, "/dev/galcore") != nullptr)
|| (strstr(module_name, "/dev/fimg2d") != nullptr)
|| (strstr(module_name, "/dev/quadd") != nullptr)
|| (strstr(module_name, "/dev/graphics") != nullptr)
|| (strstr(module_name, "/dev/mm_") != nullptr)
|| (strstr(module_name, "/dev/dri/") != nullptr))) {
maps.range = V;
return;
}
if ( ((strncmp(module_name, "/dev/", 5) == 0) && (strstr(module_name, "/dev/xLog") != nullptr))
|| (strncmp(module_name, "/system/fonts/", 0xe) == 0)
|| (strncmp(module_name, "anon_inode:dmabuf", 0x11) == 0)) {
maps.range = BAD;
return;
}
if (strstr(module_name, "[anon:.bss]") != nullptr) {
maps.range = last_is_cd ? CB : OTHER;
return;
}
if (strncmp(module_name, "/system/", 8) == 0) {
maps.range = OTHER;
return;
}
if (strstr(module_name, "/dev/zero") != nullptr) {
maps.range = CA;
return;
}
if (strstr(module_name, "PPSSPP_RAM") != nullptr) {
maps.range = PS;
return;
}
if ( (strstr(module_name, "system@") == nullptr)
&& (strstr(module_name, "gralloc") == nullptr)
&& strncmp(module_name, "[vdso]", 6) != 0
&& strncmp(module_name, "[vectors]", 9) != 0
&& (strncmp(module_name, "/dev/", 5) != 0 || (strncmp(module_name, "/dev/ashmem", 0xB) == 0)) ) {
if ( strstr(module_name, "dalvik") != nullptr ) {
if ( ((strstr(module_name, "eap") != nullptr)
|| (strstr(module_name, "dalvik-alloc") != nullptr)
|| (strstr(module_name, "dalvik-main") != nullptr)
|| (strstr(module_name, "dalvik-large") != nullptr)
|| (strstr(module_name, "dalvik-free") != nullptr))
&& (strstr(module_name, "itmap") == nullptr)
&& (strstr(module_name, "ygote") == nullptr)
&& (strstr(module_name, "ard") == nullptr)
&& (strstr(module_name, "jit") == nullptr)
&& (strstr(module_name, "inear") == nullptr) ) {
maps.range = JH;
return;
}
maps.range = J;
return;
}
if ((strstr(module_name, "/lib") != nullptr) && (strstr(module_name, ".so") != nullptr)) {
if (strstr(module_name, "/data/") != nullptr || (strstr(module_name, "/mnt/") != nullptr)) {
maps.range = CD;
return;
}
}
if (strstr(module_name, "malloc") != nullptr) {
maps.range = CA;
return;
}
if (strstr(module_name, "[heap]") != nullptr) {
maps.range = CH;
return;
}
if (strstr(module_name, "[stack") != nullptr) {
maps.range = S;
return;
}
if ((strncmp(module_name, "/dev/ashmem", 0xB) == 0) && (strstr(module_name, "MemoryHeapBase") == nullptr)) {
maps.range = AS;
return;
}
}
maps.range = OTHER;
return;
}
if (maps.is_rw && !maps.executable && maps.offset == 0) {
maps.range = A;
return;
}
maps.range = OTHER;
}

std::vector<ProcMap> getAllMaps()
{
std::vector<ProcMap> retMaps;
const char *file = "/proc/self/maps";
char line[512] = {0};

FILE *fp = fopen(file, "r");
if (!fp) {
KITTY_LOGE("Couldn't open file %s.", file);
return retMaps;
}

while (fgets(line, sizeof(line), fp)) {
ProcMap map;

char perms[5] = {0}, dev[11] = {0}, pathname[256] = {0};
// parse a line in maps file
// (format) startAddress-endAddress perms offset dev inode pathname
sscanf(line, "%llx-%llx %s %llx %s %lu %s",
&map.startAddress, &map.endAddress,
perms, &map.offset, dev, &map.inode, pathname);

map.length = map.endAddress - map.startAddress;
map.dev = dev;
map.pathname = pathname;

if (perms[0] == 'r') {
map.protection |= PROT_READ;
map.readable = true;
}
if (perms[1] == 'w') {
map.protection |= PROT_WRITE;
map.writeable = true;
}
if (perms[2] == 'x') {
map.protection |= PROT_EXEC;
map.executable = true;
}

map.is_private = (perms[3] == 'p');
map.is_shared = (perms[3] == 's');

map.is_rx = (strncmp(perms, "r-x", 3) == 0);
map.is_rw = (strncmp(perms, "rw-", 3) == 0);
map.is_ro = (strncmp(perms, "r--", 3) == 0);
determine_range(map,true);
retMaps.push_back(map);
}

fclose(fp);

if (retMaps.empty()) {
KITTY_LOGE("getAllMaps err couldn't find any map");
}
return retMaps;
}
std::vector<ProcMap> get_range(int ranges) {
std::vector<ProcMap> retMaps;
auto maps = getAllMaps();
for (auto& it : maps) {
if (it.range == ranges) {
retMaps.push_back(it);
}
}
return retMaps;
}
// 设置内存范围
void yingMemory::setRanges(int ranges) {
range = ranges;
}

// 获取结果计数
int yingMemory::getResultsCount() {
return result_address.size();
}

// 清除结果
void yingMemory::clearResults() {
result_address.clear();
}

// 过缺页
bool yingMemory::my_mincore(uintptr_t addr) {
int pageSize = getpagesize();
unsigned char vec = 0;
uintptr_t start = addr & (~(pageSize - 1));
mincore(reinterpret_cast<void*>(start), pageSize, &vec);
return (vec == 1);
}

// 搜索指定范围内的数字
template <typename T>
void yingMemory::searchNumber(T number, uintptr_t start_addr, uintptr_t end_addr, bool ignore_missing_page) {
uintptr_t temp_start = start_addr;
uintptr_t temp_end = end_addr;
uintptr_t temp_length = temp_end - temp_start;

for (int i = 0; i < temp_length / 4096; i++) {
uintptr_t start = temp_start + (i * 4096);
uintptr_t end = start + 4096;
if (ignore_missing_page) {
bool flags = my_mincore(start);
if (!flags) continue;
}
int bit = sizeof(T);
for (int j = start; j < end; j += bit) {
T value = *(T*)(j);
if (value != number) continue;
result_address.push_back(j);
}
}
}

// 根据名称搜索数字
template <typename T>
void yingMemory::searchNumber(T number, int limit, const std::string& name, bool ignore_missing_page) {
std::vector<ProcMap> maps;
if (name.empty()) {
maps = get_range(range);
}
// else {
// maps = KittyMemory::getMapsContain(name);
// }
if (maps.empty()) return;
for (const auto& it : maps) {
if (!it.readable) continue;
if (!it.isValid()) continue;
searchNumber<T>(number, it.startAddress, it.endAddress, ignore_missing_page);
if (limit <= 0) continue;
if (getResultsCount() >= limit) break;
}
}

// 根据偏移搜索数字
template <typename T>
void yingMemory::searchOffset(T number, int offset) {
std::vector<uintptr_t> temp;
for (auto& it : result_address) {
T value = *(T*)(it + offset);
if (value != number) continue;
temp.push_back(it);
}
result_address = std::move(temp);
}

// 编辑所有搜索结果
template <typename T>
void yingMemory::editAll(T value, uintptr_t offset) {
for (auto& it : result_address) {
setValues<T>(value, it + offset);
}
}
}//namespace ying

#endif // YGAMEGUARDIAN_CPP
