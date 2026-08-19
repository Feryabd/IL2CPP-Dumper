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
        // 1. Mengunci pola domain_get dan assemblies (Terbukti memicu hit tunggal di RAM)
        {"il2cpp_domain_get",            "? ? ? 17 E0 03 01 AA ? ? ? 17 F3 7B BF A9"},
        {"il2cpp_domain_get_assemblies", "F3 7B BF A9 F3 03 01 AA ? ? ? 97 08 24 40 A9 29 01 08 CB 29 FD 43 93"},
        
        // 2. Jembatan Thunk Kelas & Metode (Sudah Terbukti Membuka Alamat Asli 100% Unik di Radare2 Anda)
        {"il2cpp_image_get_class",       "5B 9B FF 17 0B 4E FF 17"}, // Pola gabungan yang baru saja sukses memicu 1 hit murni!
        {"il2cpp_class_get_interfaces",  "0B 4E FF 17"}, 
        {"il2cpp_class_get_methods",     "95 51 FF 17"}, 
        {"il2cpp_class_get_fields",      "45 6C FF 17"}, 
        
        // 3. Jembatan Parameter & Kelas Dari Tipe data
        {"il2cpp_method_get_param_name", "18 6C FF 17"}, 
        {"il2cpp_class_from_type",       "0B 4E FF 17"}, 
        
        // 4. Pola statis kaku (Terbukti memicu hit14_0 sukses di Radare2 Anda)
        {"il2cpp_type_get_name",         "FF 03 01 D1 F4 13 00 F9 F3 7B 03 A9 E8 23 00 91 E1 03 1F 2A F4 23 00 91"},
        {"il2cpp_thread_attach",         "F5 7B BE A9 F3 53 01 A9 ? ? ? 97 ? ? ? 97 ? ? ? 97"}
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
