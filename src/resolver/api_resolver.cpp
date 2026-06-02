#include "resolver/resolver.hpp"
#include "utils/kitty_mgr.hpp"
#include "utils/console_ui.hpp"
#include "utils/config.hpp"
#include <unistd.h>

// Allocation of global function pointers defined in dumper/resolver.hpp
#define DO_API(r, n, p) r (*n) p = nullptr;
DEFINE_API_LIST
#undef DO_API

namespace IL2CPP
{
    bool ResolveFunctions(uintptr_t hGameAssembly)
    {
        if (!hGameAssembly) return false;

        UI::Log("RESOLVER", "Starting API Resolution exclusively via Signature Scanning...");

        ElfScanner il2cppElf = g_KittyMgr.elfScanner.createWithBase(hGameAssembly);
        if (!il2cppElf.isValid()) {
            UI::Log("ERR", "Could not validate IL2CPP ELF structure for scanning!");
            return false;
        }

        int machine = il2cppElf.header().e_machine;
        const Config::ArchitectureRegistry* activeRegistry = nullptr;

        for (const auto& reg : Config::Registries) {
            if (reg.machine == machine) {
                activeRegistry = &reg;
                break;
            }
        }

        if (!activeRegistry) {
            UI::Log("ERR", "No signature registry found for architecture machine: " + std::to_string(machine));
            return false;
        }

        auto segments = il2cppElf.segments();
        bool allFound = true;

        #define DO_API(r, n, p) \
            { \
                const char* pattern = nullptr; \
                for (size_t i = 0; i < activeRegistry->count; ++i) { \
                    if (std::string_view(activeRegistry->signatures[i].name) == #n) { \
                        pattern = activeRegistry->signatures[i].pattern; \
                        break; \
                    } \
                } \
                if (pattern && pattern[0] != '\0') { \
                    for (const auto& seg : segments) { \
                        if (!seg.readable) continue; \
                        n = (r (*) p)g_KittyMgr.memScanner.findIdaPatternFirst(seg.startAddress, seg.endAddress, pattern); \
                        if (n) break; \
                    } \
                } \
                if (!n) { \
                    UI::Log("ERR", "Failed to resolve API: " #n); \
                    allFound = false; \
                } else { \
                    LOGD("RESOLVER: [%s] -> 0x%" PRIxPTR, #n, (uintptr_t)n); \
                } \
            }

        DEFINE_API_LIST
        #undef DO_API

        return allFound;
    }
}
