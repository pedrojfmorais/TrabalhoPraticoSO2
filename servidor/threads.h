#pragma once

#include"..\utils_so2_tp\utils_so2_tp.h"

BOOL WINAPI atualizaMapaJogoParaMonitor(LPVOID p);
BOOL WINAPI decorrerJogo(LPVOID p);
BOOL WINAPI recebeMensagemMonitor(LPVOID p);
DWORD WINAPI leMensagemUtilizador(LPVOID p);
DWORD WINAPI acabarThreads(LPVOID p);
BOOL WINAPI criaNamedPipeParaClientesTabuleiroJogo(LPVOID p);
DWORD WINAPI clienteConectaNamedPipe(LPVOID p);
BOOL WINAPI criaNamedPipeParaClientesMensagens(LPVOID p);