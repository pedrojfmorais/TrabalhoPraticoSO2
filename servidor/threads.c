#include "threads.h"
#include "pathfindAgua.h"
#include "comandosMonitor.h"
#include "inicializar.h"

BOOL WINAPI atualizaMapaJogoParaMonitor(LPVOID p) {

	PartilhaJogo* partilhaJogo = (PartilhaJogo*)p;

	while (1) {

		WaitForSingleObject(partilhaJogo->hSemaforo, INFINITE);

		SetEvent(partilhaJogo->hEvent);
		Sleep(500);
		ResetEvent(partilhaJogo->hEvent);

	}
	return TRUE;
}


BOOL WINAPI decorrerJogo(LPVOID p) {
	PartilhaJogo* partilhaJogo = (PartilhaJogo*)p;

	while (1) {

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
	return TRUE;
}

BOOL WINAPI recebeMensagemMonitor(LPVOID p) {
	PartilhaJogo* partilhaJogo = (PartilhaJogo*)p;

	while (1) {

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

	while (1) {

		_tprintf(_T("Insira um comando: "));
		_getts_s(cell.mensagem, TAM);

		if (_tcscmp(cell.mensagem, _T("exit")) == 0) {

			SetEvent(partilhaJogo->hEventFecharTudo);

		}
		else if (_tcscmp(cell.mensagem, _T("start")) == 0) {
			for (DWORD i = 0; i < N_JOGADORES; i++)
			{
				if (!partilhaJogo->jogos[i]->aJogar) {
					inicializaJogo(partilhaJogo->jogos[i], partilhaJogo->definicoesJogo);
					
					if (i == 0) {

						SetEvent(partilhaJogo->hEventJogosDecorrer);
						ReleaseSemaphore(partilhaJogo->hSemaforo, 1, NULL);
					}
					break;
				}
			}
		}
		else if (_tcscmp(cell.mensagem, _T("pausarJogo")) == 0) {
			for (DWORD i = 0; i < N_JOGADORES; i++)
			{
				if (partilhaJogo->jogos[i]->aJogar) {
					partilhaJogo->jogos[i]->jogoPausado = !partilhaJogo->jogos[i]->jogoPausado;
				}
			}
		}
		else if (_tcscmp(cell.mensagem, _T("listar")) == 0) {
			for (DWORD i = 0; i < N_JOGADORES; i++)
			{
				if (partilhaJogo->jogos[i]->aJogar) {
					_tprintf(_T("Jogador %d\nPontução: %d\n\n"), partilhaJogo->jogos[i]->idJogador, partilhaJogo->jogos[i]->pontuacao);
				}
			}
		}
	}
	return TRUE;
}

DWORD WINAPI acabarThreads(LPVOID p) {

	ThreadsServidor* threadsServidor = (ThreadsServidor*)p;

	WaitForSingleObject(threadsServidor->hEventFecharTudo, INFINITE);

	for (DWORD i = 0; i < N_THREADS_SERVIDOR; i++)
		TerminateThread(threadsServidor->hThreads[i], 0);

	return 0;
}