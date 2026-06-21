@echo off
setlocal enabledelayedexpansion

:: ============================================================================
:: IL2CPP Dumper Automated Build Script for Android (All ABIs)
:: ============================================================================

:: Automatically find the highest available CMake version
set "SDK_CMAKE_DIR="
for /d %%I in ("%LOCALAPPDATA%\Android\Sdk\cmake\*") do (
    if exist "%%I\bin\cmake.exe" (
        set "SDK_CMAKE_DIR=%%I\bin"
    )
)

:: Automatically find the highest available NDK version
set "NDK_TOOLCHAIN="
for /d %%I in ("%LOCALAPPDATA%\Android\Sdk\ndk\*") do (
    if exist "%%I\build\cmake\android.toolchain.cmake" (
        set "NDK_TOOLCHAIN=%%I\build\cmake\android.toolchain.cmake"
    )
)

echo ============================================================================
echo [BUILD] Checking dependencies...
echo ============================================================================

:: Check if Sdk CMake exists
if not exist "%SDK_CMAKE_DIR%\cmake.exe" (
    echo [ERROR] CMake executable was not found at:
    echo        "%SDK_CMAKE_DIR%\cmake.exe"
    echo        Please verify your Android SDK CMake installation folder!
    pause
    exit /b 1
)

:: Check if Sdk Ninja exists
if not exist "%SDK_CMAKE_DIR%\ninja.exe" (
    echo [ERROR] Ninja build generator was not found at:
    echo        "%SDK_CMAKE_DIR%\ninja.exe"
    echo        Please verify your Android SDK CMake installation folder!
    pause
    exit /b 1
)

:: Check if NDK toolchain script exists
if not exist "%NDK_TOOLCHAIN%" (
    echo [ERROR] Android NDK CMake toolchain script was not found at:
    echo        "%NDK_TOOLCHAIN%"
    echo        Please verify your Android NDK version path!
    pause
    exit /b 1
)

:: Append CMake bin directory temporarily to PATH
set "PATH=%SDK_CMAKE_DIR%;%PATH%"

echo [SUCCESS] Dependencies resolved successfully.
echo.

:: List of target ABIs to build
set "ABIS=arm64-v8a armeabi-v7a x86_64"

:: Create main output directory for libs inside build folder
if not exist "build\libs" (
    mkdir "build\libs"
)

for %%A in (%ABIS%) do (
    echo ============================================================================
    echo [BUILD] Building Target ABI: %%A
    echo ============================================================================
    
    :: Create build directory for specific ABI
    if not exist "build\%%A" (
        mkdir "build\%%A"
    )
    
    cd "build\%%A"
    
    :: Configure target build
    cmake -G "Ninja" ^
        -DCMAKE_TOOLCHAIN_FILE="%NDK_TOOLCHAIN%" ^
        -DCMAKE_BUILD_TYPE=Debug ^
        -DANDROID_ABI=%%A ^
        -DANDROID_PLATFORM=android-24 ^
        ..\..
        
    if !errorlevel! neq 0 (
        echo.
        echo [ERROR] CMake configuration failed for %%A!
        cd ..\..
        pause
        exit /b 1
    )
    
    :: Update compile_commands.json for IntelliSense
    if exist "compile_commands.json" (
        copy /y "compile_commands.json" "..\..\compile_commands.json" >nul
    )
    
    :: Execute compilation
    cmake --build .
    
    if !errorlevel! neq 0 (
        echo.
        echo [ERROR] Library compilation failed for %%A!
        cd ..\..
        pause
        exit /b 1
    )
    
    :: Copy output binary to output directory
    if exist "libil2cpp_dumper.so" (
        if not exist "..\libs\%%A" (
            mkdir "..\libs\%%A"
        )
        copy /y "libil2cpp_dumper.so" "..\libs\%%A\" >nul
        echo [SUCCESS] Generated binary for %%A: build\libs\%%A\libil2cpp_dumper.so
    ) else (
        echo [ERROR] Target binary was not found for %%A!
        cd ..\..
        pause
        exit /b 1
    )
    
    cd ..\..
    echo.
)

echo ============================================================================
echo [BUILD] All ABIs Built Successfully! Check the 'build\libs' folder.
echo ============================================================================
echo.
pause
