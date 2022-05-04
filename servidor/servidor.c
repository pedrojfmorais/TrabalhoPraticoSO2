
#include "..\\utils_so2_tp\utils_so2_tp.h"

typedef BOOL(*PFUNC_TypeBool_NoArguments) ();
typedef BOOL(*PFUNC_TypeBool_PointerPartilhaMapaJogo) (PartilhaMapaJogo*);

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
			if (i == dadosMapaJogo->coordenadasOrigemAgua[0] && j == dadosMapaJogo->coordenadasOrigemAgua[1])
				dadosMapaJogo->mapaJogo[i][j] = tuboOrigemAgua;
			else if (i == dadosMapaJogo->coordenadasDestinoAgua[0] && j == dadosMapaJogo->coordenadasDestinoAgua[1])
				dadosMapaJogo->mapaJogo[i][j] = tuboDestinoAgua;
			else
				dadosMapaJogo->mapaJogo[i][j] = tuboVazio;
		}
	}

	//debug
	dadosMapaJogo->mapaJogo[0][3] = tuboHorizontal;
	dadosMapaJogo->mapaJogo[2][3] = tuboVertical;
	dadosMapaJogo->mapaJogo[4][3] = tuboCurvaDireitaParaCima;
	dadosMapaJogo->mapaJogo[0][1] = tuboCurvaEsquerdaParaCima;
	dadosMapaJogo->mapaJogo[2][1] = tuboDireitaParaBaixo;
	dadosMapaJogo->mapaJogo[4][1] = tuboEsquerdaParaBaixo;
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

HINSTANCE verificacoesIniciais() {
	TCHAR caminhoCompletoDLL[MAX_PATH];
	GetFullPathName(_T("utils_so2_tp.dll"),
		MAX_PATH,
		caminhoCompletoDLL,
		NULL);
	HINSTANCE hLibrary;
	hLibrary = LoadLibrary(caminhoCompletoDLL);

	if (hLibrary == NULL) {
		_tprintf(_T("A DLL não abriu!\n"));
		return -1;
	}

	PFUNC_TypeBool_NoArguments verificaServidorJaEstaCorrer;
	verificaServidorJaEstaCorrer = (PFUNC_TypeBool_NoArguments)GetProcAddress(hLibrary, "verificaServidorJaEstaCorrer");

	if (verificaServidorJaEstaCorrer() == TRUE) {
		_tprintf(_T("O servidor já está a correr!\n"));
		exit(1);
	}
	return hLibrary;
}

int _tmain(int argc, TCHAR* argv[]) {

#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	_setmode(_fileno(stderr), _O_WTEXT);
#endif 

	HINSTANCE hLibrary = verificacoesIniciais();

	PFUNC_TypeBool_PointerPartilhaMapaJogo initMemAndSync;
	initMemAndSync = (PFUNC_TypeBool_PointerPartilhaMapaJogo)GetProcAddress(hLibrary, "initMemAndSync");

	PartilhaMapaJogo dadosPartilhaMapaJogo;

	if (!initMemAndSync(&dadosPartilhaMapaJogo))
		return 1;

	DadosMapaJogo dadosMapaJogo;

	inicializaServidor(argc, argv, &dadosMapaJogo);

	//debig
	_tprintf(_T("%d %d %d \n"), dadosMapaJogo.nLinhas, dadosMapaJogo.nColunas, dadosMapaJogo.tempoAguaComecaFluir);
	_tprintf(_T("%d %d \n"), dadosMapaJogo.coordenadasOrigemAgua[0], dadosMapaJogo.coordenadasOrigemAgua[1]);
	_tprintf(_T("%d %d \n"), dadosMapaJogo.coordenadasDestinoAgua[0], dadosMapaJogo.coordenadasDestinoAgua[1]);

	HANDLE hThread = CreateThread(NULL, 0, atualizaMapaJogoParaMonitor, &dadosPartilhaMapaJogo, 0, NULL);

	TCHAR comandoInserido[TAM];

	while (1) {
		_getts_s(comandoInserido, TAM);

		if (_tcscmp(comandoInserido, _T("exit")) == 0) {
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
	CloseHandle(hLibrary);

	return 0;
}