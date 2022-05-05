#include "printPecas.h"

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