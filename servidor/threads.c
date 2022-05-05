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
			DadosJogo* dadosJogo = partilhaJogo->jogador1;

			if (i == 1)
				dadosJogo = partilhaJogo->jogador2;

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

		if (!partilhaJogo->jogador1->aJogar && !partilhaJogo->jogador2->aJogar)
			ResetEvent(partilhaJogo->hEvent);

		Sleep(1000);
	}
}