
#include "..\\utils_so2_tp\utils_so2_tp.h"
#include "threads.h"
#include "registry.h"
#include "comandosMonitor.h"

void inicializaJogo(DadosJogo* jogo, DefinicoesJogo definicoesJogo) {

	jogo->nLinhas = definicoesJogo.nLinhas;
	jogo->nColunas = definicoesJogo.nColunas;

	jogo->tempoAguaComecaFluir = definicoesJogo.tempoAguaComecaFluir;
	jogo->tempoDecorrido = 0;

	jogo->aJogar = TRUE;
	jogo->ganhou = FALSE;
	jogo->perdeu = FALSE;
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

	for (DWORD i = 0; i < jogo->nLinhas; i++) {
		for (DWORD j = 0; j < jogo->nColunas; j++) {
			if (i == jogo->coordenadasOrigemAgua[0] && j == jogo->coordenadasOrigemAgua[1])
				jogo->mapaJogo[i][j] = tuboOrigemAgua * 10;
			else if (i == jogo->coordenadasDestinoAgua[0] && j == jogo->coordenadasDestinoAgua[1])
				jogo->mapaJogo[i][j] = tuboDestinoAgua;
			else
				jogo->mapaJogo[i][j] = tuboVazio;
		}
	}

	jogo->coordenadaAtualAgua[0] = jogo->coordenadasOrigemAgua[0];
	jogo->coordenadaAtualAgua[1] = jogo->coordenadasOrigemAgua[1];

	if(definicoesJogo.modoAleatorioLigado)
		jogo->proximaPeca = getRandomProximaPeca();
	else
		jogo->proximaPeca = tuboHorizontal; // primeira peça
	/*
	//debug
	jogo->mapaJogo[0][0] = tuboOrigemAgua * 10;

	jogo->coordenadasOrigemAgua[0] = 0;
	jogo->coordenadasOrigemAgua[1] = 0;

	jogo->coordenadaAtualAgua[0] = 0;
	jogo->coordenadaAtualAgua[1] = 0;

	jogo->mapaJogo[0][1] = tuboHorizontal;
	jogo->mapaJogo[0][2] = tuboEsquerdaParaBaixo;
	jogo->mapaJogo[1][2] = tuboEsquerdaParaBaixo;
	jogo->mapaJogo[2][2] = tuboCurvaDireitaParaCima;
	jogo->mapaJogo[2][3] = tuboHorizontal;
	jogo->mapaJogo[2][4] = tuboDestinoAgua;
	*/
}

void inicializaServidor(int argc, TCHAR* argv[], PartilhaJogo* partilhaJogo) {

	TCHAR nomeChaves[3][TAM] = { _T("nLinhas"), _T("nColunas"), _T("tempoAguaComecaFluir") };
	DWORD definicoesJogo[3];

	if (argc == 1) {
		//Lê no registry
		if (!lerDoRegistryDadosMapaJogo(nomeChaves, definicoesJogo))
			exit(1);

	}else if(argc == 4) {
		//Guarda no registry
		for (DWORD i = 0; i < 3; i++)
			definicoesJogo[i] = (DWORD) _tcstod(argv[i+1], _T('\0'));

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

	partilhaJogo->definicoesJogo.nLinhas = definicoesJogo[0];
	partilhaJogo->definicoesJogo.nColunas = definicoesJogo[1];
	partilhaJogo->definicoesJogo.tempoAguaComecaFluir = definicoesJogo[2];
	partilhaJogo->definicoesJogo.modoAleatorioLigado = TRUE;
	partilhaJogo->definicoesJogo.tempoPararAgua = 0;

	for (DWORD i = 0; i < N_JOGADORES; i++)
	{
		partilhaJogo->jogos[i]->idJogador = i + 1;
	}

	//debug
	for (DWORD i = 0; i < N_JOGADORES; i++)
	{
		inicializaJogo(partilhaJogo->jogos[i], partilhaJogo->definicoesJogo);
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
		exit(1);
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
	int setmodeSTDIN = _setmode(_fileno(stdin), _O_WTEXT);
	int setmodeSTDOUT = _setmode(_fileno(stdout), _O_WTEXT);
	int setmodeSTDERR = _setmode(_fileno(stderr), _O_WTEXT);
#endif 

	srand((unsigned int)time(NULL));

	HINSTANCE hLibrary = verificacoesIniciais();

	PFUNC_TypeBool_PointerPartilhaMapaJogo initMemAndSync;
	initMemAndSync = (PFUNC_TypeBool_PointerPartilhaMapaJogo)GetProcAddress(hLibrary, "initMemAndSync");

	PartilhaJogo partilhaJogo;

	if (!initMemAndSync(&partilhaJogo))
		return 1;

	inicializaServidor(argc, argv, &partilhaJogo);

	//debug

	for (DWORD i = 0; i < N_JOGADORES; i++)
	{
		_tprintf(_T("%d %d %d \n"), partilhaJogo.jogos[i]->nLinhas, partilhaJogo.jogos[i]->nColunas, partilhaJogo.jogos[i]->tempoAguaComecaFluir);
		_tprintf(_T("%d %d \n"), partilhaJogo.jogos[i]->coordenadasOrigemAgua[0], partilhaJogo.jogos[i]->coordenadasOrigemAgua[1]);
		_tprintf(_T("%d %d \n"), partilhaJogo.jogos[i]->coordenadasDestinoAgua[0], partilhaJogo.jogos[i]->coordenadasDestinoAgua[1]);
	}

	HANDLE hThread = CreateThread(NULL, 0, atualizaMapaJogoParaMonitor, &partilhaJogo, 0, NULL);
	HANDLE hThreadDecorreJogo = CreateThread(NULL, 0, decorrerJogo, &partilhaJogo, 0, NULL);
	HANDLE hThreadBufferCircularMonitorParaServidor = CreateThread(NULL, 0, recebeMensagemMonitor, &partilhaJogo, 0, NULL);
	HANDLE hThreadBufferLeMensagemUtilizador = CreateThread(NULL, 0, leMensagemUtilizador, &partilhaJogo, 0, NULL);

	if (hThread == NULL || hThreadDecorreJogo == NULL || hThreadBufferCircularMonitorParaServidor == NULL || hThreadBufferLeMensagemUtilizador == NULL) {
		_tprintf(_T("Não foi possivel criar as threads necessárias para o funcionamento do servidor."));
		return 1;
	}

	WaitForSingleObject(hThread, INFINITE);

	for (DWORD i = 0; i < N_JOGADORES; i++) {
		CloseHandle(partilhaJogo.hMapFileJogos[i]);
		UnmapViewOfFile(partilhaJogo.jogos[i]);
	}
	CloseHandle(partilhaJogo.hMutexBufferCircularMonitorParaServidor);
	CloseHandle(partilhaJogo.hRWMutex);
	CloseHandle(partilhaJogo.hEvent);
	CloseHandle(partilhaJogo.hEventJogosDecorrer);

	UnmapViewOfFile(partilhaJogo.bufferCircularMonitorParaServidor);
	CloseHandle(partilhaJogo.hMapFileBufferCircularMonitorParaServidor);
	CloseHandle(partilhaJogo.hSemaforoEscritaBufferCircularMonitorParaServidor);
	CloseHandle(partilhaJogo.hSemaforoLeituraBufferCircularMonitorParaServidor);

	CloseHandle(partilhaJogo.hMapFileBufferCircularServidorParaMonitor);
	UnmapViewOfFile(partilhaJogo.bufferCircularServidorParaMonitor);
	CloseHandle(partilhaJogo.hMutexBufferCircularServidorParaMonitor);
	CloseHandle(partilhaJogo.hSemaforoEscritaBufferCircularServidorParaMonitor);
	CloseHandle(partilhaJogo.hSemaforoLeituraBufferCircularServidorParaMonitor);

	CloseHandle(hThread);
	CloseHandle(hThreadDecorreJogo);
	CloseHandle(hThreadBufferCircularMonitorParaServidor);
	CloseHandle(hThreadBufferLeMensagemUtilizador);
	CloseHandle(hLibrary);

	return 0;
}