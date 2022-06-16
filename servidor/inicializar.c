#include "inicializar.h"
#include "registry.h"
#include "comandosMonitor.h"


void inicializaJogo(DadosJogo* jogo, DefinicoesJogo definicoesJogo) {

	jogo->nLinhas = definicoesJogo.nLinhas;
	jogo->nColunas = definicoesJogo.nColunas;

	jogo->tempoAguaComecaFluir = definicoesJogo.tempoAguaComecaFluir;
	jogo->tempoDecorrido = 0;

	jogo->aJogar = TRUE;
	jogo->ganhou = FALSE;
	jogo->perdeu = FALSE;
	jogo->jogoPausado = FALSE;

	jogo->pontuacao = 0;

	jogo->coordenadasOrigemAgua[0] = getRandomNumberBetweenMaxAndMin(0, jogo->nLinhas);
	jogo->coordenadasOrigemAgua[1] = 0;

	jogo->coordenadasDestinoAgua[0] = getRandomNumberBetweenMaxAndMin(0, jogo->nLinhas);
	jogo->coordenadasDestinoAgua[1] = jogo->nColunas - 1;

	//Verifica se a origem e destino da àgua estão em quadrantes diagonais opostos
	if (jogo->coordenadasOrigemAgua[0] > (jogo->nLinhas / 2) && jogo->coordenadasDestinoAgua[0] > (jogo->nLinhas / 2))
		jogo->coordenadasDestinoAgua[0] -= jogo->nLinhas / 2;

	else if (jogo->coordenadasOrigemAgua[0] < (jogo->nLinhas / 2) && jogo->coordenadasDestinoAgua[0] < (jogo->nLinhas / 2))
		jogo->coordenadasDestinoAgua[0] += jogo->nLinhas / 2;

	for (DWORD i = 0; i < jogo->nLinhas; i++) {
		for (DWORD j = 0; j < jogo->nColunas; j++) {
			if (i == jogo->coordenadasOrigemAgua[0] && j == jogo->coordenadasOrigemAgua[1])
				jogo->mapaJogo[i][j] = tuboOrigemAgua * 10;
			else if (i == jogo->coordenadasDestinoAgua[0] && j == jogo->coordenadasDestinoAgua[1])
				jogo->mapaJogo[i][j] = tuboDestinoAgua;
			else
				jogo->mapaJogo[i][j] = tuboVazio;
		}
	}

	jogo->coordenadaAtualAgua[0] = jogo->coordenadasOrigemAgua[0];
	jogo->coordenadaAtualAgua[1] = jogo->coordenadasOrigemAgua[1];

	jogo->modoAleatorioLigado = TRUE;
	jogo->tempoPararAgua = 0;

	if (jogo->modoAleatorioLigado)
		jogo->proximaPeca = getRandomProximaPeca();
	else
		jogo->proximaPeca = tuboHorizontal; // primeira peça
}

void inicializaServidor(int argc, TCHAR* argv[], PartilhaJogoServidorMonitor* partilhaJogoMonitor, PartilhaJogoServidorCliente* partilhaJogoCliente) {

	TCHAR nomeChaves[3][TAM] = { _T("nLinhas"), _T("nColunas"), _T("tempoAguaComecaFluir") };
	DWORD definicoesJogo[3];

	partilhaJogoMonitor->deveContinuar = 1;

	if (argc == 1) {
		//Lê no registry
		if (!lerDoRegistryDadosMapaJogo(nomeChaves, definicoesJogo))
			exit(1);

	}
	else if (argc == 4) {
		//Guarda no registry
		for (DWORD i = 0; i < 3; i++)
			definicoesJogo[i] = (DWORD)_tcstod(argv[i + 1], _T('\0'));

		for (DWORD i = 0; i < 2; i++)
			if (definicoesJogo[i] > 20)
				definicoesJogo[i] = 20;
			else if (definicoesJogo[i] < 5)
				definicoesJogo[i] = 5;

		if (!guardaNoRegistryDadosMapaJogo(nomeChaves, definicoesJogo))
			_tprintf(_T("Não foi possivel guardar os valores no Registry!"));

	}
	else {
		_tprintf(_T("Número de argumentos inválido!"));
		exit(1);
	}

	partilhaJogoMonitor->definicoesJogo.nLinhas = definicoesJogo[0];
	partilhaJogoMonitor->definicoesJogo.nColunas = definicoesJogo[1];
	partilhaJogoMonitor->definicoesJogo.tempoAguaComecaFluir = definicoesJogo[2];

	for (DWORD i = 0; i < N_JOGADORES; i++)
	{
		partilhaJogoMonitor->jogos[i]->idJogador = i + 1;

		partilhaJogoCliente->deveContinuar = partilhaJogoMonitor->deveContinuar;
		partilhaJogoCliente->hEventFecharTudo = partilhaJogoMonitor->hEventFecharTudo;
		partilhaJogoCliente->hReadWriteMutexAtualizacaoNoJogo = partilhaJogoMonitor->hReadWriteMutexAtualizacaoNoJogo;
		partilhaJogoCliente->jogos[i] = partilhaJogoMonitor->jogos[i];
	}
}
