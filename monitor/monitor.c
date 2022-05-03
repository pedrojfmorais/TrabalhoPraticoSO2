#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>

typedef struct {
	DWORD nLinhas;
	DWORD nColunas;
	DWORD tempoAguaComecaFluir;
	DWORD coordenadasOrigemAgua[2];
	DWORD coordenadasDestinoAgua[2];
	int mapaJogo[20][20];
} DadosMapaJogo;


//memória partilhada para o monitor
#define SHM_NAME _T("memoriaPartilhaMapaJogo")
#define SEMAPHORE_NAME _T("SEMAFORO_PartilhaMapaJogo")
#define MUTEX_NAME_PARTILHA_MAPA_JOGO _T("MutexAtualizarMapaJogo")
#define EVENT_NAME_PARTILHA_MAPA_JOGO _T("NovaAtualizacaoMapaJogo")
#define MSGTEXT_SZ 1000

#define MSGBUFSIZE sizeof(DadosMapaJogo)

typedef struct {
	HANDLE hMapFile;
	DadosMapaJogo* mapaJogo;
	int threadMustContinue;
	HANDLE newMsg;
	HANDLE hRWMutex;
	int* arrayMeu;
} PartilhaMapaJogo;

void desenharMapaJogo(DadosMapaJogo* dadosMapaJogo) {

	for (int i = 0; i < dadosMapaJogo->nLinhas; i++)
	{
		for (int j = 0; j < dadosMapaJogo->nColunas; j++)
		{
			if (i == dadosMapaJogo->coordenadasOrigemAgua[0] && j == dadosMapaJogo->coordenadasOrigemAgua[1])
				_tprintf(_T("C "));
			else if (i == dadosMapaJogo->coordenadasDestinoAgua[0] && j == dadosMapaJogo->coordenadasDestinoAgua[1])
				_tprintf(_T("D "));
			else
				_tprintf(_T("%d "), dadosMapaJogo->mapaJogo[i][j]);
		}
		_tprintf(_T("\n"));
	}

}

BOOL WINAPI recebeMapaJogoDoServidor(LPVOID p) {

	PartilhaMapaJogo* pcd = (PartilhaMapaJogo*)p;
	DadosMapaJogo msg;

	while (1) {
		WaitForSingleObject(pcd->newMsg, INFINITE);
		if (!pcd->threadMustContinue)
			return 0;

		WaitForSingleObject(pcd->hRWMutex, INFINITE);
		CopyMemory(&msg, pcd->mapaJogo, sizeof(DadosMapaJogo));

		ReleaseMutex(pcd->hRWMutex);

		desenharMapaJogo(pcd->mapaJogo);

		Sleep(1000);
	}
	return 0;
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
	else if (GetLastError() != ERROR_ALREADY_EXISTS) {
		_tprintf(_T("ERROR: Programa Servidor ainda não foi aberto! \n"));
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
}

int _tmain(int argc, TCHAR* argv[]) {

#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	_setmode(_fileno(stderr), _O_WTEXT);
#endif 

	PartilhaMapaJogo dadosPartilhaMapaJogo;

	if (!initMemAndSync(&dadosPartilhaMapaJogo))
		return 0;

	CloseHandle(dadosPartilhaMapaJogo.hMapFile);
	HANDLE hThread = CreateThread(NULL, 0, recebeMapaJogoDoServidor, &dadosPartilhaMapaJogo, 0, NULL);

	WaitForSingleObject(hThread, INFINITE);
	UnmapViewOfFile(dadosPartilhaMapaJogo.mapaJogo);
	CloseHandle(dadosPartilhaMapaJogo.hMapFile);
	CloseHandle(dadosPartilhaMapaJogo.hRWMutex);
	CloseHandle(dadosPartilhaMapaJogo.newMsg);
	CloseHandle(hThread);
	CloseHandle(hThread);

	return 0;
}