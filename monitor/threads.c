
#include "threads.h"

void limpaEcra() { system("cls"); }

BOOL WINAPI recebeMapaJogoDoServidor(LPVOID p) {

	PartilhaJogo* partilhaJogo = (PartilhaJogo*)p;

	while (1) {

		WaitForSingleObject(partilhaJogo->hEventAtualizacaoNoJogo, INFINITE);
		WaitForSingleObject(partilhaJogo->hReadWriteMutexAtualizacaoNoJogo, INFINITE);

		limpaEcra();

		for (DWORD i = 0; i < N_JOGADORES; i++)
			if (partilhaJogo->jogos[i]->aJogar)
				desenharMapaJogo(partilhaJogo->jogos[i]);

		ReleaseMutex(partilhaJogo->hReadWriteMutexAtualizacaoNoJogo);

		Sleep(1000);
	}
	return TRUE;
}

DWORD WINAPI enviaMensagemServidor(LPVOID p) {
	PartilhaJogo* partilhaJogo = (PartilhaJogo*)p;
	BufferCell cell;

	while (1) {
		
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

DWORD WINAPI acabarThreads(LPVOID p) {

	ThreadsMonitor* threadsMonitor = (ThreadsMonitor*)p;

	WaitForSingleObject(threadsMonitor->hEventFecharTudo, INFINITE);

	for (DWORD i = 0; i < N_THREADS_MONITOR; i++)
		TerminateThread(threadsMonitor->hThreads[i], 0);

	return 0;
}