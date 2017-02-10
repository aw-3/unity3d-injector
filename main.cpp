#include "main.h"

#include <pthread.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include <string>
#include <iostream>
#include <sstream>

#include "utility.h"
#include "mono.h"

//
void OpenConsole()
{
    int consolePtr = posix_openpt(O_RDWR);

    if (consolePtr == -1)
    {
        return;
    }
    char* ptname = ptsname(consolePtr);
    if (!ptname)
    {
        close(consolePtr);
        return;
    }

    if (unlockpt(consolePtr) == -1)
    {
        close(consolePtr);
        return;
    }

    std::ostringstream oss;
    oss << "xterm -S" << (strrchr(ptname, '/')+1) << "/" << consolePtr << " &";
    system(oss.str().c_str());

    int xterm_fd = open(ptname,O_RDWR);
    char c;
    do read(xterm_fd, &c, 1); while (c!='\n');

    if (dup2(consolePtr, 1) <0)
    {
        std::cerr << "Could not redirect standard output.\n";
        close(consolePtr);
        return;
    }
    if (dup2(consolePtr, 2) <0)
    {
        std::cerr << "Could not redirect standard error output.\n";
        close(consolePtr);
        return;
    }

    return;
}

//

__attribute__((constructor))
void hello()
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