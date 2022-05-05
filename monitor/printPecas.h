#pragma once

#include "..\\utils_so2_tp\utils_so2_tp.h"

// Retirado da internet "https://www.codewithc.com/change-text-color-in-codeblocks-console-window/"
void SetColor(int ForgC);

void printTuboOrigemAgua(int i, int j, BOOL temAgua);
void printTuboDestinoAgua(int i, int j, BOOL temAgua);
void printTuboHorizontal(int i, int j, BOOL temAgua);
void printTuboVertical(int i, int j, BOOL temAgua);
void printTuboCurvaEsquerdaCima(int i, int j, BOOL temAgua);
void printTuboCurvaEsquerdaBaixo(int i, int j, BOOL temAgua);
void printTuboCurvaDireitaCima(int i, int j, BOOL temAgua);
void printTuboCurvaDireitaBaixo(int i, int j, BOOL temAgua);
void printPeca(DWORD peca, int i, int j, BOOL temAgua);