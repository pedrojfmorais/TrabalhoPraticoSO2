#pragma once

#include "..\\utils_so2_tp\utils_so2_tp.h"
#include "mostraMapaJogo.h"

BOOL WINAPI recebeMapaJogoDoServidor(LPVOID p);
void enviarMensagemServidor(PartilhaJogo* partilhaJogo, TCHAR* mensagem);
DWORD WINAPI enviaMensagemServidor(LPVOID p);
DWORD WINAPI acabarThreads(LPVOID p);
