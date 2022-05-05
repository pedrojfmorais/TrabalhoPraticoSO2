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