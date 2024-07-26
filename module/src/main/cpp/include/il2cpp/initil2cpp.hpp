#ifndef YING_INIT_IL2CPP_HPP
#define YING_INIT_IL2CPP_HPP
#include "UnityResolve.hpp"

void Initialization() {
UnityResolve::Init(dlopen("libil2cpp.so", RTLD_NOW), UnityResolve::Mode::Il2Cpp);
// GunModeRuntimeData
	// RVA: 0x9418c0 VA: 0xb2fcf8c0
	// public ActorBodySimplePartType get_aimPart() { }
const auto assembly = UnityResolve::Get("Assembly-CSharp.dll");
const auto pClass   = assembly->Get("GunModeRuntimeData");

// assembly->Get("className | 类名称", "*");
// assembly->Get("className | 类名称", "namespace | 空间命名");

// const auto field       = pClass->Get<UnityResolve::Field>("Field Name | 变量名");
// const auto fieldOffset = pClass->Get<std::int32_t>("Field Name | 变量名");
// const int  time        = pClass->GetValue<int>(obj Instance | 对象地址, "time");
// pClass->GetValue(obj Instance*, name);
// pClass->SetValue(obj Instance*, name, value);
const auto method      = pClass->Get<UnityResolve::Method>("get_aimPart");
// pClass->Get<UnityResolve::Method>("Method Name | 函数名", { "System.String" });
// pClass->Get<UnityResolve::Method>("Method Name | 函数名", { "*", "System.String" });
// pClass->Get<UnityResolve::Method>("Method Name | 函数名", { "*", "", "System.String" });
// pClass->Get<UnityResolve::Method>("Method Name | 函数名", { "*", "System.Int32", "System.String" });
// pClass->Get<UnityResolve::Method>("Method Name | 函数名", { "*", "System.Int32", "System.String", "*" });
// "*" == ""

// const auto functionPtr = method->function;

// const auto method1 = pClass->Get<UnityResolve::Method>("method name1 | 函数名称1");
// const auto method2 = pClass->Get<UnityResolve::Method>("method name2 | 函数名称2");

// method1->Invoke<int>(114, 514, "114514");
// Invoke<return type>(args...);

// Cast<return type, args...>(void);
// Cast(IM::MethodPointer<return type, args...>&);
// const IM::MethodPointer<void, int, bool> ptr = method2->Cast<void, int, bool>();
// ptr(114514, true);
// IM::MethodPointer<void, int, bool> add;
// ptr = method1->Cast(add);
LOGI("get_aimPart的函数地址是%lx", method->function);
}
#endif