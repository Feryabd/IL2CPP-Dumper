#pragma once
#include <cstdint>
#include "utils/il2cpp-api.h"

namespace IL2CPP
{
    bool ResolveFunctions(uintptr_t gameAssembly);
}

#define DEFINE_API_LIST \
    DO_API(Il2CppDomain*, il2cpp_domain_get, ()); \
    DO_API(const Il2CppAssembly**, il2cpp_domain_get_assemblies, (const Il2CppDomain* domain, size_t* size)); \
    DO_API(const Il2CppClass*, il2cpp_image_get_class, (const Il2CppImage* image, size_t index)); \
    DO_API(Il2CppClass*, il2cpp_class_get_interfaces, (Il2CppClass* klass, void** iter)); \
    DO_API(const MethodInfo*, il2cpp_class_get_methods, (Il2CppClass* klass, void** iter)); \
    DO_API(FieldInfo*, il2cpp_class_get_fields, (Il2CppClass* klass, void** iter)); \
    DO_API(const char*, il2cpp_method_get_param_name, (const MethodInfo* method, uint32_t index)); \
    DO_API(Il2CppClass*, il2cpp_class_from_type, (const Il2CppType* type, int flag)); \
    DO_API(char*, il2cpp_type_get_name, (const Il2CppType* type)); \
    DO_API(Il2CppThread*, il2cpp_thread_attach, (Il2CppDomain* domain))

#define DO_API(return_type, name, params) extern return_type (*name) params;
DEFINE_API_LIST
#undef DO_API
