#include "generator/generator.hpp"
#include "resolver/resolver.hpp"
#include "utils/console_ui.hpp"
#include "utils/config.hpp"
#include "KittyUtils.hpp"
#include <elf.h>
#include <cstring>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <csignal>

// Direct struct offset access macros and inline helpers
namespace Dumper
{
    namespace Generator
    {
        // -----------------------------------------------------------------------
        // Struct-direct accessors
        // -----------------------------------------------------------------------
        inline const char*       ClassName(const Il2CppClass* k) { return k ? k->name : "Unknown"; }
        inline const char*       ClassNS(const Il2CppClass* k) { return k ? k->namespaze : ""; }
        inline Il2CppClass*      ClassParent(const Il2CppClass* k) { return k ? k->parent : nullptr; }
        inline uint32_t          ClassFlags(const Il2CppClass* k) { return k ? k->flags : 0; }
        inline bool              ClassIsIface(const Il2CppClass* k) { return k && (k->flags & TYPE_ATTRIBUTE_INTERFACE); }
        inline bool              ClassIsEnum(const Il2CppClass* k) { return k && k->enumtype; }
        inline bool              ClassIsValue(const Il2CppClass* k) { return k && k->byval_arg.valuetype; }
        inline uint32_t          ClassToken(const Il2CppClass* k) { return k ? k->token : 0; }

        inline const char*       FieldName(const FieldInfo* f) { return f ? f->name : "?"; }
        inline const Il2CppType* FieldType(const FieldInfo* f) { return f ? f->type : nullptr; }
        inline int32_t           FieldOffset(const FieldInfo* f) { return f ? f->offset : 0; }
        inline uint32_t          FieldFlags(const FieldInfo* f) { return (f && f->type) ? f->type->attrs : 0; }

        inline const char*       MethName(const MethodInfo* m) { return m ? m->name : "?"; }
        inline const Il2CppType* MethRet(const MethodInfo* m) { return m ? m->return_type : nullptr; }

        // Helper: Format to HEX string
        template<typename T>
        std::string ToHex(T val) {
            std::stringstream ss;
            ss << "0x" << std::hex << std::uppercase << static_cast<uint64_t>(val);
            return ss.str();
        }

        // Parse C# Field Modifiers
        std::string GetFieldModifiers(uint32_t flags) {
            std::string str = "";
            uint32_t access = flags & FIELD_ATTRIBUTE_FIELD_ACCESS_MASK;
            switch (access) {
                case FIELD_ATTRIBUTE_PRIVATE: str += "private "; break;
                case FIELD_ATTRIBUTE_PUBLIC: str += "public "; break;
                case FIELD_ATTRIBUTE_FAMILY: str += "protected "; break;
                case FIELD_ATTRIBUTE_ASSEMBLY:
                case FIELD_ATTRIBUTE_FAM_AND_ASSEM: str += "internal "; break;
                case FIELD_ATTRIBUTE_FAM_OR_ASSEM: str += "protected internal "; break;
            }
            if (flags & FIELD_ATTRIBUTE_LITERAL) {
                str += "const ";
            } else {
                if (flags & FIELD_ATTRIBUTE_STATIC) str += "static ";
                if (flags & FIELD_ATTRIBUTE_INIT_ONLY) str += "readonly ";
            }
            return str;
        }

        // Parse C# Method Modifiers
        std::string GetModifiers(uint32_t flags) {
            std::string str = "";
            uint32_t access = flags & METHOD_ATTRIBUTE_MEMBER_ACCESS_MASK;
            switch (access) {
                case METHOD_ATTRIBUTE_PRIVATE: str += "private "; break;
                case METHOD_ATTRIBUTE_PUBLIC: str += "public "; break;
                case METHOD_ATTRIBUTE_FAMILY: str += "protected "; break;
                case METHOD_ATTRIBUTE_ASSEM:
                case METHOD_ATTRIBUTE_FAM_AND_ASSEM: str += "internal "; break;
                case METHOD_ATTRIBUTE_FAM_OR_ASSEM: str += "protected internal "; break;
            }
            if (flags & METHOD_ATTRIBUTE_STATIC) str += "static ";
            if (flags & METHOD_ATTRIBUTE_ABSTRACT) {
                str += "abstract ";
                if ((flags & METHOD_ATTRIBUTE_VTABLE_LAYOUT_MASK) == METHOD_ATTRIBUTE_REUSE_SLOT) str += "override ";
            } else if (flags & METHOD_ATTRIBUTE_FINAL) {
                if ((flags & METHOD_ATTRIBUTE_VTABLE_LAYOUT_MASK) == METHOD_ATTRIBUTE_REUSE_SLOT) str += "sealed override ";
            } else if (flags & METHOD_ATTRIBUTE_VIRTUAL) {
                if ((flags & METHOD_ATTRIBUTE_VTABLE_LAYOUT_MASK) == METHOD_ATTRIBUTE_NEW_SLOT) str += "virtual ";
                else str += "override ";
            }
            if (flags & METHOD_ATTRIBUTE_PINVOKE_IMPL) str += "extern ";
            return str;
        }

        // Parse C# Class Modifiers
        std::string GetTypeModifiers(uint32_t flags) {
            std::string str = "";
            uint32_t visibility = flags & TYPE_ATTRIBUTE_VISIBILITY_MASK;
            switch (visibility) {
                case TYPE_ATTRIBUTE_PUBLIC:
                case TYPE_ATTRIBUTE_NESTED_PUBLIC: str += "public "; break;
                case TYPE_ATTRIBUTE_NOT_PUBLIC:
                case TYPE_ATTRIBUTE_NESTED_FAM_AND_ASSEM:
                case TYPE_ATTRIBUTE_NESTED_ASSEMBLY: str += "internal "; break;
                case TYPE_ATTRIBUTE_NESTED_PRIVATE: str += "private "; break;
                case TYPE_ATTRIBUTE_NESTED_FAMILY: str += "protected "; break;
                case TYPE_ATTRIBUTE_NESTED_FAM_OR_ASSEM: str += "protected internal "; break;
            }
            if ((flags & TYPE_ATTRIBUTE_ABSTRACT) && (flags & TYPE_ATTRIBUTE_SEALED)) str += "static ";
            else if (!(flags & TYPE_ATTRIBUTE_INTERFACE) && (flags & TYPE_ATTRIBUTE_ABSTRACT)) str += "abstract ";
            else if ((flags & TYPE_ATTRIBUTE_SEALED)) str += "sealed ";
            return str;
        }

        // Dynamically compute TypeInfo offset of Il2Cpp Klasses
        uintptr_t FindClassPointerOffset(uintptr_t gameAssembly, uintptr_t klassAddress) {

            if (!gameAssembly || !klassAddress) return 0;

            Elf64_Ehdr* ehdr = (Elf64_Ehdr*)gameAssembly;
            if (memcmp(ehdr->e_ident, ELFMAG, SELFMAG) != 0) return 0;

            Elf64_Phdr* phdr = (Elf64_Phdr*)(gameAssembly + ehdr->e_phoff);

            for (int i = 0; i < ehdr->e_phnum; i++) {
              
                if (phdr[i].p_type == PT_LOAD && (phdr[i].p_flags & PF_R) && (phdr[i].p_flags & PF_W)) {
                   
                    uintptr_t sectionStart = gameAssembly + phdr[i].p_vaddr;
                    size_t sectionSize = phdr[i].p_memsz;

                    for (size_t j = 0; j < sectionSize; j += sizeof(uintptr_t)) {
                       
                        uintptr_t* ptr = (uintptr_t*)(sectionStart + j);
                      
                        if (*ptr == klassAddress) {

                            return (sectionStart + j) - gameAssembly;
                        }
                    }
                }
            }
            return 0;
        }

        // Custom C# Type resolution layer
        std::string ResolveTypeName(const Il2CppType* type) {
            if (!type) return "void";

            if (il2cpp_type_get_name) {
                char* namePtr = il2cpp_type_get_name(type);
                if (namePtr && namePtr[0] != '\0') {
                    std::string name(namePtr);
                    if (name == "System.Void") return "void";
                    if (name == "System.Boolean") return "bool";
                    if (name == "System.Byte") return "byte";
                    if (name == "System.SByte") return "sbyte";
                    if (name == "System.Int16") return "short";
                    if (name == "System.UInt16") return "ushort";
                    if (name == "System.Int32") return "int";
                    if (name == "System.UInt32") return "uint";
                    if (name == "System.Int64") return "long";
                    if (name == "System.UInt64") return "ulong";
                    if (name == "System.Single") return "float";
                    if (name == "System.Double") return "double";
                    if (name == "System.String") return "string";
                    if (name == "System.Object") return "object";
                    if (name == "System.Char") return "char";
                    return name;
                }
            }

            // Safe Switch statement fallback for fully stripped IL2CPP APIs
            switch (type->type) {
                case IL2CPP_TYPE_VOID:     return "void";
                case IL2CPP_TYPE_BOOLEAN:  return "bool";
                case IL2CPP_TYPE_CHAR:     return "char";
                case IL2CPP_TYPE_I1:       return "sbyte";
                case IL2CPP_TYPE_U1:       return "byte";
                case IL2CPP_TYPE_I2:       return "short";
                case IL2CPP_TYPE_U2:       return "ushort";
                case IL2CPP_TYPE_I4:       return "int";
                case IL2CPP_TYPE_U4:       return "uint";
                case IL2CPP_TYPE_I8:       return "long";
                case IL2CPP_TYPE_U8:       return "ulong";
                case IL2CPP_TYPE_R4:       return "float";
                case IL2CPP_TYPE_R8:       return "double";
                case IL2CPP_TYPE_STRING:   return "string";
                case IL2CPP_TYPE_OBJECT:   return "object";
                case IL2CPP_TYPE_I:        return "IntPtr";
                case IL2CPP_TYPE_U:        return "UIntPtr";
                case IL2CPP_TYPE_TYPEDBYREF: return "TypedReference";
                case IL2CPP_TYPE_SZARRAY:  return ResolveTypeName(type->data.type) + "[]";
                case IL2CPP_TYPE_PTR:      return ResolveTypeName(type->data.type) + "*";
                default: break;
            }

            const Il2CppClass* klass = il2cpp_class_from_type(type, 1);
            if (klass) {
                std::string ns = ClassNS(klass);
                std::string name = ClassName(klass);
                if (!name.empty() && name != "Unknown") {
                    return ns.empty() ? name : (ns + "." + name);
                }
            }

            return "object";
        }

        bool ExportCS(uintptr_t gameAssembly, Il2CppDomain* domain)
        {
            if (!gameAssembly || !domain) return false;

            std::string storagePath = KittyUtils::Android::getExternalStorage();
            if (storagePath.empty()) storagePath = "/sdcard";
            std::string outPath = storagePath + "/Download/dump.cs";

            std::ofstream out(outPath);
            if (!out.is_open()) {
                UI::Log("ERR", "Failed to open output file: " + outPath);
                return false;
            }

            size_t assemblyCount = 0;
            const Il2CppAssembly** assemblies = il2cpp_domain_get_assemblies(domain, &assemblyCount);
            if (!assemblies || assemblyCount == 0) {
                UI::Log("ERR", "Failed to query loaded assemblies from Domain.");
                return false;
            }

            UI::PrintStatusBlock(gameAssembly, domain, assemblyCount);

            out << "// IL2CPP DUMP - " << Config::GAME_ASSEMBLY_NAME << "\n";
            out << "// Base Address: " << ToHex(gameAssembly) << "\n\n";

            for (size_t i = 0; i < assemblyCount; ++i) {
                const Il2CppAssembly* assembly = assemblies[i];
                if (!assembly || !assembly->image) continue;

                const Il2CppImage* image = assembly->image;
                const char* imageName = image->name;
                uint32_t classCount = image->typeCount;

                UI::Log("INFO", "Exporting Assembly [" + std::to_string(i+1) + "/" + std::to_string(assemblyCount) + "] " + imageName);
                
                out << "// ASSEMBLY: " << imageName << " (" << classCount << " classes)\n";
                out << "namespace " << (image->nameNoExt ? image->nameNoExt : "Unknown") << "\n{\n";

                for (size_t j = 0; j < classCount; ++j) {
                    const Il2CppClass* klass = il2cpp_image_get_class(image, j);
                    if (!klass) continue;

                    std::string ns = ClassNS(klass);
                    std::string name = ClassName(klass);
                    uint32_t flags = ClassFlags(klass);

                    out << "\t// [Token: " << ToHex(ClassToken(klass)) << "]\n";
                    out << "\t// [TypeInfo RVA: " << ToHex(FindClassPointerOffset(gameAssembly, (uintptr_t)klass)) << "]\n";
                    
                    out << "\t" << GetTypeModifiers(flags);
                    if (ClassIsIface(klass)) out << "interface ";
                    else if (ClassIsEnum(klass)) out << "enum ";
                    else if (ClassIsValue(klass)) out << "struct ";
                    else out << "class ";
                    
                    out << name;
                    
                    Il2CppClass* parent = ClassParent(klass);
                    if (parent) {
                        out << " : " << ClassName(parent);
                    }
                    
                    out << "\n\t{\n";

                    // --- Extract Fields ---
                    void* f_iter = nullptr;
                    FieldInfo* field;
                    bool hasFields = false;
                    while ((field = il2cpp_class_get_fields((Il2CppClass*)klass, &f_iter))) {
                        const char* f_name = FieldName(field);
                        const Il2CppType* f_type = FieldType(field);
                        if (!f_name || f_name[0] < 32) continue;

                        out << "\t\t" << GetFieldModifiers(FieldFlags(field)) << ResolveTypeName(f_type) << " " << f_name << "; // " << ToHex((uint32_t)FieldOffset(field)) << "\n";
                        hasFields = true;
                    }
                    if (hasFields) out << "\n";

                    // --- Extract Methods ---
                    void* m_iter = nullptr;
                    const MethodInfo* method;
                    while ((method = il2cpp_class_get_methods((Il2CppClass*)klass, &m_iter))) {
                        const char* m_name = MethName(method);
                        if (!m_name || m_name[0] < 32) continue;

                        uintptr_t addr = (uintptr_t)method->methodPointer;
                        uintptr_t relative = addr ? (addr - gameAssembly) : 0;

                        out << "\t\t// RVA: " << ToHex(relative) << " VA: " << ToHex(addr) << "\n";
                        out << "\t\t" << GetModifiers(method->flags) << ResolveTypeName(method->return_type) << " " << m_name << "(";

                        for (uint8_t p = 0; p < method->parameters_count; ++p) {
                            const Il2CppType* p_type = method->parameters[p];
                            const char* p_name = il2cpp_method_get_param_name(method, p);
                            out << ResolveTypeName(p_type) << " " << (p_name ? p_name : "param" + std::to_string(p));
                            if (p < method->parameters_count - 1) out << ", ";
                        }

                        out << ");\n";
                    }

                    out << "\t}\n\n";
                }
                out << "}\n\n";
            }

            out.close();
            UI::Success("Exported C# assembly layout successfully to: " + outPath);
            return true;
        }
    }
}
