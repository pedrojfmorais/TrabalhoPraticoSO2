
#include "pathfindAgua.h"

BOOL pathfindAguaParaCima(DadosJogo* dadosJogo) {

	if (dadosJogo->coordenadaAtualAgua[0] == 0)
		return FALSE;

	dadosJogo->coordenadaAtualAgua[0] -= 1;

	switch (dadosJogo->mapaJogo[dadosJogo->coordenadaAtualAgua[0]][dadosJogo->coordenadaAtualAgua[1]])
	{
	case tuboCurvaDireitaBaixo:
	case tuboCurvaEsquerdaBaixo:
	case tuboVertical:
		dadosJogo->mapaJogo[dadosJogo->coordenadaAtualAgua[0]][dadosJogo->coordenadaAtualAgua[1]] *= 10;
		return TRUE;
	}

	return FALSE;
}
BOOL pathfindAguaParaBaixo(DadosJogo* dadosJogo) {

	if (dadosJogo->coordenadaAtualAgua[0] == dadosJogo->nLinhas - 1)
		return FALSE;

	dadosJogo->coordenadaAtualAgua[0] += 1;

	switch (dadosJogo->mapaJogo[dadosJogo->coordenadaAtualAgua[0]][dadosJogo->coordenadaAtualAgua[1]])
	{
	case tuboCurvaDireitaCima:
	case tuboCurvaEsquerdaCima:
	case tuboVertical:
		dadosJogo->mapaJogo[dadosJogo->coordenadaAtualAgua[0]][dadosJogo->coordenadaAtualAgua[1]] *= 10;
		return TRUE;
	}
	return FALSE;
}
BOOL pathfindAguaParaEsquerda(DadosJogo* dadosJogo) {

	if (dadosJogo->coordenadaAtualAgua[1] == 0)
		return FALSE;

	dadosJogo->coordenadaAtualAgua[1] -= 1;

	switch (dadosJogo->mapaJogo[dadosJogo->coordenadaAtualAgua[0]][dadosJogo->coordenadaAtualAgua[1]])
	{
	case tuboCurvaDireitaCima:
	case tuboCurvaDireitaBaixo:
	case tuboHorizontal:
		dadosJogo->mapaJogo[dadosJogo->coordenadaAtualAgua[0]][dadosJogo->coordenadaAtualAgua[1]] *= 10;
		return TRUE;
	}
	return FALSE;
}
BOOL pathfindAguaParaDireita(DadosJogo* dadosJogo) {

	if (dadosJogo->coordenadaAtualAgua[1] == dadosJogo->nColunas - 1)
		return FALSE;

	dadosJogo->coordenadaAtualAgua[1] += 1;

	switch (dadosJogo->mapaJogo[dadosJogo->coordenadaAtualAgua[0]][dadosJogo->coordenadaAtualAgua[1]])
	{
	case tuboCurvaEsquerdaCima:
	case tuboCurvaEsquerdaBaixo:
	case tuboHorizontal:
		dadosJogo->mapaJogo[dadosJogo->coordenadaAtualAgua[0]][dadosJogo->coordenadaAtualAgua[1]] *= 10;
		return TRUE;
	case tuboDestinoAgua:
		dadosJogo->mapaJogo[dadosJogo->coordenadaAtualAgua[0]][dadosJogo->coordenadaAtualAgua[1]] *= 10;
		dadosJogo->ganhou = TRUE;
		return TRUE;
	}
	return FALSE;
}
BOOL pathfindAguaJogo(DadosJogo* dadosJogo) {

	//coordenada atual da água no mapa
	switch (dadosJogo->mapaJogo[dadosJogo->coordenadaAtualAgua[0]][dadosJogo->coordenadaAtualAgua[1]] / 10)
	{
	case tuboCurvaEsquerdaCima:

		if (dadosJogo->coordenadaAtualAgua[0] != 0 &&
			dadosJogo->mapaJogo[dadosJogo->coordenadaAtualAgua[0] - 1][dadosJogo->coordenadaAtualAgua[1]] / 10 > 0)

			return pathfindAguaParaEsquerda(dadosJogo);

		else if (dadosJogo->coordenadaAtualAgua[1] != 0 &&
			dadosJogo->mapaJogo[dadosJogo->coordenadaAtualAgua[0]][dadosJogo->coordenadaAtualAgua[1] - 1] / 10 > 0)

			return pathfindAguaParaCima(dadosJogo);
		else
			return FALSE;

	case tuboCurvaDireitaCima:
		if (dadosJogo->coordenadaAtualAgua[1] != dadosJogo->nColunas - 1 &&
			dadosJogo->mapaJogo[dadosJogo->coordenadaAtualAgua[0]][dadosJogo->coordenadaAtualAgua[1] + 1] / 10 > 0)

			return pathfindAguaParaCima(dadosJogo);

		else if (dadosJogo->coordenadaAtualAgua[0] != 0 &&
			dadosJogo->mapaJogo[dadosJogo->coordenadaAtualAgua[0] - 1][dadosJogo->coordenadaAtualAgua[1]] / 10 > 0)

			return pathfindAguaParaDireita(dadosJogo);
		else
			return FALSE;

	case tuboCurvaEsquerdaBaixo:

		if (dadosJogo->coordenadaAtualAgua[0] != dadosJogo->nLinhas - 1 &&
			dadosJogo->mapaJogo[dadosJogo->coordenadaAtualAgua[0] + 1][dadosJogo->coordenadaAtualAgua[1]] / 10 > 0)

			return pathfindAguaParaEsquerda(dadosJogo);

		else if (dadosJogo->coordenadaAtualAgua[1] != 0 &&
			dadosJogo->mapaJogo[dadosJogo->coordenadaAtualAgua[0]][dadosJogo->coordenadaAtualAgua[1] - 1] / 10 > 0)

			return pathfindAguaParaBaixo(dadosJogo);

		else
			return FALSE;

	case tuboCurvaDireitaBaixo:
		if (dadosJogo->coordenadaAtualAgua[1] != dadosJogo->nColunas - 1 &&
			dadosJogo->mapaJogo[dadosJogo->coordenadaAtualAgua[0]][dadosJogo->coordenadaAtualAgua[1] + 1] / 10 > 0)

			return pathfindAguaParaBaixo(dadosJogo);

		else if (dadosJogo->coordenadaAtualAgua[0] != dadosJogo->nLinhas - 1 &&
			dadosJogo->mapaJogo[dadosJogo->coordenadaAtualAgua[0] + 1][dadosJogo->coordenadaAtualAgua[1]] / 10 > 0)

			return pathfindAguaParaDireita(dadosJogo);
		else
			return FALSE;

	case tuboHorizontal:
		if (dadosJogo->coordenadaAtualAgua[1] != 0 &&
			dadosJogo->mapaJogo[dadosJogo->coordenadaAtualAgua[0]][dadosJogo->coordenadaAtualAgua[1] - 1] / 10 > 0)

			return pathfindAguaParaDireita(dadosJogo);

		else if (dadosJogo->coordenadaAtualAgua[1] != dadosJogo->nColunas - 1 &&
			dadosJogo->mapaJogo[dadosJogo->coordenadaAtualAgua[0]][dadosJogo->coordenadaAtualAgua[1] + 1] / 10 > 0)

			return pathfindAguaParaEsquerda(dadosJogo);

		else
			return FALSE;

	case tuboVertical:
		if (dadosJogo->coordenadaAtualAgua[0] != 0 &&
			dadosJogo->mapaJogo[dadosJogo->coordenadaAtualAgua[0] - 1][dadosJogo->coordenadaAtualAgua[1]] / 10 > 0)

			return pathfindAguaParaBaixo(dadosJogo);

		else if (dadosJogo->coordenadaAtualAgua[0] != dadosJogo->nLinhas - 1 &&
			dadosJogo->mapaJogo[dadosJogo->coordenadaAtualAgua[0] + 1][dadosJogo->coordenadaAtualAgua[1]] / 10 > 0)

			return pathfindAguaParaCima(dadosJogo);
		else
			return FALSE;

	case tuboOrigemAgua:
		return pathfindAguaParaDireita(dadosJogo);

	case tuboDestinoAgua:
		dadosJogo->mapaJogo[dadosJogo->coordenadaAtualAgua[0]][dadosJogo->coordenadaAtualAgua[1]] *= 10;
		dadosJogo->ganhou = TRUE;
		return TRUE;

	}
	return FALSE;
}
