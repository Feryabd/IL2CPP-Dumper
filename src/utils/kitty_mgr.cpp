#include "utils/kitty_mgr.hpp"
#include <unistd.h>

KittyMemoryMgr g_KittyMgr;

bool InitKittyMgr()
{
    // Initialize with current process ID and SYSCALL mode
    // We set initMemPatch to true by default to allow patching if needed later
    return g_KittyMgr.initialize(getpid(), EK_MEM_OP_SYSCALL, true);
}
