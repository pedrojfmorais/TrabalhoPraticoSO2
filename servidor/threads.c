#include "threads.h"
#include "pathfindAgua.h"

BOOL WINAPI atualizaMapaJogoParaMonitor(LPVOID p) {

	PartilhaJogo* partilhaJogo = (PartilhaJogo*)p;

	while (1) {


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

	while (1) {

		WaitForSingleObject(partilhaJogo->hEventJogosDecorrer, INFINITE);

		BOOL sofreuAlteracoes = FALSE;

		for (DWORD i = 0; i < N_JOGADORES; i++)
		{
			DadosJogo* dadosJogo = partilhaJogo->jogos[i];

			if (!dadosJogo->aJogar || dadosJogo->jogoPausado)
				continue;

			if (dadosJogo->tempoAguaComecaFluir < dadosJogo->tempoDecorrido) {

				if (partilhaJogo->definicoesJogo.tempoPararAgua > 0)
					partilhaJogo->definicoesJogo.tempoPararAgua--;

				else if ((dadosJogo->tempoDecorrido - dadosJogo->tempoAguaComecaFluir) % 2 == 0) {
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
//testar
void trataMensagem(TCHAR* Comando);

BOOL WINAPI recebeMensagemMonitor(LPVOID p) {
	SharedMemBufferCircular* bufferCircular = (SharedMemBufferCircular*)p;

	while (1) {


		WaitForSingleObject(bufferCircular->hSemaforoLeitura, INFINITE);
		WaitForSingleObject(bufferCircular->hMutex, INFINITE);

		trataMensagem(bufferCircular->buffer[bufferCircular->rP].mensagem);
		(bufferCircular->rP) += 1;

		if (bufferCircular->rP == BUFFER_SIZE)
			bufferCircular->rP = 0;

		ReleaseMutex(bufferCircular->hMutex);

	}
	return TRUE;
}
