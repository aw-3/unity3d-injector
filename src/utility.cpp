//
// Created by nicholas on 06/02/17.
//

#include "utility.h"

#include <link.h>

#include <cstring>
#include <string>
#include <vector>
#include <fstream>

void* utility::GetModuleHandle(const char *moduleName)
{
    std::vector<const char*> modules;

    dl_iterate_phdr([](struct dl_phdr_info* info, size_t size, void* data)
    {
        ((std::vector<const char*>*)data)->push_back(info->dlpi_name);
        return 0;
    }, &modules);

    for(auto module : modules)
    {
        //if(!strcmp(module, moduleName))
        if(std::string(module).find(moduleName) != std::string::npos)
        {
            return dlopen(module, RTLD_NOLOAD | RTLD_NOW);
        }
    }

    return NULL;
}

std::vector<char> utility::FileReadAllBytes(std::string filePath)
{
    std::ifstream input(filePath, std::ios::binary);

    if (input.is_open()) {
        std::vector<char> buffer(
                (std::istreambuf_iterator<char>(input)),
                (std::istreambuf_iterator<char>())
        );
        return buffer;
    }
    return std::vector<char>();
}