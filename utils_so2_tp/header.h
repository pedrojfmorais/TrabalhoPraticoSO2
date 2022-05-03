#pragma once
#include <windows.h>

#ifdef UTILS_EXPORTS
#define DLL_IMP_API __declspec(dllexport)
#else
#define DLL_IMP_API __declspec(dllimport)
#endif

DLL_IMP_API BOOL verificaServidorJaEstaCorrer();