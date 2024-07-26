#ifndef HOOK_HPP
#define HOOK_HPP
#include "dobby.h"
#define il2cpp_func(ReturnType, name, ...)                                                                         \
  typedef ReturnType (*orig_##name##_handle)(void* _this, ##__VA_ARGS__);                                         \
  ReturnType fake_##name(void* _this, ##__VA_ARGS__);                                                            \
  orig_##name##_handle orig_##name = NULL;                                                                       \
  void hook_##name(void* addr) {                                                                                   \
    if (orig_##name == NULL) {                                                                                     \
      orig_##name = (orig_##name##_handle)(addr);                                                                \
      DobbyHook((void*)addr, (dobby_dummy_func_t)fake_##name, (dobby_dummy_func_t*)&orig_##name);                 \
    }                                                                                                             \
  }                                                                                                               \
  ReturnType fake_##name(void* _this, ##__VA_ARGS__) 

#endif
