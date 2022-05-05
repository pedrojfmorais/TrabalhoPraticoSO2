
#include "threads.h"

void limpaEcra() { system("cls"); }

BOOL WINAPI recebeMapaJogoDoServidor(LPVOID p) {

	PartilhaJogo* partilhaJogo = (PartilhaJogo*)p;

	while (1) {
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
	return 0;
}