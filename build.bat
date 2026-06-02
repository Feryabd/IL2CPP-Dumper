@echo off
setlocal enabledelayedexpansion

:: ============================================================================
:: IL2CPP Dumper Automated Build Script for Android (All ABIs)
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
