#ifndef YING_PATCH_HPP
#define YING_PATCH_HPP
#include <unordered_map>
#include "obfuscate.h"
#include "KittyInclude.hpp"

// 参考
// https://armconverter.com
// https://platinmods.com/threads/crazy-list-of-hex-codes.204057/
// https://platinmods.com/threads/list-of-hex-codes-years-of-research.209699/
// https://gameguardian.net/forum/topic/37104-arm-patching-advanced/
// https://www.binaryconvert.com/convert_float.html
#if defined(__ANDROID__)
#if defined(__arm__)
//bx lr
#define hex_delete "1E FF 2F E1"
//mov r0,#1
// bx lr
#define hex_true "01 00 A0 E3 1E FF 2F E1"
//mov r0,#0
// bx lr
#define hex_false "00 00 A0 E3 1E FF 2F E1"
// il2cpp+0x1d18740
// 0x3f800000=1.0f
// asm code
// movw r0, #0
// movt r0, #0x3f80
// str r1, [r0, #0x12c]
// 000000E3r;800F43E3r;2C1180E5r;
#define hex_runtimeActiveRange "000000E3 800F43E3 2C1180E5"
#endif // __arm__
#endif // __ANDROID__
ProcMap g_il2cppBaseMap;
std::unordered_map<uintptr_t, MemoryPatch> memoryPatchesMap;

// Patching a offset with switch.
void patchOffset(uintptr_t offset, std::string hexBytes, bool isOn) {
    MemoryPatch patch = MemoryPatch::createWithHex(g_il2cppBaseMap, offset, hexBytes);
    // 检查偏移量是否已存在于 unordered_map 中
    auto it = memoryPatchesMap.find(offset);
    if (it != memoryPatchesMap.end()) {
        LOGE(OBFUSCATE("Already exists"));
        patch = it->second; // 获取已存在的 MemoryPatch 对象
    } else {
        memoryPatchesMap[offset] = patch;
        LOGI(OBFUSCATE("Added"));
    }
    
    if (!patch.isValid()) {
        return;
    }

    if (isOn && patch.get_CurrBytes() == patch.get_OrigBytes()) {
        patch.Modify();
    } else if (!isOn && patch.get_CurrBytes() != patch.get_OrigBytes()) {
        patch.Restore();
    }
}
uintptr_t string2Offset(const char *c) {
    int base = 16;
    // See if this function catches all possibilities.
    // If it doesn't, the function would have to be amended
    // whenever you add a combination of architecture and
    // compiler that is not yet addressed.
    static_assert(sizeof(uintptr_t) == sizeof(unsigned long)
                  || sizeof(uintptr_t) == sizeof(unsigned long long),
                  "Please add string to handle conversion for this architecture.");

    // Now choose the correct function ...
    if (sizeof(uintptr_t) == sizeof(unsigned long)) {
    // LOGI("%s strtoul %lx",__func__,strtoul(c, nullptr, base));
        return strtoul(c, nullptr, base);
    }

    // All other options exhausted, sizeof(uintptr_t) == sizeof(unsigned long long))
    // LOGI("%s strtoull %lx",__func__,strtoull(c, nullptr, base));
    return strtoull(c, nullptr, base);
}

// #define HOOK(offset, ptr, orig) hook((void *)(g_il2cppBaseMap.startAddress + string2Offset(OBFUSCATE(offset))), (void *)ptr, (void **)&orig)
#define PATCH(offset, hex) patchOffset(string2Offset(OBFUSCATE(offset)), OBFUSCATE(hex), true)
#define PATCH_SWITCH(offset, hex, boolean) patchOffset(string2Offset(OBFUSCATE(offset)), OBFUSCATE(hex), boolean)
#define RESTORE(offset) patchOffset(string2Offset(OBFUSCATE(offset)), "", false)

#ifndef kNO_KEYSTONE
// Patching a offset with switch.
void patchAsm(uintptr_t offset, std::string asm_code, bool isOn) {
#ifdef __arm__
    MemoryPatch patch = MemoryPatch::createWithAsm(g_il2cppBaseMap, offset,MP_ASM_ARM32, asm_code);
    // 检查偏移量是否已存在于 unordered_map 中
    auto it = memoryPatchesMap.find(offset);
    if (it != memoryPatchesMap.end()) {
        LOGE(OBFUSCATE("Already exists"));
        patch = it->second; // 获取已存在的 MemoryPatch 对象
    } else {
        memoryPatchesMap[offset] = patch;
        LOGI(OBFUSCATE("Added"));
    }
    
    if (!patch.isValid()) {
        return;
    }

    if (isOn && patch.get_CurrBytes() == patch.get_OrigBytes()) {
        patch.Modify();
    } else if (!isOn && patch.get_CurrBytes() != patch.get_OrigBytes()) {
        patch.Restore();
    }
    #endif
}
#endif
#endif // 
