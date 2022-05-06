#pragma once

#include "..\utils_so2_tp\utils_so2_tp.h"

DWORD getRandomProximaPeca();
DWORD getProximaPecaSequencia(DWORD pecaAnterior);
void alternaModoAleatorioPeca(DadosJogo* dadosJogo);
BOOL colocaParede_x_y(DWORD x, DWORD y, DadosJogo* jogo);
void setTempoPararAgua(DWORD segundos, DadosJogo* dadosJogo);
void trataMensagem(PartilhaJogo* partilhaJogo, TCHAR* comando);