#include "registry.h"
#include <ctype.h>

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
			(LPBYTE)&val[i],
			&sizeVal
		);

		if (returnQueryValue != ERROR_SUCCESS || !isdigit(val[i])) {
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
			(LPBYTE)&val[i],
			(DWORD)sizeof(DWORD)
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
