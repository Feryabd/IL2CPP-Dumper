#pragma once
#include <cstdint>
#include "utils/il2cpp-api.h"

namespace Dumper
{
    namespace Generator
    {
        // Gathers metadata from target IL2CPP domain and generates standard C# formats
        bool ExportCS(uintptr_t gameAssembly, Il2CppDomain* domain);
    }
}
