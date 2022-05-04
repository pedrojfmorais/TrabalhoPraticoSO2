
#include "..\\utils_so2_tp\utils_so2_tp.h"

typedef BOOL(*PFUNC_TypeBool_NoArguments) ();
typedef BOOL(*PFUNC_TypeBool_PointerPartilhaMapaJogo) (PartilhaMapaJogo*);

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

void desenharMapaJogo(DadosMapaJogo* dadosMapaJogo) {

	_tprintf(_T("\n\n\n"));
	for (int i = 0; i < dadosMapaJogo->nLinhas * 3; i++)
	{
		if (i == 0) {
			_tprintf(_T("  "));

			for (int j = 0; j < dadosMapaJogo->nColunas * 3; j++)
				if(j%3 == 1)
					_tprintf(_T("%d"), (j/3)+1);
				else
					_tprintf(_T(" "));
			_tprintf(_T("\n "));

			for (int j = 0; j < (dadosMapaJogo->nColunas * 3) + 2; j++)
				_tprintf(_T("-"));
			_tprintf(_T("\n"));
		}

		for (int j = 0; j < dadosMapaJogo->nColunas * 3; j++)
		{
			if (j == 0) {
				if (i % 3 == 1)
					_tprintf(_T("%d"), (i / 3) + 1);
				else
					_tprintf(_T(" "));
				_tprintf(_T("|"));
			}

			SetColor(CorCinzento);

			int tipoTubo = dadosMapaJogo->mapaJogo[i / 3][j / 3];
			BOOL temAgua = FALSE;

			if (tipoTubo / 10 > 0) {
				tipoTubo /= 10;
				temAgua = TRUE;
			}

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
							SetColor(CorCinzento);
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
							SetColor(CorCinzento);
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
						SetColor(CorCinzento);
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
						SetColor(CorCinzento);
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
						SetColor(CorCinzento);
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
						SetColor(CorCinzento);
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
						SetColor(CorCinzento);
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
						SetColor(CorCinzento);
					}
					else
						_tprintf(_T(" "));
				}

				break;
			default:
				_tprintf(_T(" "));
			}
			
		}
		SetColor(CorBranco);
		_tprintf(_T("|\n"));
		if (i == (dadosMapaJogo->nLinhas * 3) - 1) {
			for (int i = 0; i < (dadosMapaJogo->nColunas * 3) + 2; i++) {
				if (i == 0)
					_tprintf(_T(" "));
				_tprintf(_T("-"));
			}
			_tprintf(_T("\n"));
		}
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
	CloseHandle(hLibrary);

	return 0;
}