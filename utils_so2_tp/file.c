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

BOOL initMemAndSync(PartilhaMapaJogo* cdata) {

	cdata->hMapFile = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		MSGBUFSIZE,
		SHM_NAME
	);

	if (cdata->hMapFile == NULL) {
		_tprintf(_T("ERROR: CreateFileMapping (%d)\n"), GetLastError());
		return FALSE;
	}

	cdata->mapaJogo = (DadosMapaJogo*)MapViewOfFile(
		cdata->hMapFile,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		MSGBUFSIZE
	);


	if (cdata->hMapFile == NULL) {
		_tprintf(_T("ERROR: MapViewOfFile (%d)\n"), GetLastError());
		CloseHandle(cdata->hMapFile);
		return FALSE;
	}

	cdata->hRWMutex = CreateMutex(
		NULL,
		FALSE,
		MUTEX_NAME_PARTILHA_MAPA_JOGO
	);

	if (cdata->hRWMutex == NULL) {
		_tprintf(_T("ERROR: CreateMutex (%d)\n"), GetLastError());
		UnmapViewOfFile(cdata->mapaJogo);
		CloseHandle(cdata->hMapFile);
		return FALSE;
	}

	cdata->newMsg = CreateEvent(
		NULL,
		TRUE,
		FALSE,
		EVENT_NAME_PARTILHA_MAPA_JOGO
	);

	if (cdata->newMsg == NULL) {
		_tprintf(_T("ERROR: CreateEvent (%d)\n"), GetLastError());
		UnmapViewOfFile(cdata->mapaJogo);
		CloseHandle(cdata->hMapFile);
		CloseHandle(cdata->hRWMutex);
		return FALSE;
	}
	cdata->hSemaforo = CreateSemaphore(
		NULL,
		0,
		1,
		SEMAPHORE_NAME
	);
	if (cdata->hSemaforo == NULL) {
		_tprintf(_T("ERROR: CreateSemaphore (%d)\n"), GetLastError());
		UnmapViewOfFile(cdata->mapaJogo);
		CloseHandle(cdata->hMapFile);
		CloseHandle(cdata->hRWMutex);
		CloseHandle(cdata->newMsg);
		return FALSE;
	}
}