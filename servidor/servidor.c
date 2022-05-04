
#include "..\\utils_so2_tp\utils_so2_tp.h"

DWORD getRandomNumberBetweenMaxAndMin(DWORD min, DWORD max) {
	return (rand() % (max - min)) + min;
}

DWORD getRandomProximaPeca(){
	return getRandomNumberBetweenMaxAndMin(1, 9); //maior peça + 1
}

void inicializaJogo(DadosJogo* jogo, DWORD* definicoesJogo) {

	jogo->nLinhas = definicoesJogo[0];
	jogo->nColunas = definicoesJogo[1];

	jogo->tempoAguaComecaFluir = definicoesJogo[2];
	jogo->tempoDecorrido = 0;

	jogo->aJogar = TRUE;
	jogo->ganhou = FALSE;
	jogo->jogoPausado = FALSE;

	jogo->pontuacao = 0;

	jogo->coordenadasOrigemAgua[0] = getRandomNumberBetweenMaxAndMin(0, jogo->nLinhas);
	jogo->coordenadasOrigemAgua[1] = 0;

	jogo->coordenadasDestinoAgua[0] = getRandomNumberBetweenMaxAndMin(0, jogo->nLinhas);
	jogo->coordenadasDestinoAgua[1] = jogo->nColunas - 1;

	//Verifica se a origem e destino da àgua estão em quadrantes diagonais opostos
	if (jogo->coordenadasOrigemAgua[0] > (jogo->nLinhas / 2) && jogo->coordenadasDestinoAgua[0] > (jogo->nLinhas / 2))
		jogo->coordenadasDestinoAgua[0] -= jogo->nLinhas / 2;

	else if (jogo->coordenadasOrigemAgua[0] < (jogo->nLinhas / 2) && jogo->coordenadasDestinoAgua[0] < (jogo->nLinhas / 2))
		jogo->coordenadasDestinoAgua[0] += jogo->nLinhas / 2;

	for (int i = 0; i < jogo->nLinhas; i++) {
		for (int j = 0; j < jogo->nColunas; j++) {
			if (i == jogo->coordenadasOrigemAgua[0] && j == jogo->coordenadasOrigemAgua[1])
				jogo->mapaJogo[i][j] = tuboOrigemAgua;
			else if (i == jogo->coordenadasDestinoAgua[0] && j == jogo->coordenadasDestinoAgua[1])
				jogo->mapaJogo[i][j] = tuboDestinoAgua;
			else
				jogo->mapaJogo[i][j] = tuboVazio;
		}
	}

	jogo->coordenadaAtualAgua[0] = jogo->coordenadasOrigemAgua[0];
	jogo->coordenadaAtualAgua[1] = jogo->coordenadasOrigemAgua[1];

	jogo->proximaPeca = getRandomProximaPeca();

	//debug
	jogo->mapaJogo[0][3] = tuboHorizontal*10;
	jogo->mapaJogo[2][3] = tuboVertical;
	jogo->mapaJogo[4][3] = tuboCurvaDireitaParaCima;
	jogo->mapaJogo[0][1] = tuboCurvaEsquerdaParaCima;
	jogo->mapaJogo[2][1] = tuboDireitaParaBaixo;
	jogo->mapaJogo[4][1] = tuboEsquerdaParaBaixo;
}

void inicializaServidor(int argc, TCHAR* argv[], PartilhaJogo* partilhaJogo, DWORD* definicoesJogo) {

	TCHAR nomeChaves[3][TAM] = { _T("nLinhas"), _T("nColunas"), _T("tempoAguaComecaFluir") };

	if (argc == 1) {
		//Lê no registry
		if (!lerDoRegistryDadosMapaJogo(nomeChaves, definicoesJogo))
			exit(1);

	}else if(argc == 4) {
		//Guarda no registry
		for (DWORD i = 0; i < 3; i++)
			definicoesJogo[i] = wcstod(argv[i+1], _T('\0'));

		for (DWORD i = 0; i < 2; i++)
			if (definicoesJogo[i] > 20)
				definicoesJogo[i] = 20;
			else if (definicoesJogo[i] < 5)
				definicoesJogo[i] = 5;

		if(!guardaNoRegistryDadosMapaJogo(nomeChaves, definicoesJogo))
			_tprintf(_T("Não foi possivel guardar os valores no Registry!"));

	}
	else {
		_tprintf(_T("Número de argumentos inválido!"));
		exit(1);
	}

	inicializaJogo(partilhaJogo->jogador1, definicoesJogo);
	partilhaJogo->jogador1->idJogador = 1;
	inicializaJogo(partilhaJogo->jogador2, definicoesJogo);
	partilhaJogo->jogador2->idJogador = 2;

	//debug
	partilhaJogo->jogador1->jogoPausado = TRUE;

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

	PartilhaJogo* partilhaJogo = (PartilhaJogo*)p;

	while (1) {


		WaitForSingleObject(partilhaJogo->hSemaforo, INFINITE);

		if (!partilhaJogo->threadMustContinue)
			break;

		SetEvent(partilhaJogo->hEvent);
		Sleep(500);
		ResetEvent(partilhaJogo->hEvent);

	}
	return TRUE;
}

//testar
BOOL pathfindAguaParaCima(DadosJogo* dadosJogo) {

	if (dadosJogo->coordenadaAtualAgua[0] == 0)
		return FALSE;

	dadosJogo->mapaJogo[dadosJogo->coordenadaAtualAgua[0]][dadosJogo->coordenadaAtualAgua[1]] *= 10;
	dadosJogo->coordenadaAtualAgua[0] -= 1;
	return TRUE;
}
//testar
BOOL pathfindAguaParaBaixo(DadosJogo* dadosJogo) {

	if (dadosJogo->coordenadaAtualAgua[0] == dadosJogo->nLinhas-1)
		return FALSE;

	dadosJogo->mapaJogo[dadosJogo->coordenadaAtualAgua[0]][dadosJogo->coordenadaAtualAgua[1]] *= 10;
	dadosJogo->coordenadaAtualAgua[0] += 1;
	return TRUE;
}
//testar
BOOL pathfindAguaParaEsquerda(DadosJogo* dadosJogo) {

	if (dadosJogo->coordenadaAtualAgua[1] == 0)
		return FALSE;

	dadosJogo->mapaJogo[dadosJogo->coordenadaAtualAgua[0]][dadosJogo->coordenadaAtualAgua[1]] *= 10;
	dadosJogo->coordenadaAtualAgua[1] -= 1;
	return TRUE;
}
//testar
BOOL pathfindAguaParaDireita(DadosJogo* dadosJogo) {

	if (dadosJogo->coordenadaAtualAgua[1] == dadosJogo->nColunas - 1)
		return FALSE;

	dadosJogo->mapaJogo[dadosJogo->coordenadaAtualAgua[0]][dadosJogo->coordenadaAtualAgua[1]] *= 10;
	dadosJogo->coordenadaAtualAgua[1] += 1;
	return TRUE;
}

BOOL pathfindAguaJogo(DadosJogo* dadosJogo) {

	//coordenada atual da água no mapa
	switch (dadosJogo->mapaJogo[dadosJogo->coordenadaAtualAgua[0]][dadosJogo->coordenadaAtualAgua[1]])
	{
	case tuboCurvaEsquerdaParaCima:
		
		if (dadosJogo->coordenadaAtualAgua[0] != 0 && 
			dadosJogo->mapaJogo[dadosJogo->coordenadaAtualAgua[0]][dadosJogo->coordenadaAtualAgua[1] - 1] % 10 > 0)

			return pathfindAguaParaCima(dadosJogo);

		else if (dadosJogo->coordenadaAtualAgua[1] != 0 && 
			dadosJogo->mapaJogo[dadosJogo->coordenadaAtualAgua[0] - 1][dadosJogo->coordenadaAtualAgua[1]] % 10 > 0)
			
			return pathfindAguaParaEsquerda(dadosJogo);
		
		break;
	case tuboCurvaDireitaParaCima:

		
		break;
	case tuboEsquerdaParaBaixo:
	case tuboDireitaParaBaixo:
		break;
	case tuboHorizontal:
		break;
	case tuboVertical:
		break;
	case tuboOrigemAgua:
		break;
	case tuboDestinoAgua:
		break;
	default:
		break;
	}

}

BOOL WINAPI decorrerJogo(LPVOID p) {
	PartilhaJogo* partilhaJogo = (PartilhaJogo*)p;
	
	while (1) {

		BOOL sofreuAlteracoes = FALSE;

		for (DWORD i = 0; i < N_JOGADORES; i++)
		{
			DadosJogo* dadosJogo;

			if (i == 0)
				dadosJogo == partilhaJogo->jogador1;
			else if (i == 1)
				dadosJogo == partilhaJogo->jogador2;

			if (!dadosJogo->aJogar || dadosJogo->jogoPausado)
				continue;

			if (dadosJogo->tempoAguaComecaFluir < dadosJogo->tempoDecorrido) {
				//água avança a cada 2 segundos
				if ((dadosJogo->tempoDecorrido - dadosJogo->tempoAguaComecaFluir) % 2 == 0) {
					//pathfind
				}
			}

			dadosJogo->tempoDecorrido += 1;
			sofreuAlteracoes = TRUE;
		}
		if(sofreuAlteracoes)
			ReleaseSemaphore(partilhaJogo->hSemaforo, 1, NULL);
		Sleep(1000);
	}
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

	srand((unsigned int)time(NULL));

	HINSTANCE hLibrary = verificacoesIniciais();

	PFUNC_TypeBool_PointerPartilhaMapaJogo initMemAndSync;
	initMemAndSync = (PFUNC_TypeBool_PointerPartilhaMapaJogo)GetProcAddress(hLibrary, "initMemAndSync");

	PartilhaJogo partilhaJogo;

	if (!initMemAndSync(&partilhaJogo))
		return 1;

	DWORD definicoesJogo[3];
	inicializaServidor(argc, argv, &partilhaJogo, definicoesJogo);

	//debug
	_tprintf(_T("%d %d %d \n"), partilhaJogo.jogador1->nLinhas, partilhaJogo.jogador1->nColunas, partilhaJogo.jogador1->tempoAguaComecaFluir);
	_tprintf(_T("%d %d \n"), partilhaJogo.jogador1->coordenadasOrigemAgua[0], partilhaJogo.jogador1->coordenadasOrigemAgua[1]);
	_tprintf(_T("%d %d \n"), partilhaJogo.jogador1->coordenadasDestinoAgua[0], partilhaJogo.jogador1->coordenadasDestinoAgua[1]);
	
	_tprintf(_T("%d %d %d \n"), partilhaJogo.jogador2->nLinhas, partilhaJogo.jogador2->nColunas, partilhaJogo.jogador2->tempoAguaComecaFluir);
	_tprintf(_T("%d %d \n"), partilhaJogo.jogador2->coordenadasOrigemAgua[0], partilhaJogo.jogador2->coordenadasOrigemAgua[1]);
	_tprintf(_T("%d %d \n"), partilhaJogo.jogador2->coordenadasDestinoAgua[0], partilhaJogo.jogador2->coordenadasDestinoAgua[1]);

	HANDLE hThread = CreateThread(NULL, 0, atualizaMapaJogoParaMonitor, &partilhaJogo, 0, NULL);

	TCHAR comandoInserido[TAM];

	while (1) {
		_getts_s(comandoInserido, TAM);

		if (_tcscmp(comandoInserido, _T("exit")) == 0) {
			partilhaJogo.threadMustContinue = 0;
			ReleaseSemaphore(partilhaJogo.hSemaforo, 1, NULL);
			break;
		}

		ReleaseSemaphore(partilhaJogo.hSemaforo, 1, NULL);
	}

	WaitForSingleObject(hThread, INFINITE);

	UnmapViewOfFile(partilhaJogo.jogador1);
	CloseHandle(partilhaJogo.hMapFileJogador1);
	UnmapViewOfFile(partilhaJogo.jogador2);
	CloseHandle(partilhaJogo.hMapFileJogador2);
	CloseHandle(partilhaJogo.hRWMutex);
	CloseHandle(partilhaJogo.hEvent);
	CloseHandle(partilhaJogo.hSemaforo);
	CloseHandle(hThread);
	CloseHandle(hLibrary);

	return 0;
}