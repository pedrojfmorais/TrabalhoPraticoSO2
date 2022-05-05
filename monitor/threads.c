
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

		if (partilhaJogo->jogador1->aJogar)
			desenharMapaJogo(partilhaJogo->jogador1);
		if (partilhaJogo->jogador2->aJogar)
			desenharMapaJogo(partilhaJogo->jogador2);

		ReleaseMutex(partilhaJogo->hRWMutex);

		Sleep(1000);
	}
	return 0;
}