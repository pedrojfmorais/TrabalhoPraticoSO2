#pragma once

#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>

// só define a função uma vez
#ifndef UTILS_SO2_TP_H
#define UTILS_SO2_TP_H

DWORD getRandomNumberBetweenMaxAndMin(DWORD min, DWORD max);

#endif  /* UTILS_SO2_TP_H */

#define TAM 200
#define N_JOGADORES 2
#define caminhoRegistry _T("software\\so2\\tp\\")

//memória partilhada para o monitor
#define SHM_NAME_JOGO _T("memoriaPartilhaJogador")
#define SEMAPHORE_NAME _T("SEMAFORO_PartilhaMapaJogo")
#define MUTEX_NAME_PARTILHA_MAPA_JOGO _T("MutexAtualizarMapaJogo")
#define EVENT_NAME_PARTILHA_MAPA_JOGO _T("NovaAtualizacaoMapaJogo")
#define EVENT_NAME_JOGOS_A_DECORRER _T("ExistemJogosDecorrer")
#define MSGTEXT_SZ 1000

#define MSGBUFSIZE sizeof(DadosJogo)

// Um dos tubos multiplicado por 10 significa que tem água, exceto tuboVazio
#define tuboVazio 0
#define tuboBloqueado 1
#define tuboOrigemAgua 2
#define tuboDestinoAgua 3
#define tuboHorizontal 4
#define tuboVertical 5
#define tuboCurvaEsquerdaCima 6
#define tuboCurvaEsquerdaBaixo 7
#define tuboCurvaDireitaCima 8
#define tuboCurvaDireitaBaixo 9

// Cores para a consola
#define CorVermelho 4
#define CorBranco 7
#define CorVerde 10
#define CorAzulClaro 11
#define CorSalmao 12
#define CorAmareloClaro 14

//representa um jogo
typedef struct {

	DWORD idJogador;

	BOOL aJogar;
	BOOL jogoPausado;

	BOOL ganhou;
	BOOL perdeu;

	DWORD pontuacao;

	DWORD nLinhas;
	DWORD nColunas;

	DWORD tempoAguaComecaFluir;
	DWORD tempoDecorrido;

	DWORD coordenadasOrigemAgua[2];
	DWORD coordenadasDestinoAgua[2];

	DWORD coordenadaAtualAgua[2];

	int mapaJogo[20][20]; //máximo tamanho do mapa

	DWORD proximaPeca;
} DadosJogo;

//definições do jogo
typedef struct {

	DWORD nLinhas;
	DWORD nColunas;

	DWORD tempoAguaComecaFluir;

	DWORD tempoPararAgua;
	BOOL modoAleatorioLigado;

} DefinicoesJogo;

//Tamanho buffer circular
#define BUFFER_SIZE 10

typedef struct _BufferCell {
	TCHAR mensagem[TAM];
} BufferCell;

typedef struct {

	DWORD wP; // Posição do buffer circular para escrita
	DWORD rP; // Posição do buffer circular para leitura

	BufferCell buffer[BUFFER_SIZE]; // Buffer Circular

	HANDLE hMutex;

	HANDLE hSemaforoLeitura;
	HANDLE hSemaforoEscrita;

} SharedMemBufferCircular;

typedef struct {
	HANDLE hMapFileJogos[N_JOGADORES];

	DadosJogo* jogos[N_JOGADORES];

	DefinicoesJogo definicoesJogo;

	int threadMustContinue;

	HANDLE hEvent;
	HANDLE hEventJogosDecorrer;
	
	HANDLE hRWMutex;
	
	HANDLE hSemaforo;

} PartilhaJogo;

typedef BOOL(*PFUNC_TypeBool_NoArguments) ();
typedef BOOL(*PFUNC_TypeBool_PointerPartilhaMapaJogo) (PartilhaJogo*);
