#pragma once

#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>

#define TAM 200
#define caminhoRegistry _T("software\\so2\\tp\\")

//memória partilhada para o monitor
#define SHM_NAME_JOGADOR1 _T("memoriaPartilhaJogador1")
#define SHM_NAME_JOGADOR2 _T("memoriaPartilhaJogador2")
#define SEMAPHORE_NAME _T("SEMAFORO_PartilhaMapaJogo")
#define MUTEX_NAME_PARTILHA_MAPA_JOGO _T("MutexAtualizarMapaJogo")
#define EVENT_NAME_PARTILHA_MAPA_JOGO _T("NovaAtualizacaoMapaJogo")
#define MSGTEXT_SZ 1000

#define MSGBUFSIZE sizeof(DadosJogo)

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
#define CorVermelho 4
#define CorBranco 7
#define CorVerde 10
#define CorAzulClaro 11
#define CorSalmao 12
#define CorAmareloClaro 14

typedef struct {

	DWORD idJogador;

	BOOL aJogar;
	BOOL ganhou;
	BOOL jogoPausado;

	DWORD pontuacao;

	DWORD nLinhas;
	DWORD nColunas;

	DWORD tempoAguaComecaFluir;
	DWORD tempoDecorrido;

	DWORD coordenadasOrigemAgua[2];
	DWORD coordenadasDestinoAgua[2];

	DWORD coordenadaAtualAgua[2];

	int mapaJogo[20][20];
} DadosJogo;

typedef struct {
	HANDLE hMapFileJogador1;
	HANDLE hMapFileJogador2;
	DadosJogo* jogador1;
	DadosJogo* jogador2;
	int threadMustContinue;
	HANDLE hEvent;
	HANDLE hRWMutex;
	HANDLE hSemaforo;
} PartilhaJogo;

typedef BOOL(*PFUNC_TypeBool_NoArguments) ();
typedef BOOL(*PFUNC_TypeBool_PointerPartilhaMapaJogo) (PartilhaJogo*);