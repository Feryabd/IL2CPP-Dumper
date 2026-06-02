#pragma once

namespace Dumper
{
    // Initialize the engine (set up exception handlers, JNI caches, and KittyMemory)
    bool Initialize();

    // The main engine dispatch routine that locates the assembly, resolves functions, and dumps
    void Run();
}
