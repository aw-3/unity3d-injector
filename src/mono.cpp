//
// Created by nicholas on 07/02/17.
//

#include "mono.h"

#include "main.h"
#include "subhook.h"

#include <stdlib.h>
#include <dlfcn.h>

#include <cstdint>

subhook::Hook g_MonoGetDomainHook;
subhook::Hook g_MonoFieldGetValueHook;

char *mono::assemblyToLoad = NULL;
char *mono::assemblyNamespace = NULL;

mono::fMonoDomainGet              DoDomainGet;
mono::fMonoDomainAssemblyOpen     DoDomainAssemblyOpen;
mono::fMonoAssemblyGetImage       DoAssemblyGetImage;
mono::fMonoClassFromName          DoClassFromName;
mono::fMonoClassGetMethodFromName DoClassGetMethodFromName;
mono::fMonoFieldGetValue          DoFieldGetValue;
mono::fMonoRuntimeInvoke          DoRuntimeInvoke;

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
    DoDomainGet();
}

void my_mono_field_get_value(void* obj, void* classField, void* value)
{
    g_MonoFieldGetValueHook.Remove();

    // Call original
    DoFieldGetValue(obj, classField, value);

    // Oh sorry Mono, looks like our assembly isn't real D:
    *(MonoPtr*)value = 0;
    value = NULL;
}

bool mono::Initialize(void *library)
{
    DoDomainGet              = (fMonoDomainGet)dlsym(library, "mono_domain_get");
    DoDomainAssemblyOpen     = (fMonoDomainAssemblyOpen)dlsym(library, "mono_domain_assembly_open");
    DoAssemblyGetImage       = (fMonoAssemblyGetImage)dlsym(library, "mono_assembly_get_image");
    DoClassFromName          = (fMonoClassFromName)dlsym(library, "mono_class_from_name");
    DoClassGetMethodFromName = (fMonoClassGetMethodFromName)dlsym(library, "mono_class_get_method_from_name");
    DoFieldGetValue          = (fMonoFieldGetValue)dlsym(library, "mono_field_get_value");
    DoRuntimeInvoke          = (fMonoRuntimeInvoke)dlsym(library, "mono_runtime_invoke");

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

    domain = DoDomainGet();
    assembly = (_MonoAssembly*)DoDomainAssemblyOpen(domain, assemblyToLoad);

    // GetAssemblies hide method
    assembly->corlib_internal = 1;

    image = DoAssemblyGetImage(assembly);
    klass = DoClassFromName(image, assemblyNamespace, LOADER_CLASS);
    method = DoClassGetMethodFromName(klass, LOADER_METHOD, 0);
    DoRuntimeInvoke(method, NULL, NULL, NULL);
}

void mono::HookAndLoadAssembly()
{
    // Both hooks are single use and will be removed automatically

    g_MonoFieldGetValueHook.Install((void*)DoFieldGetValue, (void*)my_mono_field_get_value, subhook::HookOption64BitOffset);
    g_MonoGetDomainHook.Install((void*)DoDomainGet, (void*)my_mono_get_domain, subhook::HookOption64BitOffset);
}