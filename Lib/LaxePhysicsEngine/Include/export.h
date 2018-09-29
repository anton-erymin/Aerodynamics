#pragma once

#ifdef COMPILE_ALCHEMY
#define DLL_EXPORT

#else 

#ifdef COMPILE_DLL
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT //__declspec(dllimport)
#endif

#endif

void trace(char *s, ...);