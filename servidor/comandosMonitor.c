#include "..\utils_so2_tp\utils_so2_tp.c"
#include "comandosMonitor.h"

DWORD getRandomProximaPeca() {
	return getRandomNumberBetweenMaxAndMin(tuboHorizontal, tuboCurvaDireitaBaixo + 1); //maior peça + 1
}
DWORD getProximaPecaSequencia(DWORD pecaAnterior) {
	if (pecaAnterior == tuboCurvaDireitaBaixo)
		return tuboHorizontal;
	else
		return ++pecaAnterior;
}
void alternaModoAleatorioPeca(DadosJogo* dadosJogo) {
	dadosJogo->modoAleatorioLigado = !dadosJogo->modoAleatorioLigado;
}
BOOL colocaParede_x_y(DWORD x, DWORD y, DadosJogo* jogo) {

	if (jogo->mapaJogo[x][y] == tuboVazio)
		jogo->mapaJogo[x][y] = tuboBloqueado;
	else
		return FALSE;

	return TRUE;
}
void setTempoPararAgua(DWORD segundos, DadosJogo* dadosJogo) {
	dadosJogo->tempoPararAgua = segundos;
}
void trataMensagem(PartilhaJogoServidorMonitor* partilhaJogo, TCHAR* comando) {
	TCHAR* next_token = NULL;
	TCHAR* command;
	command = _tcstok_s(comando, _T(" "), &next_token);

	if (_tcscmp(command, _T("pararAgua")) == 0) {

		TCHAR* jogadorString = _tcstok_s(NULL, _T(" "), &next_token);
		TCHAR* segundosString = _tcstok_s(NULL, _T(" "), &next_token);

		if (jogadorString == NULL)
			return;

		DWORD jogador = (DWORD)_tcstod(jogadorString, _T('\0'));

		if (segundosString == NULL){
			DWORD segundos = jogador;
			for (DWORD i = 0; i < N_JOGADORES; i++)
			{
				if (partilhaJogo->jogos[i]->aJogar)
					setTempoPararAgua(segundos, partilhaJogo->jogos[i]);
			}
		}
		else if (jogador <= N_JOGADORES && jogador > 0 && partilhaJogo->jogos[jogador - 1]->aJogar) {

			DWORD segundos = (DWORD)_tcstod(segundosString, _T('\0'));
			setTempoPararAgua(segundos, partilhaJogo->jogos[jogador - 1]);
		}

	}else if (_tcscmp(command, _T("meteParede")) == 0) {

		TCHAR* jogadorString = _tcstok_s(NULL, _T(" "), &next_token);
		TCHAR* xString = _tcstok_s(NULL, _T(" "), &next_token);
		TCHAR* yString = _tcstok_s(NULL, _T(" "), &next_token);

		if (jogadorString == NULL || xString == NULL)
			return;

		DWORD jogador = (DWORD)_tcstod(jogadorString, _T('\0'));
		DWORD x = (DWORD)_tcstod(xString, _T('\0')) - 1;

		if (yString == NULL) {
			DWORD y = x;
			DWORD x = jogador - 1;

			for (DWORD i = 0; i < N_JOGADORES; i++)
				colocaParede_x_y(x, y, partilhaJogo->jogos[i]);
		}
		else if (jogador <= N_JOGADORES && jogador > 0 && partilhaJogo->jogos[jogador - 1]->aJogar) {

			DWORD y = (DWORD)_tcstod(yString, _T('\0')) - 1;;

			colocaParede_x_y(x, y, partilhaJogo->jogos[jogador - 1]);

		}

	}else if (_tcscmp(command, _T("alternaModoAleatorio")) == 0) {

		TCHAR* jogadorString = _tcstok_s(NULL, _T(" "), &next_token);

		if (jogadorString == NULL) {
			for (DWORD i = 0; i < N_JOGADORES; i++)
				alternaModoAleatorioPeca(partilhaJogo->jogos[i]);
			
		}
		else {
			DWORD jogador = (DWORD)_tcstod(jogadorString, _T('\0'));
			if (jogador <= N_JOGADORES && jogador > 0 && partilhaJogo->jogos[jogador - 1]->aJogar)
				alternaModoAleatorioPeca(partilhaJogo->jogos[jogador - 1]);
	
		}
	}
}