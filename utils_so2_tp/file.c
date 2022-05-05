#include "header.h"
#include "utils_so2_tp.h"

#define MUTEX_VERIFY_SERVER_OPEN _T("MutexServidorExiste")

BOOL verificaServidorJaEstaCorrer() {
	HANDLE returnCreate = CreateMutex(
		NULL,
		FALSE,
		MUTEX_VERIFY_SERVER_OPEN
	);
	if (returnCreate == NULL)
		return TRUE;

	else if (GetLastError() == ERROR_ALREADY_EXISTS)
		return TRUE;

	return FALSE;
}

BOOL initMemAndSync(PartilhaJogo* partilhaJogo) {

	partilhaJogo->hMapFileJogador1 = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		MSGBUFSIZE,
		SHM_NAME_JOGADOR1
	);

	if (partilhaJogo->hMapFileJogador1 == NULL) {
		_tprintf(_T("ERROR: CreateFileMapping (%d)\n"), GetLastError());
		return FALSE;
	}

	partilhaJogo->jogador1 = (DadosJogo*)MapViewOfFile(
		partilhaJogo->hMapFileJogador1,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		MSGBUFSIZE
	);


	if (partilhaJogo->jogador1 == NULL) {
		_tprintf(_T("ERROR: MapViewOfFile (%d)\n"), GetLastError());
		CloseHandle(partilhaJogo->hMapFileJogador1);
		return FALSE;
	}

	partilhaJogo->hMapFileJogador2 = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		MSGBUFSIZE,
		SHM_NAME_JOGADOR2
	);

	if (partilhaJogo->hMapFileJogador2 == NULL) {
		_tprintf(_T("ERROR: CreateFileMapping (%d)\n"), GetLastError());
		UnmapViewOfFile(partilhaJogo->jogador1);
		CloseHandle(partilhaJogo->hMapFileJogador1);
		return FALSE;
	}

	partilhaJogo->jogador2 = (DadosJogo*)MapViewOfFile(
		partilhaJogo->hMapFileJogador2,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		MSGBUFSIZE
	);


	if (partilhaJogo->jogador2 == NULL) {
		_tprintf(_T("ERROR: MapViewOfFile (%d)\n"), GetLastError());
		CloseHandle(partilhaJogo->hMapFileJogador2);
		UnmapViewOfFile(partilhaJogo->jogador1);
		CloseHandle(partilhaJogo->hMapFileJogador1);
		return FALSE;
	}

	partilhaJogo->hRWMutex = CreateMutex(
		NULL,
		FALSE,
		MUTEX_NAME_PARTILHA_MAPA_JOGO
	);

	if (partilhaJogo->hRWMutex == NULL) {
		_tprintf(_T("ERROR: CreateMutex (%d)\n"), GetLastError());
		UnmapViewOfFile(partilhaJogo->jogador1);
		CloseHandle(partilhaJogo->hMapFileJogador1);
		UnmapViewOfFile(partilhaJogo->jogador2);
		CloseHandle(partilhaJogo->hMapFileJogador2);
		return FALSE;
	}

	partilhaJogo->hEvent = CreateEvent(
		NULL,
		TRUE,
		FALSE,
		EVENT_NAME_PARTILHA_MAPA_JOGO
	);

	if (partilhaJogo->hEvent == NULL) {
		_tprintf(_T("ERROR: CreateEvent (%d)\n"), GetLastError());
		UnmapViewOfFile(partilhaJogo->jogador1);
		CloseHandle(partilhaJogo->hMapFileJogador1);
		UnmapViewOfFile(partilhaJogo->jogador2);
		CloseHandle(partilhaJogo->hMapFileJogador2);
		CloseHandle(partilhaJogo->hRWMutex);
		return FALSE;
	}

	partilhaJogo->hEventJogosDecorrer = CreateEvent(
		NULL,
		TRUE,
		FALSE,
		EVENT_NAME_JOGOS_A_DECORRER
	);

	if (partilhaJogo->hEventJogosDecorrer == NULL) {
		_tprintf(_T("ERROR: CreateEvent (%d)\n"), GetLastError());
		UnmapViewOfFile(partilhaJogo->jogador1);
		CloseHandle(partilhaJogo->hMapFileJogador1);
		UnmapViewOfFile(partilhaJogo->jogador2);
		CloseHandle(partilhaJogo->hMapFileJogador2);
		CloseHandle(partilhaJogo->hRWMutex);
		CloseHandle(partilhaJogo->hEvent);
		return FALSE;
	}
	partilhaJogo->hSemaforo = CreateSemaphore(
		NULL,
		0,
		1,
		SEMAPHORE_NAME
	);
	if (partilhaJogo->hSemaforo == NULL) {
		_tprintf(_T("ERROR: CreateSemaphore (%d)\n"), GetLastError());
		UnmapViewOfFile(partilhaJogo->jogador1);
		CloseHandle(partilhaJogo->hMapFileJogador1);
		UnmapViewOfFile(partilhaJogo->jogador2);
		CloseHandle(partilhaJogo->hMapFileJogador2);
		CloseHandle(partilhaJogo->hRWMutex);
		CloseHandle(partilhaJogo->hEvent);
		CloseHandle(partilhaJogo->hEventJogosDecorrer);
		return FALSE;
	}
	return TRUE;
}