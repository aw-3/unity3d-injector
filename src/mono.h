//
// Created by nicholas on 07/02/17.
//

#ifndef UNITY3D_INJECT_MONO_H
#define UNITY3D_INJECT_MONO_H

class mono {
public:
    typedef void* (*fMonoDomainGet)();
    typedef void* (*fMonoDomainAssemblyOpen)(void* domain, const char* file);
    typedef void* (*fMonoAssemblyGetImage)(void* assembly);
    typedef void* (*fMonoClassFromName)(void* image, const char* ns, const char* name);
    typedef void* (*fMonoClassGetMethodFromName)(void* klass, const char* name, int param_count);
    typedef void  (*fMonoFieldGetValue)(void* obj, void* classField, void* val);
    typedef void* (*fMonoRuntimeInvoke)(void* method, void* instance, void **params, void* exc);

    static char *assemblyToLoad;
    static char *assemblyNamespace;

    static bool Initialize(void *library);

    static void HookAndLoadAssembly();
    static void LoadAssemblyInternal();
};


#endif //UNITY3D_INJECT_MONO_H
