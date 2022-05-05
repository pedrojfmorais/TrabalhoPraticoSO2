#pragma once

#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>

// s� define a fun��o uma vez
#ifndef UTILS_SO2_TP_H
#define UTILS_SO2_TP_H

DWORD getRandomNumberBetweenMaxAndMin(DWORD min, DWORD max);

#endif  /* UTILS_SO2_TP_H */

#define TAM 200
#define N_JOGADORES 2
#define caminhoRegistry _T("software\\so2\\tp\\")

//mem�ria partilhada para o monitor
#define SHM_NAME_JOGO _T("memoriaPartilhaJogador")
#define SEMAPHORE_NAME _T("SEMAFORO_PartilhaMapaJogo")
#define MUTEX_NAME_PARTILHA_MAPA_JOGO _T("MutexAtualizarMapaJogo")
#define EVENT_NAME_PARTILHA_MAPA_JOGO _T("NovaAtualizacaoMapaJogo")
#define EVENT_NAME_JOGOS_A_DECORRER _T("ExistemJogosDecorrer")
#define MSGTEXT_SZ 1000

#define MSGBUFSIZE sizeof(DadosJogo)

//buffer circular monitor para servidor
#define SHM_NAME_BUFFER_CIRCULAR_MONITOR_PARA_SERVIDOR _T("memoriaPartilhaMonitorParaServidor")
#define MUTEX_NAME_BUFFER_CIRCULAR_MONITOR_PARA_SERVIDOR TEXT("MUTEX_BUFFER_CIRCULAR_MONITOR_PARA_SERVIDOR")
#define SEM_WRITE_NAME_BUFFER_CIRCULAR_MONITOR_PARA_SERVIDOR TEXT("SEM_WRITE_BUFFER_CIRCULAR_MONITOR_PARA_SERVIDOR")
#define SEM_READ_NAME_BUFFER_CIRCULAR_MONITOR_PARA_SERVIDOR TEXT("SEM_READ_BUFFER_CIRCULAR_MONITOR_PARA_SERVIDOR")

//buffer circular servidor para monitor
#define SHM_NAME_BUFFER_CIRCULAR_SERVIDOR_PARA_MONITOR _T("memoriaPartilhaServidorParaMonitor")
#define MUTEX_NAME_BUFFER_CIRCULAR_SERVIDOR_PARA_MONITOR TEXT("MUTEX_BUFFER_CIRCULAR_SERVIDOR_PARA_MONITOR")
#define SEM_WRITE_NAME_BUFFER_CIRCULAR_SERVIDOR_PARA_MONITOR TEXT("SEM_WRITE_BUFFER_CIRCULAR_SERVIDOR_PARA_MONITOR")
#define SEM_READ_NAME_BUFFER_CIRCULAR_SERVIDOR_PARA_MONITOR TEXT("SEM_READ_BUFFER_CIRCULAR_SERVIDOR_PARA_MONITOR")

// Um dos tubos multiplicado por 10 significa que tem �gua, exceto tuboVazio
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

	int mapaJogo[20][20]; //m�ximo tamanho do mapa

	DWORD proximaPeca;
} DadosJogo;

//defini��es do jogo
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

	DWORD wP; // Posi��o do buffer circular para escrita
	DWORD rP; // Posi��o do buffer circular para leitura

	BufferCell buffer[BUFFER_SIZE]; // Buffer Circular

} BufferCircular;

//estrutura de control
typedef struct {
	HANDLE hMapFileJogos[N_JOGADORES];

	DadosJogo* jogos[N_JOGADORES];

	DefinicoesJogo definicoesJogo;

	BOOL threadMustContinue;

	HANDLE hEvent;
	HANDLE hEventJogosDecorrer;
	
	HANDLE hRWMutex;
	
	HANDLE hSemaforo;

	//bufferCircular Monitor para Servidor
	HANDLE hMapFileBufferCircularServidorParaMonitor;
	BufferCircular* bufferCircularServidorParaMonitor;

	HANDLE hMutexBufferCircularServidorParaMonitor;

	HANDLE hSemaforoLeituraBufferCircularServidorParaMonitor;
	HANDLE hSemaforoEscritaBufferCircularServidorParaMonitor;

	//bufferCircular Servidor para Monitor
	HANDLE hMapFileBufferCircularMonitorParaServidor;
	BufferCircular* bufferCircularMonitorParaServidor;

	HANDLE hMutexBufferCircularMonitorParaServidor;

	HANDLE hSemaforoLeituraBufferCircularMonitorParaServidor;
	HANDLE hSemaforoEscritaBufferCircularMonitorParaServidor;

} PartilhaJogo;

typedef BOOL(*PFUNC_TypeBool_NoArguments) ();
typedef BOOL(*PFUNC_TypeBool_PointerPartilhaMapaJogo) (PartilhaJogo*);
