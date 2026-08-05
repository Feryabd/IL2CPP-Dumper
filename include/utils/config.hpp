#pragma once

#include <cstddef>

// ============================================================
// Target Environment Configuration
//
// How to Extract and Verify IDA Style Signatures (AOB Patterns)
// ============================================================
// To locate necessary IL2CPP APIs in games where exports have been stripped,
// we can generate signature patterns using a dummy game. Here is the process:
//
// 1. Identify Target Unity Version:
//    - Find the exact Unity version the target game is using (e.g., by checking
//      the game's metadata files, such as 'globalgamemanagers', 'mainData',
//      or package properties).
//
// 2. Build a Dummy Game:
//    - Download the corresponding Unity version.
//    - Create an empty dummy project and build it using IL2CPP for the same
//      target architecture (e.g., ARM64, ARMv7, x86_64).
//    - Note: This assumes the target game uses the standard Unity IL2CPP toolchain
//      without a heavily customized or custom-forked version of IL2CPP.
//
// 3. Decompile the Dummy Binary:
//    - Locate the unstripped 'libil2cpp.so' in the dummy game build.
//    - Load it into a disassembler/decompiler such as IDA Pro (or Ghidra).
//    - You do not need to let the full auto-analysis finish; analyze just enough
//      until the exported IL2CPP APIs are populated.
//
// 4. Create the Signatures:
//    - In IDA Pro, go to the 'Exports' tab and search for the desired IL2CPP
//      API methods (e.g., 'il2cpp_domain_get', 'il2cpp_class_get_fields', etc.).
//    - Navigate to the disassembly of the found API.
//    - Use the "IDA SignMaker" plugin (or the "Sigga" signature plugin if using Ghidra,
//      or a similar signature maker tool) to generate a unique byte signature
//      (Array of Bytes / AOB) for the function.
//
// 5. Verify the Signatures on the Target Binary:
//    - Load the stripped 'libil2cpp.so' from the target game into a new IDA instance.
//    - Search for the generated AOB pattern to verify that it successfully finds the function.
//    - Check that the matching function's assembly code structure aligns with 
//      the assembly code observed in the dummy binary.
//
// 6. Update the Signatures:
//    - Once verified, add/replace the patterns in the respective architecture
//      arrays below (e.g., 'AArch64_Sigs', 'ARM_Sigs', 'x86_64_Sigs').
// ============================================================

namespace Config
{
    inline constexpr const char* GAME_ASSEMBLY_NAME = "libil2cpp.so";

    struct ApiSignature {
        const char* name;
        const char* patterns[5];

        constexpr ApiSignature(const char* n, const char* p1 = nullptr, const char* p2 = nullptr, const char* p3 = nullptr, const char* p4 = nullptr, const char* p5 = nullptr)
            : name(n), patterns{p1, p2, p3, p4, p5} {}
    };

    // ============================================================
    // IL2CPP API Signatures (Edit patterns here)
    // ============================================================

    static constexpr ApiSignature x86_64_Sigs[] = {
        {"il2cpp_domain_get",            ""},
        {"il2cpp_domain_get_assemblies", ""},
        {"il2cpp_image_get_class",       ""},
        {"il2cpp_class_get_interfaces",  ""},
        {"il2cpp_class_get_methods",     ""},
        {"il2cpp_class_get_fields",      ""},
        {"il2cpp_method_get_param_name", ""},
        {"il2cpp_class_from_type",       ""},
        {"il2cpp_type_get_name",         ""},
        {"il2cpp_thread_attach",         ""}
    };

    static constexpr ApiSignature AArch64_Sigs[] = {
        {"il2cpp_domain_get",            "?? ?? ?? 90 ?? ?? 40 F9 C0 03 5F D6"},
        {"il2cpp_domain_get_assemblies", "F4 4F BE A9 FD 7B 01 A9 FD 43 00 91 ?? ?? ?? 35 ?? ?? ?? 90 ?? ?? 40 F9"},
        {"il2cpp_image_get_class",       "F6 57 BD A9 F4 4F 01 A9 FD 7B 02 A9 FD 83 00 91 ?? ?? ?? 35 ?? ?? ?? 37"},
        {"il2cpp_class_get_interfaces",  "F4 4F BE A9 FD 7B 01 A9 FD 43 00 91 ?? ?? ?? B5 ?? ?? ?? F9 08 01 80 52"},
        {"il2cpp_class_get_methods",     "F5 57 BE A9 F3 7B 01 A9 FD 03 00 91 ?? ?? ?? B5 ?? ?? ?? F9 F3 03 01 AA"},
        {"il2cpp_class_get_fields",      "F5 57 BE A9 F3 7B 01 A9 FD 03 00 91 ?? ?? ?? B5 ?? ?? ?? F9 02 00 80 52"},
        {"il2cpp_method_get_param_name", "F4 4F BE A9 FD 7B 01 A9 FD 43 00 91 ?? ?? ?? B5 ?? ?? ?? B9 00 00 40 F9"},
        {"il2cpp_class_from_type",       "F4 4F BE A9 FD 7B 01 A9 FD 43 00 91 ?? ?? ?? B5 ?? ?? ?? B9 ?? ?? ?? 94"},
        {"il2cpp_type_get_name",         "F4 4F BE A9 FD 7B 01 A9 FD 43 00 91 ?? ?? ?? B5 ?? ?? ?? F9 01 00 80 52"},
        {"il2cpp_thread_attach",         "F6 57 BD A9 F4 4F 01 A9 FD 7B 02 A9 FD 83 00 91 ?? ?? ?? 94 ?? ?? ?? 94"}
    };

    static constexpr ApiSignature ARM_Sigs[] = {
        {"il2cpp_domain_get",            ""},
        {"il2cpp_domain_get_assemblies", ""},
        {"il2cpp_image_get_class",       ""},
        {"il2cpp_class_get_interfaces",  ""},
        {"il2cpp_class_get_methods",     ""},
        {"il2cpp_class_get_fields",      ""},
        {"il2cpp_method_get_param_name", ""},
        {"il2cpp_class_from_type",       ""},
        {"il2cpp_type_get_name",         ""},
        {"il2cpp_thread_attach",         ""}
    };

    struct ArchitectureRegistry {
        int machine;
        const ApiSignature* signatures;
        size_t count;
    };

    static constexpr ArchitectureRegistry Registries[] = {
        {62,  x86_64_Sigs,  sizeof(x86_64_Sigs) / sizeof(ApiSignature)},
        {183, AArch64_Sigs, sizeof(AArch64_Sigs) / sizeof(ApiSignature)},
        {40,  ARM_Sigs,     sizeof(ARM_Sigs) / sizeof(ApiSignature)}
    };
}
