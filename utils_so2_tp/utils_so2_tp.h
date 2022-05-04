#pragma once

#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>

#define TAM 200
#define caminhoRegistry _T("software\\so2\\tp\\")

//memória partilhada para o monitor
#define SHM_NAME _T("memoriaPartilhaMapaJogo")
#define SEMAPHORE_NAME _T("SEMAFORO_PartilhaMapaJogo")
#define MUTEX_NAME_PARTILHA_MAPA_JOGO _T("MutexAtualizarMapaJogo")
#define EVENT_NAME_PARTILHA_MAPA_JOGO _T("NovaAtualizacaoMapaJogo")
#define MSGTEXT_SZ 1000

#define MSGBUFSIZE sizeof(DadosMapaJogo)

// Um dos tubos multiplicado por 10 significa que tem água, exceto tuboVazio
#define tuboVazio 0
#define tuboOrigemAgua 1
#define tuboDestinoAgua 2
#define tuboHorizontal 3
#define tuboVertical 4
#define tuboCurvaEsquerdaParaCima 5
#define tuboEsquerdaParaBaixo 6
#define tuboCurvaDireitaParaCima 7
#define tuboDireitaParaBaixo 8

// Cores para a consola
#define CorBranco 7
#define CorCinzento 14
#define CorAzulClaro 11

typedef struct {
	DWORD nLinhas;
	DWORD nColunas;
	DWORD tempoAguaComecaFluir;
	DWORD coordenadasOrigemAgua[2];
	DWORD coordenadasDestinoAgua[2];
	int mapaJogo[20][20];
} DadosMapaJogo;

typedef struct {
	HANDLE hMapFile;
	DadosMapaJogo* mapaJogo;
	int threadMustContinue;
	HANDLE newMsg;
	HANDLE hRWMutex;
	HANDLE hSemaforo;
} PartilhaMapaJogo;