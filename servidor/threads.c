#include "threads.h"
#include "pathfindAgua.h"
#include "comandosMonitor.h"

BOOL WINAPI atualizaMapaJogoParaMonitor(LPVOID p) {

	PartilhaJogo* partilhaJogo = (PartilhaJogo*)p;

	while (partilhaJogo->threadMustContinue) {


		WaitForSingleObject(partilhaJogo->hSemaforo, INFINITE);

		if (!partilhaJogo->threadMustContinue)
			break;

		SetEvent(partilhaJogo->hEvent);
		Sleep(500);
		ResetEvent(partilhaJogo->hEvent);

	}
	return TRUE;
}


BOOL WINAPI decorrerJogo(LPVOID p) {
	PartilhaJogo* partilhaJogo = (PartilhaJogo*)p;

	while (partilhaJogo->threadMustContinue) {

		WaitForSingleObject(partilhaJogo->hEventJogosDecorrer, INFINITE);

		BOOL sofreuAlteracoes = FALSE;

		for (DWORD i = 0; i < N_JOGADORES; i++)
		{
			DadosJogo* dadosJogo = partilhaJogo->jogos[i];

			if (!dadosJogo->aJogar || dadosJogo->jogoPausado)
				continue;

			if (dadosJogo->tempoAguaComecaFluir < dadosJogo->tempoDecorrido) {

				if (partilhaJogo->definicoesJogo.tempoPararAgua > 0){
					if (i == 0)
						partilhaJogo->definicoesJogo.tempoPararAgua--;
				
				}else if ((dadosJogo->tempoDecorrido - dadosJogo->tempoAguaComecaFluir) % 2 == 0) {
					//água avança a cada 2 segundos
					if (dadosJogo->ganhou || dadosJogo->perdeu)
						dadosJogo->aJogar = FALSE;

					if (!pathfindAguaJogo(dadosJogo))
						dadosJogo->perdeu = TRUE;
					else
						dadosJogo->pontuacao += 15;
				}
			}

			dadosJogo->tempoDecorrido += 1;
			sofreuAlteracoes = TRUE;
		}
		if (sofreuAlteracoes)
			ReleaseSemaphore(partilhaJogo->hSemaforo, 1, NULL);

		BOOL todosJogosParados = TRUE;

		for (DWORD i = 0; i < N_JOGADORES; i++)
			if (partilhaJogo->jogos[i]->aJogar)
				todosJogosParados = FALSE;
		
		if (todosJogosParados)
			ResetEvent(partilhaJogo->hEvent);

		Sleep(1000);
	}
}

BOOL WINAPI recebeMensagemMonitor(LPVOID p) {
	PartilhaJogo* partilhaJogo = (PartilhaJogo*)p;

	while (partilhaJogo->threadMustContinue) {

		WaitForSingleObject(partilhaJogo->hSemaforoLeituraBufferCircularMonitorParaServidor, INFINITE);
		WaitForSingleObject(partilhaJogo->hMutexBufferCircularMonitorParaServidor, INFINITE);

		trataMensagem(partilhaJogo, partilhaJogo->bufferCircularMonitorParaServidor->buffer[partilhaJogo->bufferCircularMonitorParaServidor->rP].mensagem);

		(partilhaJogo->bufferCircularMonitorParaServidor->rP) += 1;

		if (partilhaJogo->bufferCircularMonitorParaServidor->rP == BUFFER_SIZE)
			partilhaJogo->bufferCircularMonitorParaServidor->rP = 0;

		ReleaseMutex(partilhaJogo->hMutexBufferCircularMonitorParaServidor);
		ReleaseSemaphore(partilhaJogo->hSemaforoEscritaBufferCircularMonitorParaServidor, 1, NULL);

	}
	return TRUE;
}


DWORD WINAPI leMensagemUtilizador(LPVOID p) {
	PartilhaJogo* partilhaJogo = (PartilhaJogo*)p;
	BufferCell cell;

	while (partilhaJogo->threadMustContinue) {

		_tprintf(_T("Insira um comando: "));
		_getts_s(cell.mensagem, TAM);

		if (_tcscmp(cell.mensagem, _T("exit")) == 0) {

			WaitForSingleObject(partilhaJogo->hSemaforoEscritaBufferCircularServidorParaMonitor, INFINITE); // Produtor verifica se pode 
			WaitForSingleObject(partilhaJogo->hMutexBufferCircularServidorParaMonitor, INFINITE); // Inicio de zona critica

			CopyMemory(&(partilhaJogo->bufferCircularServidorParaMonitor->buffer[partilhaJogo->bufferCircularServidorParaMonitor->wP]), &cell, sizeof(BufferCell)); // Copia para o buffer circular

			(partilhaJogo->bufferCircularServidorParaMonitor->wP) += 1;

			if (partilhaJogo->bufferCircularServidorParaMonitor->wP == BUFFER_SIZE)
				partilhaJogo->bufferCircularServidorParaMonitor->wP = 0; // Volta ao inicio

			ReleaseMutex(partilhaJogo->hMutexBufferCircularServidorParaMonitor); // Fim da zona critica
			ReleaseSemaphore(partilhaJogo->hSemaforoLeituraBufferCircularServidorParaMonitor, 1, NULL);

			partilhaJogo->threadMustContinue = FALSE;

		}
		if (_tcscmp(cell.mensagem, _T("start")) == 0) {
			SetEvent(partilhaJogo->hEventJogosDecorrer);
			ReleaseSemaphore(partilhaJogo->hSemaforo, 1, NULL);
		}
	}
	return 0;
}