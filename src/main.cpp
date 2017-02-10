#include "main.h"

#include "utility.h"
#include "mono.h"

#include <pthread.h>

__attribute__((constructor))
void HelloWorld()
{
    void *libMono = utility::GetModuleHandle("libmono.so");

    if(libMono)
    {
        mono::Initialize(libMono);
    }
    else
    {
        // Throw some sort of exception without triggering any anti-cheat
    }
}

void* LoadMonoDllThread(void *args)
{
    mono::HookAndLoadAssembly();
}

extern "C" {
void LoadMonoDll(char *dllPath, char *ns) {
    mono::assemblyToLoad = dllPath;
    mono::assemblyNamespace = ns;

    pthread_t pThread;
    pthread_create(&pThread, 0, LoadMonoDllThread, 0);
}
};