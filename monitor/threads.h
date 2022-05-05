#pragma once

#include "..\\utils_so2_tp\utils_so2_tp.h"
#include "mostraMapaJogo.h"

BOOL WINAPI recebeMapaJogoDoServidor(LPVOID p);
DWORD WINAPI enviaMensagemServidor(LPVOID p);
BOOL WINAPI recebeMensagemServidor(LPVOID p);
