#pragma once

#include "..\utils_so2_tp\utils_so2_tp.h"

DWORD getRandomProximaPeca();
DWORD getProximaPecaSequencia(DWORD pecaAnterior);
void alternaModoAleatorioPeca(DefinicoesJogo* definicoesJogo);
BOOL colocaParede_x_y(DWORD x, DWORD y, DadosJogo* jogo);
void setTempoPararAgua(DWORD segundos, PartilhaJogo* partilhaJogo);