
#include "..\\utils_so2_tp\utils_so2_tp.h"

// Retirado da internet "https://www.codewithc.com/change-text-color-in-codeblocks-console-window/"
void SetColor(int ForgC)
{
	WORD wColor;
	//This handle is needed to get the current background attribute

	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	//csbi is used for wAttributes word

	if (GetConsoleScreenBufferInfo(hStdOut, &csbi))
	{
		//To mask out all but the background attribute, and to add the color
		wColor = (csbi.wAttributes & 0xF0) + (ForgC & 0x0F);
		SetConsoleTextAttribute(hStdOut, wColor);
	}
	return;
}

void limpaEcra() {
	system("cls");
}

void desenharMapaJogo(DadosJogo* dadosJogo) {

	_tprintf(_T("\n\n\n"));

	SetColor(CorVerde);
	if (dadosJogo->ganhou) {
		_tprintf(_T("O jogador %d ganhou!\n"), dadosJogo->idJogador);
		SetColor(CorBranco);
		return;
	}

	SetColor(CorVermelho);
	if (dadosJogo->jogoPausado)
		_tprintf(_T("Jogo em pausa!\n"));

	SetColor(CorBranco);
	_tprintf(_T("Jogador %d\n"), dadosJogo->idJogador);
	_tprintf(_T("Pontuação: %d\n"), dadosJogo->pontuacao);
	_tprintf(_T("Tempo decorrido: %d segundos\n"), dadosJogo->tempoDecorrido);

	SetColor(CorBranco);
	for (int i = 0; i < dadosJogo->nLinhas * 3; i++)
	{
		if (i == 0) {
			_tprintf(_T("  "));

			SetColor(CorVerde);
			for (int j = 0; j < dadosJogo->nColunas * 3; j++)
				if(j%3 == 1)
					_tprintf(_T("%d"), (j/3)+1);
				else
					_tprintf(_T(" "));
			_tprintf(_T("\n "));

			SetColor(CorSalmao);
			for (int j = 0; j < (dadosJogo->nColunas * 3) + 2; j++)
				_tprintf(_T("-"));

			_tprintf(_T("\n"));
		}

		for (int j = 0; j < dadosJogo->nColunas * 3; j++)
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

			SetColor(CorAmareloClaro);
			switch (tipoTubo)
			{
			case tuboVazio:
				_tprintf(_T(" "));
				break;
			case tuboOrigemAgua:
				if (i % 3 == 0 || i % 3 == 2) 
					_tprintf(_T("█"));
				
				else if (i % 3 == 1) {
					if (j % 3 == 0) 
						_tprintf(_T("█"));
					
					else if (j % 3 == 1 || j % 3 == 2) {
						if (temAgua) {
							SetColor(CorAzulClaro);
							_tprintf(_T("█"));
							SetColor(CorAmareloClaro);
						}else
							_tprintf(_T(" "));
					}
				}
				break;
			case tuboDestinoAgua:
				if (i % 3 == 0 || i % 3 == 2) 
					_tprintf(_T("█"));
				
				else if (i % 3 == 1) {
					if (j % 3 == 0 || j % 3 == 1) {
						if (temAgua) {
							SetColor(CorAzulClaro);
							_tprintf(_T("█"));
							SetColor(CorAmareloClaro);
						}
						else
							_tprintf(_T(" "));
					}
					else if (j % 3 == 2) 
						_tprintf(_T("█"));
					
				}
						
				break;
			case tuboHorizontal:

				if (i % 3 == 0 || i % 3 == 2)
					_tprintf(_T("█"));

				else if (i % 3 == 1) {
					if (temAgua) {
						SetColor(CorAzulClaro);
						_tprintf(_T("█"));
						SetColor(CorAmareloClaro);
					}
					else
						_tprintf(_T(" "));
				}
				break;
			case tuboVertical:
				if (j % 3 == 0 || j % 3 == 2)
					_tprintf(_T("█"));

				else if (j % 3 == 1) {
					if (temAgua) {
						SetColor(CorAzulClaro);
						_tprintf(_T("█"));
						SetColor(CorAmareloClaro);
					}
					else
						_tprintf(_T(" "));
				}
				break;
			case tuboCurvaEsquerdaParaCima:
				if ((j % 3 == 0 && i % 3 != 1) || (i % 3 == 2 && j % 3 == 1) || (j % 3 == 2))
					_tprintf(_T("█"));

				else {
					if (temAgua) {
						SetColor(CorAzulClaro);
						_tprintf(_T("█"));
						SetColor(CorAmareloClaro);
					}
					else
						_tprintf(_T(" "));
				}

				break;
			case tuboEsquerdaParaBaixo:
				if ((j % 3 == 0 && i % 3 != 1) || (i % 3 == 0 && j % 3 == 1) || (j % 3 == 2))
					_tprintf(_T("█"));

				else {
					if (temAgua) {
						SetColor(CorAzulClaro);
						_tprintf(_T("█"));
						SetColor(CorAmareloClaro);
					}
					else
						_tprintf(_T(" "));
				}

				break;
			case tuboCurvaDireitaParaCima:
				if ((j % 3 == 2 && i % 3 != 1) || (i % 3 == 2 && j % 3 == 1) || (j % 3 == 0))
					_tprintf(_T("█"));

				else {
					if (temAgua) {
						SetColor(CorAzulClaro);
						_tprintf(_T("█"));
						SetColor(CorAmareloClaro);
					}
					else
						_tprintf(_T(" "));
				}
					
				break;
			case tuboDireitaParaBaixo:
				if ((j % 3 == 2 && i % 3 != 1) || (i % 3 == 0 && j % 3 == 1) || (j % 3 == 0))
					_tprintf(_T("█"));

				else {
					if (temAgua) {
						SetColor(CorAzulClaro);
						_tprintf(_T("█"));
						SetColor(CorAmareloClaro);
					}
					else
						_tprintf(_T(" "));
				}

				break;
			default:
				_tprintf(_T(" "));
			}
			
		}
		SetColor(CorSalmao);
		_tprintf(_T("|\n"));
		if (i == (dadosJogo->nLinhas * 3) - 1) {

			for (int i = 0; i < (dadosJogo->nColunas * 3) + 2; i++) {
				if (i == 0)
					_tprintf(_T(" "));

				_tprintf(_T("-"));
			}
			_tprintf(_T("\n"));
		}
		SetColor(CorBranco);
	}
}

BOOL WINAPI recebeMapaJogoDoServidor(LPVOID p) {

	PartilhaJogo* partilhaJogo = (PartilhaJogo*) p;
	DadosJogo msg;

	while (1) {
		WaitForSingleObject(partilhaJogo->hEvent, INFINITE);
		if (!partilhaJogo->threadMustContinue)
			return 0;

		WaitForSingleObject(partilhaJogo->hRWMutex, INFINITE);

		limpaEcra();

		if(partilhaJogo->jogador1->aJogar)
			desenharMapaJogo(partilhaJogo->jogador1);
		if (partilhaJogo->jogador2->aJogar)
			desenharMapaJogo(partilhaJogo->jogador2);

		ReleaseMutex(partilhaJogo->hRWMutex);

		Sleep(1000);
	}
	return 0;
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

	WaitForSingleObject(hThread, INFINITE);
	UnmapViewOfFile(dadosPartilhaJogo.jogador1);
	CloseHandle(dadosPartilhaJogo.hMapFileJogador1);
	UnmapViewOfFile(dadosPartilhaJogo.jogador2);
	CloseHandle(dadosPartilhaJogo.hMapFileJogador2);
	CloseHandle(dadosPartilhaJogo.hRWMutex);
	CloseHandle(dadosPartilhaJogo.hEvent);
	CloseHandle(dadosPartilhaJogo.hRWMutex);
	CloseHandle(dadosPartilhaJogo.hSemaforo);
	CloseHandle(hThread);
	CloseHandle(hLibrary);

	return 0;
}