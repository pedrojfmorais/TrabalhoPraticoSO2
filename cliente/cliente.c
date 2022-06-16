#include "..\\utils_so2_tp\utils_so2_tp.h"
#include "threads.h"
#include "inicializar.h"

int _tmain(int argc, TCHAR* argv[]) {

	HANDLE hPipe, hThread, hEvent;
	ThreadsCliente threadsCliente;
	PartilhaJogo partilhaJogo;
	int i, numClientes = 0;
	DWORD offset, nBytes;

#ifdef UNICODE
	int setmodeSTDIN = _setmode(_fileno(stdin), _O_WTEXT);
	int setmodeSTDOUT = _setmode(_fileno(stdout), _O_WTEXT);
	int setmodeSTDERR = _setmode(_fileno(stderr), _O_WTEXT);
#endif
	
	threadsCliente.deveContinuar = &partilhaJogo.deveContinuar;
	threadsCliente.hMutex = CreateMutex(NULL, FALSE, NULL);

	if (dados.hMutex == NULL) {
		_tprintf(_T("[ERRO] ao criar Mutex"));
		return -1;
	}

	for (i = 0; i < N_JOGADORES; i++) {
		hPipe = CreateNamedPipe(
			pipeName,
			PIPE_ACCESS_OUTBOUND | FILE_FLAG_OVERLAPPED,
			PIPE_WAIT | PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE,
			N_JOGADORES,
			256 * sizeof(TCHAR),
			256 * sizeof(TCHAR),
			1000,
			NULL
		);
		if (hPipe == INVALID_HANDLE_VALUE) {
			_tprintf(_T("A criação do pipe falhou"));
			return -1;
		}

		hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		if (hEvent == NULL) {
			_tprintf(_T("[ERRO] ao criar evento"));
			return -1;
		}

		threadsCliente.hPipes[i].hPipe = hPipe;
		threadsCliente.ativo[i].ativo = FALSE;

		//garantir a struct overlap estar limpa
		ZeroMemory(&dados.hPipes[i].overlap, sizeof(threadsCliente.hPipes[i].overlap));

		//Preencher o evento
		threadsCliente.hPipes[i].overlap.hEvent = hEvent;
		threadsCliente.hEvents[i] = hEvent;

		//connectar o name pipe, mandado um ponteiro para a estrutra overlap
		ConnectNamedPipe(hPipe, &threadsCliente.hPipes[i].overlap);
	}

	//Criar a threadd
	hThread = CreateThread(NULL, 0, funcao(), &threadsCliente, 0, NULL);
	if (hThread == NULL) {
		_tprintf(_T("[ERRO] ao criar a thread"));
		return -1;
	}

	while (!threadsCliente.deveContinuar) {

	}

	WaitForSingleObjetc(hThread, INFINITE);

	//Desligar todas as instancias de named pipes
	for (int i = 0; i < N_JOGADORES; i++) {
		if (!DisconnectNamedPipe(threadsCliente.hPipes[i].hPipe)) {
			_tprintf(_T("[ERRO] Desligar o pipe!"));
			return -1;
		}
	}



	return 0;

}