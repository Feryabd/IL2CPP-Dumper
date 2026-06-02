cmake_minimum_required(VERSION 3.22.1)

project("il2cpp_dumper")

# Enable compilation database generation for IDE IntelliSense (clangd, etc.)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# 1. Define the path to KittyMemory (relative to this CMakeLists.txt)
set(KITTYMEMORY_PATH ${CMAKE_CURRENT_SOURCE_DIR}/KittyMemory)

# 2. Correctly find all source files in KittyMemory and its subdirectories
file(GLOB_RECURSE KITTYMEMORY_SRC
    "${KITTYMEMORY_PATH}/*.cpp"
    "${KITTYMEMORY_PATH}/*.c"
)

# 3. Add include directories so the compiler can find your .h and .hpp files
include_directories(
    ${KITTYMEMORY_PATH}
    include
    .
)

# 4. Set the Keystone library path (ensure this path actually exists in your folder structure)
set(KEYSTONE_LIB ${KITTYMEMORY_PATH}/Deps/Keystone/libs-android/${ANDROID_ABI}/libkeystone.a)

# 6. Create the shared library using BOTH your native-lib and the gathered KittyMemory sources
add_library(${CMAKE_PROJECT_NAME} SHARED
    src/app/android_main.cpp
    src/core/engine.cpp
    src/resolver/api_resolver.cpp
    src/generator/cs_generator.cpp
    src/utils/kitty_mgr.cpp
    ${KITTYMEMORY_SRC}
)

# 7. Link dependencies
target_link_libraries(${CMAKE_PROJECT_NAME}
    android
    log
    ${KEYSTONE_LIB}
)

target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE 
    kUSE_LOGCAT 
    kITTYMEMORY_DEBUG
)
