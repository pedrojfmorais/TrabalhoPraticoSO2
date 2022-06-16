#pragma once

#include"..\utils_so2_tp\utils_so2_tp.h"

void inicializaJogo(DadosJogo* jogo, DefinicoesJogo definicoesJogo);
void inicializaServidor(int argc, TCHAR* argv[], PartilhaJogoServidorMonitor* partilhaJogoMonitor, PartilhaJogoServidorCliente* partilhaJogoCliente);