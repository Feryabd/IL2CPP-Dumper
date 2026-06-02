@echo off
setlocal enabledelayedexpansion

:: ============================================================================
:: IL2CPP Dumper Automated Build Script for Android ARM64
:: ============================================================================

set "SDK_CMAKE_DIR=%LOCALAPPDATA%\Android\Sdk\cmake\3.22.1\bin"
set "NDK_TOOLCHAIN=%LOCALAPPDATA%\Android\Sdk\ndk\28.2.13676358\build\cmake\android.toolchain.cmake"

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
echo ============================================================================
echo [BUILD] Configuring CMake cache (Target: Android ARM64)...
echo ============================================================================

:: Create build folder if it does not exist
if not exist "build" (
    mkdir build
)

cd build

:: Clean up old CMake configurations for absolute safety
if exist "CMakeCache.txt" (
    echo [BUILD] Cleared old CMakeCache.txt to prevent conflict.
    del /f /q CMakeCache.txt
)

:: Configure target build
cmake -G "Ninja" ^
    -DCMAKE_TOOLCHAIN_FILE="%NDK_TOOLCHAIN%" ^
    -DCMAKE_BUILD_TYPE=Debug ^
    -DANDROID_ABI=arm64-v8a ^
    -DANDROID_PLATFORM=android-24 ^
    ..

if %errorlevel% neq 0 (
    echo.
    echo [ERROR] CMake configuration failed!
    cd ..
    pause
    exit /b 1
)

echo.
echo ============================================================================
echo [BUILD] Compiling C++ Shared Library Target...
echo ============================================================================

:: Execute compile compilation
cmake --build .

if %errorlevel% neq 0 (
    echo.
    echo [ERROR] Library compilation failed!
    cd ..
    pause
    exit /b 1
)

echo.
echo ============================================================================
echo [BUILD] Build Completed Successfully!
echo ============================================================================

if exist "libil2cpp_dumper.so" (
    echo [SUCCESS] Android Shared Library Binary generated successfully:
    echo           "!CD!\libil2cpp_dumper.so"
) else (
    echo [WARNING] Build returned success, but output binary was not found in directory.
)

cd ..
echo.
pause
