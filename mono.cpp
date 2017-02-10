//
// Created by nicholas on 07/02/17.
//

#include "mono.h"

#include <stdlib.h>
#include <cstdint>

#include "main.h"
#include "subhook/subhook.h"

subhook::Hook g_MonoGetDomainHook;
subhook::Hook g_MonoFieldGetValueHook;

char *mono::assemblyToLoad = NULL;
char *mono::assemblyNamespace = NULL;

mono::fmono_domain_get                 do_domain_get = 0;
mono::fmono_domain_assembly_open       do_domain_assembly_open;
mono::fmono_assembly_get_image         do_assembly_get_image;
mono::fmono_class_from_name            do_class_from_name;
mono::fmono_class_get_method_from_name do_class_get_method_from_name;
mono::fmono_field_get_value            do_field_get_value;
mono::fmono_runtime_invoke             do_runtime_invoke;

// Definitions taken from Mono git

typedef int8_t         gint8;
typedef uint8_t        guint8;
typedef int16_t        gint16;
typedef uint16_t       guint16;
typedef int32_t        gint32;
typedef uint32_t       guint32;
typedef int64_t        gint64;
typedef uint64_t       guint64;
typedef float          gfloat;
typedef double         gdouble;
typedef int32_t        gboolean;

struct _MonoAssemblyName {
    const char *name;
    const char *culture;
    const char *hash_value;
    const void* public_key;
    char public_key_token [/*MONO_PUBLIC_KEY_TOKEN_LENGTH*/17];
    uint32_t hash_alg;
    uint32_t hash_len;
    uint32_t flags;
    uint16_t major, minor, build, revision, arch;
};

struct MonoTypeNameParse {
    char *name_space;
    char *name;
    _MonoAssemblyName assembly;
    void *modifiers; /* 0 -> byref, -1 -> pointer, > 0 -> array rank */
    void *type_arguments;
    void *nested;
};

struct _MonoAssembly {
    int ref_count;
    char *basedir;
    _MonoAssemblyName aname;
    void *image;
    void *friend_assembly_names;
    guint8 friend_assembly_names_inited;
    guint8 in_gac;
    guint8 dynamic;
    guint8 corlib_internal;
    gboolean ref_only;
    guint8 wrap_non_exception_throws;
    guint8 wrap_non_exception_throws_inited;
    guint8 jit_optimizer_disabled;
    guint8 jit_optimizer_disabled_inited;

    guint32 ecma:2;
    guint32 aptc:2;
    guint32 fulltrust:2;
    guint32 unmanaged:2;
    guint32 skipverification:2;
};

// End definitions

void my_mono_get_domain()
{
    mono::LoadAssemblyInternal();
    do_domain_get();
}

void my_mono_field_get_value(void* obj, void* classField, void* value)
{
    g_MonoFieldGetValueHook.Remove();

    // Call original
    do_field_get_value(obj, classField, value);

    // Oh sorry Mono, looks like our assembly isn't real D:
    *(MonoPtr*)value = 0;
    value = NULL;
}

bool mono::Initialize(void *library)
{
    do_domain_get                 = (fmono_domain_get)dlsym(library, "mono_domain_get");
    do_domain_assembly_open       = (fmono_domain_assembly_open)dlsym(library, "mono_domain_assembly_open");
    do_assembly_get_image         = (fmono_assembly_get_image)dlsym(library, "mono_assembly_get_image");
    do_class_from_name            = (fmono_class_from_name)dlsym(library, "mono_class_from_name");
    do_class_get_method_from_name = (fmono_class_get_method_from_name)dlsym(library, "mono_class_get_method_from_name");
    do_field_get_value            = (fmono_field_get_value)dlsym(library, "mono_field_get_value");
    do_runtime_invoke             = (fmono_runtime_invoke)dlsym(library, "mono_runtime_invoke");

    return true;
};

void mono::LoadAssemblyInternal()
{
    void *domain;
    _MonoAssembly *assembly;
    void *image;
    void *klass;
    void *method;

    // Remove hook from mono_domain_get
    g_MonoGetDomainHook.Remove();

    // Integrity check of some sort
    if(!assemblyToLoad || !assemblyNamespace)
        return;

    domain = do_domain_get();
    assembly = (_MonoAssembly*)do_domain_assembly_open(domain, assemblyToLoad);

    // GetAssemblies hide method
    assembly->corlib_internal = 1;

    image = do_assembly_get_image(assembly);
    klass = do_class_from_name(image, assemblyNamespace, LOADER_CLASS);
    method = do_class_get_method_from_name(klass, LOADER_METHOD, 0);
    do_runtime_invoke(method, NULL, NULL, NULL);
}

void mono::HookAndLoadAssembly()
{
    // Both are single use and will be removed automatically

    g_MonoFieldGetValueHook.Install((void*)do_field_get_value, (void*)my_mono_field_get_value, subhook::HookOption64BitOffset);
    g_MonoGetDomainHook.Install((void*)do_domain_get, (void*)my_mono_get_domain, subhook::HookOption64BitOffset);
}