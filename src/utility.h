//
// Created by nicholas on 06/02/17.
//

#ifndef UNITY3D_INJECT_UTILITY_H
#define UNITY3D_INJECT_UTILITY_H

#include <vector>
#include <string>

class utility {
public:
    static void* GetModuleHandle(const char *moduleName);
    static std::vector<char> FileReadAllBytes(std::string filePath);
};


#endif //UNITY3D_INJECT_UTILITY_H
