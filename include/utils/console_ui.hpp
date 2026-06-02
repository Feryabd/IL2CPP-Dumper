#pragma once

#include <string>
#include <android/log.h>
#include <cinttypes>

#ifndef LOG_TAG
#define LOG_TAG "IL2CPP_DUMPER"
#endif

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

namespace UI
{
    inline void PrintHeader()
    {
        LOGI("################################################");
        LOGI("               IL2CPP DUMPER V1.0");
        LOGI("################################################");
    }

    inline void PrintStatusBlock(uintptr_t gameAssembly, void* domain, size_t assemblyCount)
    {
        LOGI("--- ENVIRONMENT ------------------------------");
        LOGI("[ BASE   ] 0x%" PRIxPTR, gameAssembly);
        LOGI("[ DOMAIN ] 0x%" PRIxPTR, (uintptr_t)domain);
        LOGI("[ MODULE ] %zu Assemblies Loaded", assemblyCount);
        LOGI("----------------------------------------------");
    }


    inline void Log(const std::string& type, const std::string& msg)
    {
        LOGI(" >> %5s | %s", type.c_str(), msg.c_str());
    }

    inline void UpdateProgress(float progress, const std::string& status)
    {
        static int lastLogPercent = -1;
        int currentPercent = static_cast<int>(progress * 100);
        if (currentPercent % 10 == 0 && currentPercent != lastLogPercent) {
            LOGI("Progress: %d%% | %s", currentPercent, status.c_str());
            lastLogPercent = currentPercent;
        }
    }

    inline void Success(const std::string& msg)
    {
        LOGI("[*] SUCCESS: %s", msg.c_str());
    }
}
