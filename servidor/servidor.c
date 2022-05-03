#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>

#define TAM 200
#define caminhoRegistry _T("software\\so2\\tp\\")

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
	HANDLE hSemaforo;
} PartilhaMapaJogo;

DWORD getRandomNumberBetweenMaxAndMin(DWORD min, DWORD max) {
	return (rand() % (max - min)) + min;
}

void inicializaServidor(int argc, TCHAR* argv[], DadosMapaJogo *dadosMapaJogo) {

	TCHAR nomeChaves[3][TAM] = { _T("nLinhas"), _T("nColunas"), _T("tempoAguaComecaFluir") };
	DWORD val[3] = { 0 };

	if (argc == 1) {
		//Lê no registry

		if (!lerDoRegistryDadosMapaJogo(nomeChaves, val))
			exit(1);

	}else if(argc == 4) {
		//Guarda no registry

		for (DWORD i = 0; i < 3; i++)
			val[i] = wcstod(argv[i+1], _T('\0'));

		for (DWORD i = 0; i < 2; i++)
			if (val[i] > 20)
				val[i] = 20;
			else if (val[i] < 5)
				val[i] = 5;

		if(!guardaNoRegistryDadosMapaJogo(nomeChaves, val))
			_tprintf(_T("Não foi possivel guardar os valores no Registry!"));

	}
	else {
		_tprintf(_T("Número de argumentos inválido!"));
		exit(1);
	}

	dadosMapaJogo->nLinhas = val[0];
	dadosMapaJogo->nColunas = val[1];
	dadosMapaJogo->tempoAguaComecaFluir = val[2];

	srand((unsigned int)time(NULL));

	dadosMapaJogo->coordenadasOrigemAgua[0] = getRandomNumberBetweenMaxAndMin(0, dadosMapaJogo->nLinhas);
	dadosMapaJogo->coordenadasOrigemAgua[1] = 0;

	dadosMapaJogo->coordenadasDestinoAgua[0] = getRandomNumberBetweenMaxAndMin(0, dadosMapaJogo->nLinhas);
	dadosMapaJogo->coordenadasDestinoAgua[1] = dadosMapaJogo->nColunas-1;

	//Verifica se a origem e destino da àgua estão em quadrantes diagonais opostos
	if (dadosMapaJogo->coordenadasOrigemAgua[0] > (dadosMapaJogo->nLinhas / 2) && dadosMapaJogo->coordenadasDestinoAgua[0] > (dadosMapaJogo->nLinhas / 2))
		dadosMapaJogo->coordenadasDestinoAgua[0] -= dadosMapaJogo->nLinhas / 2;
		
	else if (dadosMapaJogo->coordenadasOrigemAgua[0] < (dadosMapaJogo->nLinhas / 2) && dadosMapaJogo->coordenadasDestinoAgua[0] < (dadosMapaJogo->nLinhas / 2))
		dadosMapaJogo->coordenadasDestinoAgua[0] += dadosMapaJogo->nLinhas / 2;
		
	for (int i = 0; i < dadosMapaJogo->nLinhas; i++) {
		for (int j = 0; j < dadosMapaJogo->nColunas; j++) {
			dadosMapaJogo->mapaJogo[i][j] = 0;
		}
	}
}

BOOL lerDoRegistryDadosMapaJogo(TCHAR nomeChaves[3][TAM], DWORD val[3]) {

	HKEY chave;
	LSTATUS returnCreate, returnQueryValue;
	TCHAR resposta[TAM];
	DWORD sizeVal;

	returnCreate = RegCreateKeyEx(
		HKEY_CURRENT_USER,
		caminhoRegistry,
		0,
		NULL,
		REG_OPTION_VOLATILE,
		KEY_ALL_ACCESS,
		NULL,
		&chave,
		&resposta
	);

	if (returnCreate != ERROR_SUCCESS) {
		DWORD error = GetLastError();
		_tprintf(_T("Erro a criar/abrir o registry! [%d]\n"), error);
		return FALSE;
	}

	for (DWORD i = 0; i < 3; i++)
	{
		returnQueryValue = RegQueryValueEx(
			chave,
			nomeChaves[i],
			NULL,
			NULL,
			(LPBYTE) &val[i],
			&sizeVal
		);

		if (returnQueryValue != ERROR_SUCCESS) {
			DWORD error = GetLastError();
			_tprintf(_T("Erro a ler do Registry [%d]\n"), error);
			RegCloseKey(chave);
			return FALSE;
		}
	}

	RegCloseKey(chave);
	return TRUE;
}
BOOL guardaNoRegistryDadosMapaJogo(TCHAR nomeChaves[3][TAM], DWORD val[3]) {

	HKEY chave;
	LSTATUS returnCreate, returnSetValue;
	TCHAR resposta[TAM];

	returnCreate = RegCreateKeyEx(
		HKEY_CURRENT_USER,
		caminhoRegistry,
		0,
		NULL,
		REG_OPTION_VOLATILE,
		KEY_ALL_ACCESS,
		NULL,
		&chave,
		&resposta
	);

	if (returnCreate != ERROR_SUCCESS) {
		DWORD error = GetLastError();
		_tprintf(_T("Erro a criar/abrir o registry! [%d]\n"), error);
		return FALSE;
	}

	for (DWORD i = 0; i < 3; i++)
	{
		returnSetValue = RegSetValueEx(
			chave,
			nomeChaves[i],
			0,
			REG_DWORD,
			(LPBYTE) &val[i],
			(DWORD) sizeof(DWORD)
		);

		if (returnSetValue != ERROR_SUCCESS) {
			DWORD error = GetLastError();
			_tprintf(_T("Erro a guardar o valor! [%d]\n"), error);
			RegCloseKey(chave);
			return FALSE;
		}
	}
	
	RegCloseKey(chave);
	return TRUE;
}

BOOL WINAPI atualizaMapaJogoParaMonitor(LPVOID p) {

	PartilhaMapaJogo* pcd = (PartilhaMapaJogo*)p;

	while (1) {


		WaitForSingleObject(pcd->hSemaforo, INFINITE);

		if (!pcd->threadMustContinue)
			break;

		SetEvent(pcd->newMsg);
		Sleep(500);
		ResetEvent(pcd->newMsg);

	}
	return TRUE;
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

int _tmain(int argc, TCHAR* argv[]) {

#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	_setmode(_fileno(stderr), _O_WTEXT);
#endif 

	DadosMapaJogo dadosMapaJogo;

	inicializaServidor(argc, argv, &dadosMapaJogo);

	_tprintf(_T("%d %d %d \n"), dadosMapaJogo.nLinhas, dadosMapaJogo.nColunas, dadosMapaJogo.tempoAguaComecaFluir);
	_tprintf(_T("%d %d \n"), dadosMapaJogo.coordenadasOrigemAgua[0], dadosMapaJogo.coordenadasOrigemAgua[1]);
	_tprintf(_T("%d %d \n"), dadosMapaJogo.coordenadasDestinoAgua[0], dadosMapaJogo.coordenadasDestinoAgua[1]);

	PartilhaMapaJogo dadosPartilhaMapaJogo;

	if (!initMemAndSync(&dadosPartilhaMapaJogo))
		return 1;

	HANDLE hThread = CreateThread(NULL, 0, atualizaMapaJogoParaMonitor, &dadosPartilhaMapaJogo, 0, NULL);

	for (int i = 0; i < dadosMapaJogo.nLinhas; i++) {
		for (int j = 0; j < dadosMapaJogo.nColunas; j++) {
			_tprintf(_T("%d "), dadosMapaJogo.mapaJogo[i][j]);
		}
		_tprintf(_T("\n"));
	}

	TCHAR qwe[TAM];

	while (1) {
		_getts_s(qwe, TAM);

		if (_tcscmp(qwe, _T("exit")) == 0) {
			dadosPartilhaMapaJogo.threadMustContinue = 0;
			ReleaseSemaphore(dadosPartilhaMapaJogo.hSemaforo, 1, NULL);
			break;
		}

		WaitForSingleObject(dadosPartilhaMapaJogo.hRWMutex, INFINITE);
		CopyMemory(dadosPartilhaMapaJogo.mapaJogo, &dadosMapaJogo, sizeof(DadosMapaJogo));
		ReleaseMutex(dadosPartilhaMapaJogo.hRWMutex);


		ReleaseSemaphore(dadosPartilhaMapaJogo.hSemaforo, 1, NULL);
	}

	WaitForSingleObject(hThread, INFINITE);

	UnmapViewOfFile(dadosPartilhaMapaJogo.mapaJogo);
	CloseHandle(dadosPartilhaMapaJogo.hMapFile);
	CloseHandle(dadosPartilhaMapaJogo.hRWMutex);
	CloseHandle(dadosPartilhaMapaJogo.newMsg);
	CloseHandle(dadosPartilhaMapaJogo.hSemaforo);
	CloseHandle(hThread);

	return 0;
}