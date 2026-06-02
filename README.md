# IL2CPP Dumper (Android Shared Library)

A simple, lightweight IL2CPP dumper designed to run inside the target Android process. It is packaged as a shared library (`.so`) and is optimized to dynamically locate and resolve stripped IL2CPP APIs using byte patterns (AOB signatures).

---

## Features

- **Multi-ABI Support**: Builds for `arm64-v8a`, `armeabi-v7a`, and `x86_64` out of the box.
- **Signature-Based API Resolution**: Locates IL2CPP export functions even if symbols are stripped from `libil2cpp.so`.
- **In-Memory Extraction**: Interacts directly with the active runtime structures for highly accurate dump results.
- **Portable Build Configuration**: Dynamic path resolving in the automated Windows build script.

---

## Build Instructions

To build the dumper for all target architectures, run the automated build script on Windows:

```cmd
.\build.bat
```

### Build Outputs
Upon a successful build, the compiled binaries will be copied into:
```
build/libs/
├── arm64-v8a/
│   └── libil2cpp_dumper.so
├── armeabi-v7a/
│   └── libil2cpp_dumper.so
└── x86_64/
    └── libil2cpp_dumper.so
```

---

## Configuration

Before building the dumper, you must configure the signature patterns for your target game. You can customize these by editing the C++ header file:
- **[config.hpp](file:///c:/Users/Feng-CN/Desktop/Android/RE/Analysis/Critical-OPS/Dumper/include/utils/config.hpp)**

Inside this file, you can define target signature patterns for each architecture (`AArch64_Sigs`, `ARM_Sigs`, `x86_64_Sigs`). Refer to the detailed header comment inside the config file for a step-by-step walkthrough on using IDA Pro or Ghidra to extract and verify these signature arrays.

---

## Usage & Injection

Because this dumper operates in-memory to directly query the active game structures, it **must be injected into the target game process** on the Android device.

### Recommended Injector

It is highly recommended to use **[AndKittyInjector](https://github.com/MJx0/AndKittyInjector)**.

#### Why AndKittyInjector?
- **Library Hiding**: Capable of hiding library segments from `/proc/self/maps` and native linker lists (`dladdr`, `dl_iterate_phdr`) to bypass simple anti-cheat detection.
- **Advanced Injection Methods**: Support for standard `dlopen` and in-memory `memfd` loading.
- **Compatibility**: Built specifically for Android game hacking/reverse engineering, with tested support from Android 5.0 up to 16.

### Quick Start Guide:
1. Compile the target `.so` library for the game's architecture (typically `arm64-v8a`).
2. Transfer the `.so` to a directory readable by the target game on the Android device (e.g., `/data/local/tmp/`).
3. Launch the target game.
4. Run `AndKittyInjector` to inject `libil2cpp_dumper.so` into the game process.
5. The dumper will initialize, run its signature scanner, resolve APIs, and dump the assembly layouts.
