#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>

#define MUTEX_VERIFY_SERVER_OPEN _T("MutexServidorExiste")

//memória partilhada para o monitor
#define SHM_NAME _T("memoriaPartilhaMapaJogo")
#define SEMAPHORE_NAME _T("SEMAFORO_PartilhaMapaJogo")
#define MUTEX_NAME_PARTILHA_MAPA_JOGO _T("MutexAtualizarMapaJogo")
#define EVENT_NAME_PARTILHA_MAPA_JOGO _T("NovaAtualizacaoMapaJogo")
#define MSGTEXT_SZ 1000

#define MSGBUFSIZE sizeof(DadosMapaJogo)

// Um dos tubos multiplicado por 10 significa que tem água, exceto tuboVazio
#define tuboVazio 0
#define tuboOrigemAgua 1
#define tuboDestinoAgua 2
#define tuboHorizontal 3
#define tuboVertical 4
#define tuboCurvaEsquerdaParaCima 5
#define tuboEsquerdaParaBaixo 6
#define tuboCurvaDireitaParaCima 7
#define tuboDireitaParaBaixo 8

#define CorBranco 7
#define CorCinzento 14
#define CorAzulClaro 11

typedef struct {
	DWORD nLinhas;
	DWORD nColunas;
	DWORD tempoAguaComecaFluir;
	DWORD coordenadasOrigemAgua[2];
	DWORD coordenadasDestinoAgua[2];
	int mapaJogo[20][20];
} DadosMapaJogo;

typedef struct {
	HANDLE hMapFile;
	DadosMapaJogo* mapaJogo;
	int threadMustContinue;
	HANDLE newMsg;
	HANDLE hRWMutex;
	HANDLE hSemaforo;
} PartilhaMapaJogo;

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

// fica
typedef BOOL(*PFUNC_TypeBool_NoArguments) ();

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
}

void verificacoesIniciais() {
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
}

int _tmain(int argc, TCHAR* argv[]) {

#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	_setmode(_fileno(stderr), _O_WTEXT);
#endif 

	verificacoesIniciais();

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