
#include "..\\utils_so2_tp\utils_so2_tp.h"
#include "threads.h"
#include "printPecas.h"

void limpaEcra() {system("cls");}

void desenharMapaJogo(DadosJogo* dadosJogo) {

	_tprintf(_T("\n\n\n"));

	SetColor(CorBranco);
	for (DWORD i = 0; i < dadosJogo->nLinhas * 3; i++)
	{
		if (i == 0) {
			_tprintf(_T("  "));

			SetColor(CorVerde);
			for (DWORD j = 0; j < dadosJogo->nColunas * 3; j++)
				if(j%3 == 1)
					_tprintf(_T("%d"), (j/3)+1);
				else
					_tprintf(_T(" "));
			_tprintf(_T("\n "));

			SetColor(CorSalmao);
			for (DWORD j = 0; j < (dadosJogo->nColunas * 3) + 2; j++)
				_tprintf(_T("-"));

			_tprintf(_T("\n"));
		}

		for (DWORD j = 0; j < dadosJogo->nColunas * 3; j++)
		{
			if (j == 0) {
				SetColor(CorVerde);
				if (i % 3 == 1)
					_tprintf(_T("%d"), (i / 3) + 1);
				else
					_tprintf(_T(" "));

				SetColor(CorSalmao);
				_tprintf(_T("|"));
			}

			SetColor(CorBranco);

			int tipoTubo = dadosJogo->mapaJogo[i / 3][j / 3];
			BOOL temAgua = FALSE;

			if (tipoTubo / 10 > 0) {
				tipoTubo /= 10;
				temAgua = TRUE;
			}

			printPeca(tipoTubo, i, j, temAgua);
		}
		SetColor(CorSalmao);
		_tprintf(_T("|"));

		switch (i)
		{
		case 0: 
			_tprintf(_T("\t"));
			if (dadosJogo->perdeu) {

				SetColor(CorVermelho);
				_tprintf(_T("O jogador %d perdeu!"), dadosJogo->idJogador);

			}else if (dadosJogo->ganhou) {

				SetColor(CorVerde);
				_tprintf(_T("O jogador %d ganhou!"), dadosJogo->idJogador);

			}else if (dadosJogo->jogoPausado){
				SetColor(CorAmareloClaro);
				_tprintf(_T("Jogo em pausa!"));
			}
			break;
		case 1:
			_tprintf(_T("\t"));
			SetColor(CorBranco);
			_tprintf(_T("Jogador %d"), dadosJogo->idJogador);
			break;
		case 2:
			_tprintf(_T("\t"));
			SetColor(CorBranco);
			_tprintf(_T("Pontuação: %d"), dadosJogo->pontuacao);
			break;
		case 3:
			_tprintf(_T("\t"));
			SetColor(CorBranco);
			_tprintf(_T("Tempo decorrido: %d segundos"), dadosJogo->tempoDecorrido);
			break;
		case 5:
			SetColor(CorBranco);
			_tprintf(_T("\tPróxima peça:"));
			break;
		default:
			break;
		}

		if (i == 6 || i == 7 || i == 8) {
			_tprintf(_T("\t"));
			for (int j = 0; j < 3; j++)
			{
				printPeca(dadosJogo->proximaPeca, i, j, FALSE);
			}
		}

		_tprintf(_T("\n"));
		SetColor(CorSalmao);

		if (i == (dadosJogo->nLinhas * 3) - 1) {

			for (DWORD i = 0; i < (dadosJogo->nColunas * 3) + 2; i++) {
				if (i == 0)
					_tprintf(_T(" "));

				_tprintf(_T("-"));
			}
			_tprintf(_T("\n"));
		}
		SetColor(CorBranco);
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

	if (!verificaServidorJaEstaCorrer()) {
		_tprintf(_T("O servidor ainda não está a correr!\n"));
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

	PartilhaJogo dadosPartilhaJogo;

	if (!initMemAndSync(&dadosPartilhaJogo))
		return 0;

	HANDLE hThread = CreateThread(NULL, 0, recebeMapaJogoDoServidor, &dadosPartilhaJogo, 0, NULL);
	//verificação thread

	WaitForSingleObject(hThread, INFINITE);
	UnmapViewOfFile(dadosPartilhaJogo.jogador1);
	CloseHandle(dadosPartilhaJogo.hMapFileJogador1);
	UnmapViewOfFile(dadosPartilhaJogo.jogador2);
	CloseHandle(dadosPartilhaJogo.hMapFileJogador2);
	CloseHandle(dadosPartilhaJogo.hRWMutex);
	CloseHandle(dadosPartilhaJogo.hEvent);
	CloseHandle(dadosPartilhaJogo.hSemaforo);
	CloseHandle(dadosPartilhaJogo.hEventJogosDecorrer);
	CloseHandle(hThread);
	CloseHandle(hLibrary);

	return 0;
}