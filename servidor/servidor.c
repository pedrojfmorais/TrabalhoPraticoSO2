
#include "..\\utils_so2_tp\utils_so2_tp.h"
#include "threads.h"
#include "inicializar.h"

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

	PartilhaJogoServidorMonitor partilhaJogoMonitor;
	PartilhaJogoServidorCliente partilhaJogoClientes;
	MensagensServidorCliente mensagensServidorCliente;

	if (!initMemAndSync(&partilhaJogoMonitor))
		return 1;

	inicializaServidor(argc, argv, &partilhaJogoMonitor, &partilhaJogoClientes);

	ThreadsServidor threadsServidor;
	threadsServidor.deveContinuar = &partilhaJogoMonitor.deveContinuar;
	threadsServidor.hEventFecharTudo = partilhaJogoMonitor.hEventFecharTudo;
	
	threadsServidor.hThreads[0] = CreateThread(NULL, 0, atualizaMapaJogoParaMonitor, &partilhaJogoMonitor, 0, NULL);
	threadsServidor.hThreads[1] = CreateThread(NULL, 0, decorrerJogo, &partilhaJogoMonitor, 0, NULL);
	threadsServidor.hThreads[2] = CreateThread(NULL, 0, recebeMensagemMonitor, &partilhaJogoMonitor, 0, NULL);
	threadsServidor.hThreads[3] = CreateThread(NULL, 0, leMensagemUtilizador, &partilhaJogoMonitor, 0, NULL);
	HANDLE hThreadParaThreads = CreateThread(NULL, 0, acabarThreads, &threadsServidor, 0, NULL);

	if (threadsServidor.hThreads[0] == NULL || threadsServidor.hThreads[1] == NULL || threadsServidor.hThreads[2] == NULL || 
		threadsServidor.hThreads[3] == NULL || hThreadParaThreads == NULL) {
		_tprintf(_T("Não foi possivel criar as threads necessárias para o funcionamento do servidor."));
		return 1;
	}

	// Testes

	HANDLE hThreadNamedPipeTabuleiro = CreateThread(NULL, 0, criaNamedPipeParaClientesTabuleiroJogo, &partilhaJogoClientes, 0, NULL);
	HANDLE hThreadNamedPipeMensagems = CreateThread(NULL, 0, criaNamedPipeParaClientesMensagens, &mensagensServidorCliente, 0, NULL);
	HANDLE hThreadNamedPipe_ = CreateThread(NULL, 0, clienteConectaNamedPipe, &partilhaJogoClientes, 0, NULL);
	
	/*
	DWORD numClientes = 0, offset, nBytes;
	for (int i = 0; i < N_JOGADORES; ++i) {

		HANDLE hPipe;
		HANDLE hEventTemp;

		_tprintf(TEXT("[ESCRITOR] Criar uma cópia do pipe '%s' ...(CreateNamedPipe)\n"), PIPE_NAME);
		hPipe = CreateNamedPipe(
			PIPE_NAME,
			PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
			PIPE_WAIT | PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE
			, N_JOGADORES,
			sizeof(PartilhaJogoServidorCliente),
			sizeof(PartilhaJogoServidorCliente),
			NMPWAIT_USE_DEFAULT_WAIT,
			NULL
		);
		if (hPipe == INVALID_HANDLE_VALUE) {
			_tprintf(TEXT("[ERRO] Criar Named Pipe! (CreateNamedPipe)"));
			exit(-1);
		}

		hEventTemp = CreateEvent(NULL, TRUE, FALSE, NULL);
		if (hEventTemp == NULL) {
			_tprintf(TEXT("[ERRO] Criar Event! (CreateEvent)"));
			exit(-1);
		}

		ZeroMemory(&partilhaJogoClientes.hPipe[i].overlap, sizeof(&partilhaJogoClientes.hPipe[i].overlap));
		partilhaJogoClientes.hPipe[i].hInstancia = hPipe;
		partilhaJogoClientes.hPipe[i].overlap.hEvent = hEventTemp;
		partilhaJogoClientes.hPipe[i].ativo = FALSE;
		partilhaJogoClientes.hEvents[i] = hEventTemp;

		if (ConnectNamedPipe(hPipe, &partilhaJogoClientes.hPipe[i].overlap)) {
			_tprintf(TEXT("[ERRO] Ligação ao leitor! (ConnectNamedPipe)\n"));
			exit(-1);
		}
	}
	
	while (partilhaJogoClientes.deveContinuar && numClientes < N_JOGADORES) {
		_tprintf(_T("asd"));
		offset = WaitForMultipleObjects(N_JOGADORES, partilhaJogoClientes.hEvents, FALSE, INFINITE);
		DWORD i = offset - WAIT_OBJECT_0;

		if (i >= 0 && i < N_JOGADORES) {
			if (GetOverlappedResult(partilhaJogoClientes.hPipe[i].hInstancia, &partilhaJogoClientes.hPipe[i].overlap, &nBytes, FALSE)) {
				ResetEvent(partilhaJogoClientes.hEvents[i]);
				WaitForSingleObject(partilhaJogoClientes.hReadWriteMutexAtualizacaoNoJogo, INFINITE);
				partilhaJogoClientes.hPipe[i].ativo = TRUE;
				ReleaseMutex(partilhaJogoClientes.hReadWriteMutexAtualizacaoNoJogo);
			}
			numClientes++;
		}
	}
	*/

	// Fim testes
	WaitForMultipleObjects(N_THREADS_SERVIDOR, threadsServidor.hThreads, TRUE, INFINITE);
	WaitForSingleObject(hThreadParaThreads, INFINITE);

	for (DWORD i = 0; i < N_JOGADORES; i++) {
		CloseHandle(partilhaJogoMonitor.hMapFileJogos[i]);
		UnmapViewOfFile(partilhaJogoMonitor.jogos[i]);
	}
	CloseHandle(partilhaJogoMonitor.hReadWriteMutexAtualizacaoNoJogo);
	CloseHandle(partilhaJogoMonitor.hEventAtualizacaoNoJogo);
	CloseHandle(partilhaJogoMonitor.hSemaforoEnviarAtualizacoesJogo);
	CloseHandle(partilhaJogoMonitor.hEventJogosDecorrer);
	CloseHandle(partilhaJogoMonitor.hEventFecharTudo);

	UnmapViewOfFile(partilhaJogoMonitor.bufferCircularMonitorParaServidor);
	CloseHandle(partilhaJogoMonitor.hMutexBufferCircularMonitorParaServidor);
	CloseHandle(partilhaJogoMonitor.hMapFileBufferCircularMonitorParaServidor);
	CloseHandle(partilhaJogoMonitor.hSemaforoEscritaBufferCircularMonitorParaServidor);
	CloseHandle(partilhaJogoMonitor.hSemaforoLeituraBufferCircularMonitorParaServidor);

	for (DWORD i = 0; i < N_THREADS_SERVIDOR; i++)
		CloseHandle(threadsServidor.hThreads[i]);

	CloseHandle(hThreadParaThreads);
	CloseHandle(hLibrary);

	return 0;
}