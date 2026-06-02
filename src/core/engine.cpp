#include "core/core.hpp"
#include "resolver/resolver.hpp"
#include "generator/generator.hpp"
#include "utils/console_ui.hpp"
#include "utils/kitty_mgr.hpp"
#include "utils/config.hpp"
#include <unistd.h>
#include <sys/stat.h>
#include <csignal>
#include <cstring>
#include <exception>

namespace Dumper
{
    static void GlobalSignalHandler(int signum)
    {
        LOGE("CRITICAL: Caught fatal signal %d (%s) inside Dumper context!", signum, strsignal(signum));
        // Safe exit to prevent target game process lockup or silent corruption
        _exit(signum);
    }

    bool Initialize()
    {
        UI::Log("CORE", "Initializing dumper subsystems...");

        // Setup signal handlers for debugging and robustness
        signal(SIGSEGV, GlobalSignalHandler);
        signal(SIGABRT, GlobalSignalHandler);
        signal(SIGILL,  GlobalSignalHandler);

        // Initialize Memory Manager mapping
        if (!InitKittyMgr()) {
            UI::Log("ERR", "Failed to initialize KittyMemory Manager!");
            return false;
        }

        UI::Log("CORE", "Subsystems initialized successfully.");
        return true;
    }

    // Helper: Dynamic Module Scanning
    uintptr_t GetModuleBase(const char* moduleName)
    {
        ElfScanner libElf = g_KittyMgr.elfScanner.findElf(moduleName, EScanElfType::Any, EScanElfFilter::App);
        return libElf.base();
    }


    void Run()
    {
        UI::PrintHeader();

        uintptr_t gameAssembly = GetModuleBase(Config::GAME_ASSEMBLY_NAME);
        if (!gameAssembly)
        {
            UI::Log("ERR", "Target game assembly dynamic library module not found (" + std::string(Config::GAME_ASSEMBLY_NAME) + ")");
            return;
        }


        UI::Log("INFO", "Resolving IL2CPP functions dynamically...");
        if (!IL2CPP::ResolveFunctions(gameAssembly)) {
            UI::Log("ERR", "Failed to resolve critical IL2CPP APIs! Dumping aborted.");
            return;
        }

        // Attach current thread to domain safely
        Il2CppDomain* domain = il2cpp_domain_get();
        if (!domain) {
            UI::Log("ERR", "IL2CPP domain is null! Game is not fully initialized.");
            return;
        }
        il2cpp_thread_attach(domain);

        UI::Log("INFO", "Building AST & Exporting...");
        try {
            Dumper::Generator::ExportCS(gameAssembly, domain);
            UI::Success("Dumper has completed operations successfully.");
        } catch (const std::exception& e) {
            LOGE("IL2CPP Generator crashed: %s", e.what());
        } catch (...) {
            LOGE("IL2CPP Generator crashed with an unhandled generic exception!");
        }
    }
}
