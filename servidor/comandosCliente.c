#include "comandosCliente.h"

BOOL colocaPeca(DadosJogo* dadosJogo, int linha, int coluna) {
	
	if (dadosJogo->mapaJogo[linha][coluna] == tuboBloqueado)
		return FALSE;

	if (dadosJogo->mapaJogo[linha][coluna] / 10 > 0)
		return FALSE;

	if (dadosJogo->mapaJogo[linha][coluna] == tuboOrigemAgua || dadosJogo->mapaJogo[linha][coluna] == tuboDestinoAgua)
		return FALSE;

	dadosJogo->mapaJogo[linha][coluna] = dadosJogo->proximaPeca;

	if (dadosJogo->modoAleatorioLigado)
		dadosJogo->proximaPeca = getRandomProximaPeca();
	else
		dadosJogo->proximaPeca = getProximaPecaSequencia(dadosJogo->proximaPeca);

	return TRUE;
}

BOOL limpaPeca(DadosJogo* dadosJogo, int linha, int coluna) {
	if (dadosJogo->mapaJogo[linha][coluna] == tuboBloqueado)
		return FALSE;

	if (dadosJogo->mapaJogo[linha][coluna] / 10 > 0)
		return FALSE;

	if (dadosJogo->mapaJogo[linha][coluna] == tuboOrigemAgua || dadosJogo->mapaJogo[linha][coluna] == tuboDestinoAgua)
		return FALSE;

	dadosJogo->mapaJogo[linha][coluna] = tuboVazio;

	return TRUE;
}