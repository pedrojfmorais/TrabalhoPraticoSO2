#pragma once
#include "utils_so2_tp.h"

#ifdef UTILS_EXPORTS
#define DLL_IMP_API __declspec(dllexport)
#else
#define DLL_IMP_API __declspec(dllimport)
#endif

DLL_IMP_API BOOL verificaServidorJaEstaCorrer();
DLL_IMP_API BOOL initMemAndSync(PartilhaJogoServidorMonitor* partilhaJogo);