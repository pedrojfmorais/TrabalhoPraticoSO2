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
void alternaModoAleatorioPeca(DefinicoesJogo* definicoesJogo) {
	definicoesJogo->modoAleatorioLigado = !definicoesJogo->modoAleatorioLigado;
}
BOOL colocaParede_x_y(DWORD x, DWORD y, DadosJogo* jogo) {

	if (jogo->mapaJogo[x][y] == tuboVazio)
		jogo->mapaJogo[x][y] = tuboBloqueado;
	else
		return FALSE;

	return TRUE;
}
void setTempoPararAgua(DWORD segundos, PartilhaJogo* partilhaJogo) {
	partilhaJogo->definicoesJogo.tempoPararAgua = segundos;
}
void trataMensagem(PartilhaJogo* partilhaJogo, TCHAR* comando) {
	TCHAR* next_token = NULL;
	TCHAR* command;
	command = _tcstok_s(comando, _T(" "), &next_token);

	if (_tcscmp(command, _T("pararAgua")) == 0) {
		
		DWORD segundos = (DWORD)_tcstod(_tcstok_s(NULL, _T(" "), &next_token), _T('\0'));
		setTempoPararAgua(segundos, partilhaJogo);

	}else if (_tcscmp(command, _T("meteParede")) == 0) {

		DWORD x = (DWORD)_tcstod(_tcstok_s(NULL, _T(" "), &next_token), _T('\0')) - 1;
		DWORD y = (DWORD)_tcstod(_tcstok_s(NULL, _T(" "), &next_token), _T('\0')) - 1;

		for (DWORD i = 0; i < N_JOGADORES; i++)
			colocaParede_x_y(x, y, partilhaJogo->jogos[i]);

	}else if (_tcscmp(command, _T("alternaModoAleatorio")) == 0) {
		alternaModoAleatorioPeca(&partilhaJogo->definicoesJogo);
	}
}