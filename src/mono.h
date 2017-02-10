//
// Created by nicholas on 07/02/17.
//

#ifndef UNITY3D_INJECT_MONO_H
#define UNITY3D_INJECT_MONO_H

class mono {
public:
    typedef void* (*fmono_domain_get)();
    typedef void* (*fmono_domain_assembly_open)(void* domain, const char* file);
    typedef void* (*fmono_assembly_get_image)(void* assembly);
    typedef void* (*fmono_class_from_name)(void* image, const char* ns, const char* name);
    typedef void* (*fmono_class_get_method_from_name)(void* klass, const char* name, int param_count);
    typedef void  (*fmono_field_get_value)(void* obj, void* classField, void* val);
    typedef void* (*fmono_runtime_invoke)(void* method, void* instance, void **params, void* exc);

    static char *assemblyToLoad;
    static char *assemblyNamespace;

    static bool Initialize(void *library);

    static void HookAndLoadAssembly();
    static void LoadAssemblyInternal();
};


#endif //UNITY3D_INJECT_MONO_H
