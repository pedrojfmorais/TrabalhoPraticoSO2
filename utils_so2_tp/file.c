#include "header.h"

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

	for (DWORD i = 0; i < N_JOGADORES; i++)
	{
		TCHAR nomeShareMemory[TAM];
		swprintf(nomeShareMemory, TAM, _T("%s%d"), SHM_NAME_JOGO, i);

		partilhaJogo->hMapFileJogos[i] = CreateFileMapping(
			INVALID_HANDLE_VALUE,
			NULL,
			PAGE_READWRITE,
			0,
			MSGBUFSIZE,
			nomeShareMemory
		);

		if (partilhaJogo->hMapFileJogos[i] == NULL) {
			_tprintf(_T("ERROR: CreateFileMapping (%d)\n"), GetLastError());
			return FALSE;
		}

		partilhaJogo->jogos[i] = (DadosJogo*)MapViewOfFile(
			partilhaJogo->hMapFileJogos[i],
			FILE_MAP_ALL_ACCESS,
			0,
			0,
			MSGBUFSIZE
		);

		if (partilhaJogo->jogos[i] == NULL) {
			_tprintf(_T("ERROR: MapViewOfFile (%d)\n"), GetLastError());
			CloseHandle(partilhaJogo->hMapFileJogos[i]);
			return FALSE;
		}
	}
	
	partilhaJogo->hRWMutex = CreateMutex(
		NULL,
		FALSE,
		MUTEX_NAME_PARTILHA_MAPA_JOGO
	);

	if (partilhaJogo->hRWMutex == NULL) {
		_tprintf(_T("ERROR: CreateMutex (%d)\n"), GetLastError());

		for (DWORD i = 0; i < N_JOGADORES; i++){
			UnmapViewOfFile(partilhaJogo->jogos[i]);
			CloseHandle(partilhaJogo->hMapFileJogos[i]);
		}
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
		for (DWORD i = 0; i < N_JOGADORES; i++) {
			UnmapViewOfFile(partilhaJogo->jogos[i]);
			CloseHandle(partilhaJogo->hMapFileJogos[i]);
		}
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
		for (DWORD i = 0; i < N_JOGADORES; i++) {
			UnmapViewOfFile(partilhaJogo->jogos[i]);
			CloseHandle(partilhaJogo->hMapFileJogos[i]);
		}
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
		for (DWORD i = 0; i < N_JOGADORES; i++) {
			UnmapViewOfFile(partilhaJogo->jogos[i]);
			CloseHandle(partilhaJogo->hMapFileJogos[i]);
		}
		CloseHandle(partilhaJogo->hRWMutex);
		CloseHandle(partilhaJogo->hEvent);
		CloseHandle(partilhaJogo->hEventJogosDecorrer);
		return FALSE;
	}

	// buffer circular monitor para servidor
	partilhaJogo->hMapFileBufferCircularMonitorParaServidor = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		MSGBUFSIZE,
		SHM_NAME_BUFFER_CIRCULAR_MONITOR_PARA_SERVIDOR
	);

	if (partilhaJogo->hMapFileBufferCircularMonitorParaServidor == NULL) {
		_tprintf(_T("ERROR: CreateFileMapping (%d)\n"), GetLastError());
		for (DWORD i = 0; i < N_JOGADORES; i++) {
			UnmapViewOfFile(partilhaJogo->jogos[i]);
			CloseHandle(partilhaJogo->hMapFileJogos[i]);
		}
		CloseHandle(partilhaJogo->hRWMutex);
		CloseHandle(partilhaJogo->hEvent);
		CloseHandle(partilhaJogo->hEventJogosDecorrer);
		return FALSE;
	}

	partilhaJogo->bufferCircularMonitorParaServidor = (BufferCircular*)MapViewOfFile(
		partilhaJogo->hMapFileBufferCircularMonitorParaServidor,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		sizeof(BufferCircular)
	);

	if (partilhaJogo->bufferCircularMonitorParaServidor == NULL) {
		_tprintf(_T("ERROR: MapViewOfFile (%d)\n"), GetLastError());
		for (DWORD i = 0; i < N_JOGADORES; i++) {
			UnmapViewOfFile(partilhaJogo->jogos[i]);
			CloseHandle(partilhaJogo->hMapFileJogos[i]);
		}
		CloseHandle(partilhaJogo->hRWMutex);
		CloseHandle(partilhaJogo->hEvent);
		CloseHandle(partilhaJogo->hEventJogosDecorrer);
		CloseHandle(partilhaJogo->hMapFileBufferCircularMonitorParaServidor);
		return FALSE;
	}

	partilhaJogo->hMutexBufferCircularMonitorParaServidor = CreateMutex(
		NULL,
		FALSE,
		MUTEX_NAME_BUFFER_CIRCULAR_MONITOR_PARA_SERVIDOR
	);

	if (partilhaJogo->hMutexBufferCircularMonitorParaServidor == NULL) {
		_tprintf(_T("ERROR: CreateMutex (%d)\n"), GetLastError());

		for (DWORD i = 0; i < N_JOGADORES; i++) {
			UnmapViewOfFile(partilhaJogo->jogos[i]);
			CloseHandle(partilhaJogo->hMapFileJogos[i]);
		}
		CloseHandle(partilhaJogo->hRWMutex);
		CloseHandle(partilhaJogo->hEvent);
		CloseHandle(partilhaJogo->hEventJogosDecorrer);
		UnmapViewOfFile(partilhaJogo->bufferCircularMonitorParaServidor);
		CloseHandle(partilhaJogo->hMapFileBufferCircularMonitorParaServidor);
		return FALSE;
	}

	partilhaJogo->hSemaforoEscritaBufferCircularMonitorParaServidor = CreateSemaphore(
		NULL,
		BUFFER_SIZE,
		BUFFER_SIZE,
		SEM_WRITE_NAME_BUFFER_CIRCULAR_MONITOR_PARA_SERVIDOR
	);
	if (partilhaJogo->hSemaforoEscritaBufferCircularMonitorParaServidor == NULL) {
		_tprintf(_T("ERROR: CreateSemaphore (%d)\n"), GetLastError());
		for (DWORD i = 0; i < N_JOGADORES; i++) {
			UnmapViewOfFile(partilhaJogo->jogos[i]);
			CloseHandle(partilhaJogo->hMapFileJogos[i]);
		}
		CloseHandle(partilhaJogo->hMutexBufferCircularMonitorParaServidor);
		CloseHandle(partilhaJogo->hRWMutex);
		CloseHandle(partilhaJogo->hEvent);
		CloseHandle(partilhaJogo->hEventJogosDecorrer);
		UnmapViewOfFile(partilhaJogo->bufferCircularMonitorParaServidor);
		CloseHandle(partilhaJogo->hMapFileBufferCircularMonitorParaServidor);
		return FALSE;
	}

	partilhaJogo->hSemaforoLeituraBufferCircularMonitorParaServidor = CreateSemaphore(
		NULL,
		0,
		BUFFER_SIZE,
		SEM_READ_NAME_BUFFER_CIRCULAR_MONITOR_PARA_SERVIDOR
	);
	if (partilhaJogo->hSemaforoLeituraBufferCircularMonitorParaServidor == NULL) {
		_tprintf(_T("ERROR: CreateSemaphore (%d)\n"), GetLastError());
		for (DWORD i = 0; i < N_JOGADORES; i++) {
			CloseHandle(partilhaJogo->hMapFileJogos[i]);
			UnmapViewOfFile(partilhaJogo->jogos[i]);
		}
		CloseHandle(partilhaJogo->hMutexBufferCircularMonitorParaServidor);
		CloseHandle(partilhaJogo->hRWMutex);
		CloseHandle(partilhaJogo->hEvent);
		CloseHandle(partilhaJogo->hEventJogosDecorrer);
		UnmapViewOfFile(partilhaJogo->bufferCircularMonitorParaServidor);
		CloseHandle(partilhaJogo->hMapFileBufferCircularMonitorParaServidor);
		CloseHandle(partilhaJogo->hSemaforoEscritaBufferCircularMonitorParaServidor);
		return FALSE;
	}

	partilhaJogo->hEventFecharTudo = CreateEvent(
		NULL,
		TRUE,
		FALSE,
		EVENT_FECHAR_TUDO
	);

	if (partilhaJogo->hEventFecharTudo == NULL) {
		_tprintf(_T("ERROR: CreateEvent (%d)\n"), GetLastError());
		for (DWORD i = 0; i < N_JOGADORES; i++) {
			CloseHandle(partilhaJogo->hMapFileJogos[i]);
			UnmapViewOfFile(partilhaJogo->jogos[i]);
		}
		CloseHandle(partilhaJogo->hMutexBufferCircularMonitorParaServidor);
		CloseHandle(partilhaJogo->hRWMutex);
		CloseHandle(partilhaJogo->hEvent);
		CloseHandle(partilhaJogo->hEventJogosDecorrer);
		UnmapViewOfFile(partilhaJogo->bufferCircularMonitorParaServidor);
		CloseHandle(partilhaJogo->hMapFileBufferCircularMonitorParaServidor);
		CloseHandle(partilhaJogo->hSemaforoEscritaBufferCircularMonitorParaServidor);
		CloseHandle(partilhaJogo->hSemaforoLeituraBufferCircularMonitorParaServidor);
		return FALSE;
	}

	return TRUE;
}