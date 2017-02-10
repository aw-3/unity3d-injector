//
// Created by nicholas on 06/02/17.
//

#ifndef UNITY3D_INJECT_MAIN_H
#define UNITY3D_INJECT_MAIN_H

#include "types.h"

#define LOADER_CLASS "Loader"
#define LOADER_METHOD "Load"

extern "C" {
    void LoadMonoDll(char *dllPath, char *ns);
};

#endif //UNITY3D_INJECT_MAIN_H
