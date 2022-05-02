#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>

#define TAM 200
#define caminhoRegistry _T("software\\so2\\tp\\")



typedef struct {
	DWORD nLinhas;
	DWORD nColunas;
	DWORD tempoAguaComecaFluir;
	DWORD coordenadasOrigemAgua[2];
	DWORD coordenadasDestinoAgua[2];
} DadosMapaJogo;

DWORD getRandomNumberBetweenMaxAndMin(DWORD min, DWORD max) {
	return (rand() % (max - min)) + min;
}

void inicializaServidor(int argc, TCHAR* argv[], DadosMapaJogo *dadosMapaJogo) {

	TCHAR nomeChaves[3][TAM] = { _T("nLinhas"), _T("nColunas"), _T("tempoAguaComecaFluir") };
	DWORD val[3] = { 0 };

	if (argc == 1) {
		//L� no registry

		if (!lerDoRegistryDadosMapaJogo(nomeChaves, val))
			exit(1);

	}else if(argc == 4) {
		//Guarda no registry

		for (DWORD i = 0; i < 3; i++)
			val[i] = wcstod(argv[i+1], _T('\0'));

		for (DWORD i = 0; i < 2; i++)
			if (val[i] > 20)
				val[i] = 20;
			else if (val[i] < 5)
				val[i] = 5;

		if(!guardaNoRegistryDadosMapaJogo(nomeChaves, val))
			_tprintf(_T("N�o foi possivel guardar os valores no Registry!"));

	}
	else {
		_tprintf(_T("N�mero de argumentos inv�lido!"));
		exit(1);
	}

	dadosMapaJogo->nLinhas = val[0];
	dadosMapaJogo->nColunas = val[1];
	dadosMapaJogo->tempoAguaComecaFluir = val[2];

	srand((unsigned int)time(NULL));

	dadosMapaJogo->coordenadasOrigemAgua[0] = getRandomNumberBetweenMaxAndMin(0, dadosMapaJogo->nLinhas);
	dadosMapaJogo->coordenadasOrigemAgua[1] = 0;

	dadosMapaJogo->coordenadasDestinoAgua[0] = getRandomNumberBetweenMaxAndMin(0, dadosMapaJogo->nLinhas);
	dadosMapaJogo->coordenadasDestinoAgua[1] = dadosMapaJogo->nColunas-1;

	//Verifica se a origem e destino da �gua est�o em quadrantes diagonais opostos
	if (dadosMapaJogo->coordenadasOrigemAgua[0] > (dadosMapaJogo->nLinhas / 2) && dadosMapaJogo->coordenadasDestinoAgua[0] > (dadosMapaJogo->nLinhas / 2))
		dadosMapaJogo->coordenadasDestinoAgua[0] -= dadosMapaJogo->nLinhas / 2;
		
	else if (dadosMapaJogo->coordenadasOrigemAgua[0] < (dadosMapaJogo->nLinhas / 2) && dadosMapaJogo->coordenadasDestinoAgua[0] < (dadosMapaJogo->nLinhas / 2))
		dadosMapaJogo->coordenadasDestinoAgua[0] += dadosMapaJogo->nLinhas / 2;
		
}

BOOL lerDoRegistryDadosMapaJogo(TCHAR nomeChaves[3][TAM], DWORD val[3]) {

	HKEY chave;
	LSTATUS returnCreate, returnQueryValue;
	TCHAR resposta[TAM];
	DWORD sizeVal;

	returnCreate = RegCreateKeyEx(
		HKEY_CURRENT_USER,
		caminhoRegistry,
		0,
		NULL,
		REG_OPTION_VOLATILE,
		KEY_ALL_ACCESS,
		NULL,
		&chave,
		&resposta
	);

	if (returnCreate != ERROR_SUCCESS) {
		DWORD error = GetLastError();
		_tprintf(_T("Erro a criar/abrir o registry! [%d]\n"), error);
		return FALSE;
	}

	for (DWORD i = 0; i < 3; i++)
	{
		returnQueryValue = RegQueryValueEx(
			chave,
			nomeChaves[i],
			NULL,
			NULL,
			(LPBYTE) &val[i],
			&sizeVal
		);

		if (returnQueryValue != ERROR_SUCCESS) {
			DWORD error = GetLastError();
			_tprintf(_T("Erro a ler do Registry [%d]\n"), error);
			RegCloseKey(chave);
			return FALSE;
		}
	}

	RegCloseKey(chave);
	return TRUE;
}
BOOL guardaNoRegistryDadosMapaJogo(TCHAR nomeChaves[3][TAM], DWORD val[3]) {

	HKEY chave;
	LSTATUS returnCreate, returnSetValue;
	TCHAR resposta[TAM];

	returnCreate = RegCreateKeyEx(
		HKEY_CURRENT_USER,
		caminhoRegistry,
		0,
		NULL,
		REG_OPTION_VOLATILE,
		KEY_ALL_ACCESS,
		NULL,
		&chave,
		&resposta
	);

	if (returnCreate != ERROR_SUCCESS) {
		DWORD error = GetLastError();
		_tprintf(_T("Erro a criar/abrir o registry! [%d]\n"), error);
		return FALSE;
	}

	for (DWORD i = 0; i < 3; i++)
	{
		returnSetValue = RegSetValueEx(
			chave,
			nomeChaves[i],
			0,
			REG_DWORD,
			(LPBYTE) &val[i],
			(DWORD) sizeof(DWORD)
		);

		if (returnSetValue != ERROR_SUCCESS) {
			DWORD error = GetLastError();
			_tprintf(_T("Erro a guardar o valor! [%d]\n"), error);
			RegCloseKey(chave);
			return FALSE;
		}
	}
	
	RegCloseKey(chave);
	return TRUE;
}

int _tmain(int argc, TCHAR* argv[]) {

#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
#endif 

	DadosMapaJogo dadosMapaJogo;

	inicializaServidor(argc, argv, &dadosMapaJogo);

	_tprintf(_T("%d %d %d \n"), dadosMapaJogo.nLinhas, dadosMapaJogo.nColunas, dadosMapaJogo.tempoAguaComecaFluir);
	_tprintf(_T("%d %d \n"), dadosMapaJogo.coordenadasOrigemAgua[0], dadosMapaJogo.coordenadasOrigemAgua[1]);
	_tprintf(_T("%d %d \n"), dadosMapaJogo.coordenadasDestinoAgua[0], dadosMapaJogo.coordenadasDestinoAgua[1]);

	return 0;
}