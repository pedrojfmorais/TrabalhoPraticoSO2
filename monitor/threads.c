
#include "threads.h"

void limpaEcra() { system("cls"); }

BOOL WINAPI recebeMapaJogoDoServidor(LPVOID p) {

	PartilhaJogo* partilhaJogo = (PartilhaJogo*)p;

	while (partilhaJogo->threadMustContinue) {
		WaitForSingleObject(partilhaJogo->hEvent, INFINITE);
		if (!partilhaJogo->threadMustContinue)
			return 0;

		WaitForSingleObject(partilhaJogo->hRWMutex, INFINITE);

		limpaEcra();

		for (DWORD i = 0; i < N_JOGADORES; i++)
			if (partilhaJogo->jogos[i]->aJogar)
				desenharMapaJogo(partilhaJogo->jogos[i]);

		ReleaseMutex(partilhaJogo->hRWMutex);

		Sleep(1000);
	}
	return TRUE;
}

DWORD WINAPI enviaMensagemServidor(LPVOID p) {
	PartilhaJogo* partilhaJogo = (PartilhaJogo*)p;
	BufferCell cell;

	while (partilhaJogo->threadMustContinue) {
		
		_tprintf(_T("Insira um comando: "));
		_getts_s(cell.mensagem, TAM);
		
		WaitForSingleObject(partilhaJogo->hSemaforoEscritaBufferCircularMonitorParaServidor, INFINITE); // Produtor verifica se pode 
		WaitForSingleObject(partilhaJogo->hMutexBufferCircularMonitorParaServidor, INFINITE); // Inicio de zona critica

		CopyMemory(&(partilhaJogo->bufferCircularMonitorParaServidor->buffer[partilhaJogo->bufferCircularMonitorParaServidor->wP]), &cell, sizeof(BufferCell)); // Copia para o buffer circular

		(partilhaJogo->bufferCircularMonitorParaServidor->wP) += 1;

		if (partilhaJogo->bufferCircularMonitorParaServidor->wP == BUFFER_SIZE)
			partilhaJogo->bufferCircularMonitorParaServidor->wP = 0; // Volta ao inicio

		ReleaseMutex(partilhaJogo->hMutexBufferCircularMonitorParaServidor); // Fim da zona critica
		ReleaseSemaphore(partilhaJogo->hSemaforoLeituraBufferCircularMonitorParaServidor, 1, NULL);

	}
	return TRUE;
}

BOOL WINAPI recebeMensagemServidor(LPVOID p) {
	PartilhaJogo* partilhaJogo = (PartilhaJogo*)p;

	while (partilhaJogo->threadMustContinue) {

		WaitForSingleObject(partilhaJogo->hSemaforoLeituraBufferCircularServidorParaMonitor, INFINITE);
		WaitForSingleObject(partilhaJogo->hMutexBufferCircularServidorParaMonitor, INFINITE);


		TCHAR* next_token = NULL;
		TCHAR* command;
		command = _tcstok_s(
			partilhaJogo->bufferCircularServidorParaMonitor->buffer[partilhaJogo->bufferCircularServidorParaMonitor->rP].mensagem, 
			_T(" "), 
			&next_token);


		if (_tcscmp(command, _T("exit")) == 0)
			partilhaJogo->threadMustContinue = FALSE;

		(partilhaJogo->bufferCircularServidorParaMonitor->rP) += 1;

		if (partilhaJogo->bufferCircularServidorParaMonitor->rP == BUFFER_SIZE)
			partilhaJogo->bufferCircularServidorParaMonitor->rP = 0;

		ReleaseMutex(partilhaJogo->hMutexBufferCircularServidorParaMonitor);
		ReleaseSemaphore(partilhaJogo->hSemaforoEscritaBufferCircularServidorParaMonitor, 1, NULL);

	}
	return TRUE;
}