#include "..\\utils_so2_tp\utils_so2_tp.h"

// TESTES

// Retirado da internet "https://www.codewithc.com/change-text-color-in-codeblocks-console-window/"
void SetColor(int ForgC)
{
	WORD wColor;
	//This handle is needed to get the current background attribute

	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	//csbi is used for wAttributes word

	if (GetConsoleScreenBufferInfo(hStdOut, &csbi))
	{
		//To mask out all but the background attribute, and to add the color
		wColor = (csbi.wAttributes & 0xF0) + (ForgC & 0x0F);
		SetConsoleTextAttribute(hStdOut, wColor);
	}
	return;
}

void printTuboOrigemAgua(int i, int j, BOOL temAgua) {
	if (i % 3 == 0 || i % 3 == 2)
		_tprintf(_T("█"));

	else if (i % 3 == 1) {
		if (j % 3 == 0)
			_tprintf(_T("█"));

		else if (j % 3 == 1 || j % 3 == 2) {
			if (temAgua) {
				SetColor(CorAzulClaro);
				_tprintf(_T("█"));
				SetColor(CorAmareloClaro);
			}
			else
				_tprintf(_T(" "));
		}
	}
}
void printTuboDestinoAgua(int i, int j, BOOL temAgua) {
	if (i % 3 == 0 || i % 3 == 2)
		_tprintf(_T("█"));

	else if (i % 3 == 1) {
		if (j % 3 == 0 || j % 3 == 1) {
			if (temAgua) {
				SetColor(CorAzulClaro);
				_tprintf(_T("█"));
				SetColor(CorAmareloClaro);
			}
			else
				_tprintf(_T(" "));
		}
		else if (j % 3 == 2)
			_tprintf(_T("█"));

	}
}
void printTuboHorizontal(int i, int j, BOOL temAgua) {
	if (i % 3 == 0 || i % 3 == 2)
		_tprintf(_T("█"));

	else if (i % 3 == 1) {
		if (temAgua) {
			SetColor(CorAzulClaro);
			_tprintf(_T("█"));
			SetColor(CorAmareloClaro);
		}
		else
			_tprintf(_T(" "));
	}
}
void printTuboVertical(int i, int j, BOOL temAgua) {
	if (j % 3 == 0 || j % 3 == 2)
		_tprintf(_T("█"));

	else if (j % 3 == 1) {
		if (temAgua) {
			SetColor(CorAzulClaro);
			_tprintf(_T("█"));
			SetColor(CorAmareloClaro);
		}
		else
			_tprintf(_T(" "));
	}
}
void printTuboCurvaEsquerdaCima(int i, int j, BOOL temAgua) {
	if ((j % 3 == 0 && i % 3 != 1) || (i % 3 == 2 && j % 3 == 1) || (j % 3 == 2))
		_tprintf(_T("█"));

	else {
		if (temAgua) {
			SetColor(CorAzulClaro);
			_tprintf(_T("█"));
			SetColor(CorAmareloClaro);
		}
		else
			_tprintf(_T(" "));
	}
}
void printTuboCurvaEsquerdaBaixo(int i, int j, BOOL temAgua) {
	if ((j % 3 == 0 && i % 3 != 1) || (i % 3 == 0 && j % 3 == 1) || (j % 3 == 2))
		_tprintf(_T("█"));

	else {
		if (temAgua) {
			SetColor(CorAzulClaro);
			_tprintf(_T("█"));
			SetColor(CorAmareloClaro);
		}
		else
			_tprintf(_T(" "));
	}
}
void printTuboCurvaDireitaCima(int i, int j, BOOL temAgua) {
	if ((j % 3 == 2 && i % 3 != 1) || (i % 3 == 2 && j % 3 == 1) || (j % 3 == 0))
		_tprintf(_T("█"));

	else {
		if (temAgua) {
			SetColor(CorAzulClaro);
			_tprintf(_T("█"));
			SetColor(CorAmareloClaro);
		}
		else
			_tprintf(_T(" "));
	}
}
void printTuboCurvaDireitaBaixo(int i, int j, BOOL temAgua) {
	if ((j % 3 == 2 && i % 3 != 1) || (i % 3 == 0 && j % 3 == 1) || (j % 3 == 0))
		_tprintf(_T("█"));

	else {
		if (temAgua) {
			SetColor(CorAzulClaro);
			_tprintf(_T("█"));
			SetColor(CorAmareloClaro);
		}
		else
			_tprintf(_T(" "));
	}
}
void printPeca(DWORD peca, int i, int j, BOOL temAgua) {
	SetColor(CorAmareloClaro);
	switch (peca)
	{
	case tuboVazio:
		_tprintf(_T(" "));
		break;
	case tuboBloqueado:
		_tprintf(_T("█"));
		break;
	case tuboOrigemAgua:
		printTuboOrigemAgua(i, j, temAgua);
		break;
	case tuboDestinoAgua:
		printTuboDestinoAgua(i, j, temAgua);
		break;
	case tuboHorizontal:
		printTuboHorizontal(i, j, temAgua);
		break;
	case tuboVertical:
		printTuboVertical(i, j, temAgua);
		break;
	case tuboCurvaEsquerdaCima:
		printTuboCurvaEsquerdaCima(i, j, temAgua);
		break;
	case tuboCurvaEsquerdaBaixo:
		printTuboCurvaEsquerdaBaixo(i, j, temAgua);
		break;
	case tuboCurvaDireitaCima:
		printTuboCurvaDireitaCima(i, j, temAgua);
		break;
	case tuboCurvaDireitaBaixo:
		printTuboCurvaDireitaBaixo(i, j, temAgua);
		break;
	default:
		_tprintf(_T(" "));
	}
}

void desenharMapaJogo(DadosJogo* dadosJogo) {

	_tprintf(_T("\n\n\n"));

	SetColor(CorBranco);
	for (DWORD i = 0; i < dadosJogo->nLinhas * 3; i++)
	{
		if (i == 0) {
			_tprintf(_T("  "));

			SetColor(CorVerde);
			for (DWORD j = 0; j < dadosJogo->nColunas * 3; j++)
				if (j % 3 == 1)
					_tprintf(_T("%d"), (j / 3) + 1);
				else
					_tprintf(_T(" "));
			_tprintf(_T("\n "));

			SetColor(CorSalmao);
			for (DWORD j = 0; j < (dadosJogo->nColunas * 3) + 2; j++)
				_tprintf(_T("-"));

			_tprintf(_T("\n"));
		}

		for (DWORD j = 0; j < dadosJogo->nColunas * 3; j++)
		{
			if (j == 0) {
				SetColor(CorVerde);
				if (i % 3 == 1)
					_tprintf(_T("%d"), (i / 3) + 1);
				else
					_tprintf(_T(" "));

				SetColor(CorSalmao);
				_tprintf(_T("|"));
			}

			SetColor(CorBranco);

			int tipoTubo = dadosJogo->mapaJogo[i / 3][j / 3];
			BOOL temAgua = FALSE;

			if (tipoTubo / 10 > 0) {
				tipoTubo /= 10;
				temAgua = TRUE;
			}

			printPeca(tipoTubo, i, j, temAgua);
		}
		SetColor(CorSalmao);
		_tprintf(_T("|"));

		switch (i)
		{
		case 0:
			_tprintf(_T("\t"));
			if (dadosJogo->perdeu) {

				SetColor(CorVermelho);
				_tprintf(_T("O jogador %d perdeu!"), dadosJogo->idJogador);

			}
			else if (dadosJogo->ganhou) {

				SetColor(CorVerde);
				_tprintf(_T("O jogador %d ganhou!"), dadosJogo->idJogador);

			}
			else if (dadosJogo->jogoPausado) {
				SetColor(CorAmareloClaro);
				_tprintf(_T("Jogo em pausa!"));
			}
			else if (dadosJogo->tempoPararAgua > 0) {
				SetColor(CorAzul);
				_tprintf(_T("Água está parada!"));
			}
			break;
		case 1:
			_tprintf(_T("\t"));
			SetColor(CorBranco);
			_tprintf(_T("Jogador %d"), dadosJogo->idJogador);
			break;
		case 2:
			_tprintf(_T("\t"));
			SetColor(CorBranco);
			_tprintf(_T("Pontuação: %d"), dadosJogo->pontuacao);
			break;
		case 3:
			_tprintf(_T("\t"));
			SetColor(CorBranco);
			_tprintf(_T("Tempo decorrido: %d segundos"), dadosJogo->tempoDecorrido);
			break;
		case 6:
			SetColor(CorBranco);
			if (dadosJogo->modoAleatorioLigado)
				_tprintf(_T("\tPeças em modo aleatório"));
			else
				_tprintf(_T("\tPeças em modo sequencial"));
			break;

		case 8:
			SetColor(CorBranco);
			_tprintf(_T("\tPróxima peça:"));
			break;
		default:
			break;
		}

		if (i == 9 || i == 10 || i == 11) {
			_tprintf(_T("\t"));
			for (int j = 0; j < 3; j++)
			{
				printPeca(dadosJogo->proximaPeca, i, j, FALSE);
			}
		}

		_tprintf(_T("\n"));
		SetColor(CorSalmao);

		if (i == (dadosJogo->nLinhas * 3) - 1) {

			for (DWORD i = 0; i < (dadosJogo->nColunas * 3) + 2; i++) {
				if (i == 0)
					_tprintf(_T(" "));

				_tprintf(_T("-"));
			}
			_tprintf(_T("\n"));
		}
		SetColor(CorBranco);
	}
}
// FIM TESTES

void copiarTabuleiroJogoRecebido(DadosJogo recebido, DadosJogo* resultadoCopia) {
	resultadoCopia->aJogar = recebido.aJogar;
	resultadoCopia->coordenadaAtualAgua[0] = recebido.coordenadaAtualAgua[0];
	resultadoCopia->coordenadaAtualAgua[1] = recebido.coordenadaAtualAgua[1];
	resultadoCopia->coordenadasDestinoAgua[0] = recebido.coordenadasDestinoAgua[0];
	resultadoCopia->coordenadasDestinoAgua[1] = recebido.coordenadasDestinoAgua[1];
	resultadoCopia->coordenadasOrigemAgua[0] = recebido.coordenadasOrigemAgua[0];
	resultadoCopia->coordenadasOrigemAgua[1] = recebido.coordenadasOrigemAgua[1];
	resultadoCopia->ganhou = recebido.ganhou;
	resultadoCopia->idJogador = recebido.idJogador;
	resultadoCopia->jogoPausado = recebido.jogoPausado;
	resultadoCopia->modoAleatorioLigado = recebido.modoAleatorioLigado;
	resultadoCopia->nColunas = recebido.nColunas;
	resultadoCopia->nLinhas = recebido.nLinhas;
	resultadoCopia->perdeu = recebido.perdeu;
	resultadoCopia->pontuacao = recebido.pontuacao;
	resultadoCopia->proximaPeca = recebido.proximaPeca;
	resultadoCopia->tempoAguaComecaFluir = recebido.tempoAguaComecaFluir;
	resultadoCopia->tempoDecorrido = recebido.tempoDecorrido;
	resultadoCopia->tempoPararAgua = recebido.tempoPararAgua;
}

int _tmain(int argc, TCHAR* argv[]) {

	HANDLE hPipe, hThread, hEvent;
	ThreadsCliente threadsCliente;
	PartilhaJogoServidorMonitor partilhaJogo;
	int i, numClientes = 0;
	DWORD offset, nBytes;
    TCHAR buf[256];
    BOOL ret;
    DWORD n;

#ifdef UNICODE
	int setmodeSTDIN = _setmode(_fileno(stdin), _O_WTEXT);
	int setmodeSTDOUT = _setmode(_fileno(stdout), _O_WTEXT);
	int setmodeSTDERR = _setmode(_fileno(stderr), _O_WTEXT);
#endif
	
	threadsCliente.deveContinuar = &partilhaJogo.deveContinuar;
	threadsCliente.hMutex = CreateMutex(NULL, FALSE, NULL);

    if (!WaitNamedPipe(PIPE_NAME, NMPWAIT_WAIT_FOREVER)) {
        exit(-1);
    }

    hPipe = CreateFile(PIPE_NAME, GENERIC_READ, 0, NULL, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL, NULL);
    if (hPipe == NULL) {
        exit(-1);
    }

	DadosJogo recebido, dados;
    while (1) {
        ret = ReadFile(hPipe, &recebido, sizeof(DadosJogo), &n, NULL);
        
        if (!ret || !n) {
            break;
        }
		
		copiarTabuleiroJogoRecebido(recebido, &dados);

		for (int j = 0; j < dados.nLinhas; j++)
			for (int k = 0; k < dados.nColunas; k++)
				dados.mapaJogo[j][k] = recebido.mapaJogo[j][k];


        if (dados.aJogar)
            desenharMapaJogo(&dados);
    }
    CloseHandle(hPipe);

	return 0;

}