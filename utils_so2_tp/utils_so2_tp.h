#pragma once

#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// só define a função uma vez
#ifndef UTILS_SO2_TP_H
#define UTILS_SO2_TP_H

DWORD getRandomNumberBetweenMaxAndMin(DWORD min, DWORD max);

#endif  /* UTILS_SO2_TP_H */

//Tamanho strings
#define TAM 200
//Configurações para o servidor
#define N_JOGADORES 2
#define N_SEGUNDOS_AVANCO_AGUA 2
#define CAMINHO_REGISTRY _T("software\\so2\\tp\\")
#define PIPE_NAME_TABULEIRO _T("\\\\.\\pipe\\pipeTabuleiro")
#define PIPE_NAME_MENSAGENS _T("\\\\.\\pipe\\pipeMensagens")


//Evento para o fecho de todas as aplicações
#define EVENT_FECHAR_TUDO _T("fecharTudo")

//memória partilhada para o monitor
#define SHM_NAME_JOGO _T("memoriaPartilhaJogador")
#define SEMAPHORE_NAME _T("SEMAFORO_PartilhaMapaJogo")
#define MUTEX_NAME_PARTILHA_MAPA_JOGO _T("MutexAtualizarMapaJogo")
#define EVENT_NAME_PARTILHA_MAPA_JOGO _T("NovaAtualizacaoMapaJogo")
#define EVENT_NAME_JOGOS_A_DECORRER _T("ExistemJogosDecorrer")
#define WAITABLE_TIMER _T("EsperarPelosMonitores")
#define MSGTEXT_SZ 1000

#define MSGBUFSIZE sizeof(DadosJogo)

//buffer circular monitor para servidor
#define SHM_NAME_BUFFER_CIRCULAR_MONITOR_PARA_SERVIDOR _T("memoriaPartilhaMonitorParaServidor")
#define MUTEX_NAME_BUFFER_CIRCULAR_MONITOR_PARA_SERVIDOR TEXT("MUTEX_BUFFER_CIRCULAR_MONITOR_PARA_SERVIDOR")
#define SEM_WRITE_NAME_BUFFER_CIRCULAR_MONITOR_PARA_SERVIDOR TEXT("SEM_WRITE_BUFFER_CIRCULAR_MONITOR_PARA_SERVIDOR")
#define SEM_READ_NAME_BUFFER_CIRCULAR_MONITOR_PARA_SERVIDOR TEXT("SEM_READ_BUFFER_CIRCULAR_MONITOR_PARA_SERVIDOR")

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
#define CorAzul 9
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

	DWORD tempoPararAgua;
	BOOL modoAleatorioLigado;

} DadosJogo;

//definições do jogo
typedef struct {

	DWORD nLinhas;
	DWORD nColunas;

	DWORD tempoAguaComecaFluir;

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

} BufferCircular;

//estrutura de control
typedef struct {
	HANDLE hMapFileJogos[N_JOGADORES];

	DadosJogo* jogos[N_JOGADORES];

	DefinicoesJogo definicoesJogo;

	//Evento que define quando fechar todos os programas servidor, monitor
	HANDLE hEventFecharTudo;

	HANDLE hEventAtualizacaoNoJogo;
	HANDLE hEventJogosDecorrer;
	
	HANDLE hReadWriteMutexAtualizacaoNoJogo;
	
	HANDLE hSemaforoEnviarAtualizacoesJogo;

	//bufferCircular Monitor para Servidor
	HANDLE hMapFileBufferCircularMonitorParaServidor;
	BufferCircular* bufferCircularMonitorParaServidor;

	HANDLE hMutexBufferCircularMonitorParaServidor;

	HANDLE hSemaforoLeituraBufferCircularMonitorParaServidor;
	HANDLE hSemaforoEscritaBufferCircularMonitorParaServidor;

	int deveContinuar;

	HANDLE hTimer;

} PartilhaJogoServidorMonitor;

//Struct para guardar todas as threads do servidor
#define N_THREADS_SERVIDOR 4
typedef struct {

	HANDLE hThreads[N_THREADS_SERVIDOR];
	
	HANDLE hEventFecharTudo;
	
	int* deveContinuar;

} ThreadsServidor;

//Struct para os namedPipe
typedef struct {
	HANDLE hInstancia;

	OVERLAPPED overlap; //estrutura overlapped para uso asincrono

	BOOL ativo; //para ver se já tem cliente ou não
} DadosPipe;

typedef struct {
	DadosPipe hPipes[N_JOGADORES];
	
	DadosJogo* jogos[N_JOGADORES];

	HANDLE hEvents[N_JOGADORES];

	HANDLE hReadWriteMutexAtualizacaoNoJogo;

	int deveContinuar;

} PartilhaJogoServidorCliente;


typedef struct {
	DadosPipe hPipes[N_JOGADORES];
	
	BufferCell* mensagens[N_JOGADORES];

	HANDLE hEvents[N_JOGADORES];

	HANDLE hMutex;

	int deveContinuar;
} MensagensServidorCliente;

//Struct para guardar todas as threads do monitor
#define N_THREADS_MONITOR 2
typedef struct {
	HANDLE hThreads[N_THREADS_MONITOR];
	
	HANDLE hEventFecharTudo;
	
	int* deveContinuar;

} ThreadsMonitor;

typedef struct {
	DadosPipe hPipes[N_JOGADORES];
	
	HANDLE hEvents[N_JOGADORES];

	HANDLE hMutex;

	int* deveContinuar;
} ThreadsCliente;

//definição de keyword usada para importar funções da DLL
typedef BOOL(*PFUNC_TypeBool_NoArguments) ();
typedef BOOL(*PFUNC_TypeBool_PointerPartilhaMapaJogo) (PartilhaJogoServidorMonitor*);
