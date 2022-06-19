#include "..\\utils_so2_tp\utils_so2_tp.h"

int _tmain(int argc, TCHAR* argv[]) {

	HANDLE hPipeMensagens;
	BufferCell mensagensServidorCliente;
	int numClientes = 0;
	DWORD n;

#ifdef UNICODE
	int setmodeSTDIN = _setmode(_fileno(stdin), _O_WTEXT);
	int setmodeSTDOUT = _setmode(_fileno(stdout), _O_WTEXT);
	int setmodeSTDERR = _setmode(_fileno(stderr), _O_WTEXT);
#endif

	if (!WaitNamedPipe(PIPE_NAME_MENSAGENS, NMPWAIT_WAIT_FOREVER)) {
		exit(-1);
	}

	hPipeMensagens = CreateFile(
		PIPE_NAME_MENSAGENS,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (hPipeMensagens == NULL) {
		exit(-1);
	}

	while (1) {

		//WaitForSingleObject(dadosMensagens->hMutex, INFINITE);

		_tprintf(TEXT("[ESCRITOR] Frase: "));
		_getts_s(mensagensServidorCliente.mensagem, TAM);


		//CopyMemory(dadosMensagens->mensagens[i].mensagem, buf, _tcslen(buf) * sizeof(TCHAR));
		//dadosMensagens->mensagens[i].mensagem[_tcslen(dadosMensagens->mensagens[i].mensagem) - 1] = '\0';

		_tprintf(_T("-%s-"), mensagensServidorCliente.mensagem);

		if (!WriteFile(hPipeMensagens, &mensagensServidorCliente, sizeof(BufferCell), &n, NULL)) {
			exit(-1);
		}

		_tprintf(_T("Bem %d\n"), n);
		//ReleaseMutex(dadosMensagens->hMutex);

		/*
		ret = ReadFile(hPipeMensagens, &mensagensServidorCliente, sizeof(BufferCell), &n, NULL);

		_tprintf(_T("%d %d "), ret, n);
		if (!ret || !n) {
			_tprintf(_T("\nERRO"));
			break;
		}

		_tprintf(_T("AQUI %s\n\n"), mensagensServidorCliente.mensagem);
		*/
	}

	//CloseHandle(hPipeMensagens);

	return 0;

}