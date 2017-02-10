//
// Created by nicholas on 06/02/17.
//

#ifndef UNITY3D_INJECT_TYPES_H
#define UNITY3D_INJECT_TYPES_H

#if defined __linux__
  #define UNITY3D_INJECT_LINUX
#elif defined __WIN32
  #error "Windows is not yet supported"
  #define UNITY3D_INJECT_WINDOWS
#elif defined __APPLE__
  #error "OS X is not yet supported"
  #define UNITY3D_INJECT_OSX
#endif

#if defined __i386__ || defined _M_IX86
#define MonoPtr int32_t
#elif defined __amd64__ || defined _M_AMD64
#define MonoPtr int64_t
#endif

#endif //UNITY3D_INJECT_TYPES_H
