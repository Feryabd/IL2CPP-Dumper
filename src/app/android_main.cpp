#include <pthread.h>
#include <unistd.h>
#include <jni.h>
#include "core/core.hpp"
#include "utils/console_ui.hpp"

void* MainThread(void*)
{
    LOGI("[APP] Spawning dumper thread. Suspending thread execution for 5 seconds to ensure game assembly loads...");
    sleep(5); 

    if (!Dumper::Initialize()) {
        LOGE("[APP] Failed to initialize core dumper systems!");
        return nullptr;
    }

    LOGI("[APP] Invoking Dumper logic...");
    Dumper::Run();

    return nullptr;
}

extern "C" jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    LOGI("[APP] JNI_OnLoad called! Standard initialization triggered.");
    pthread_t thread;
    pthread_create(&thread, nullptr, MainThread, nullptr);
    pthread_detach(thread);
    return JNI_VERSION_1_6;
}
