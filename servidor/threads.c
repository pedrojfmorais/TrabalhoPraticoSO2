#include "threads.h"
#include "pathfindAgua.h"
#include "comandosMonitor.h"
#include "inicializar.h"

BOOL WINAPI criaNamedPipeParaClientesTabuleiroJogo(LPVOID p) {

	PartilhaJogoServidorCliente* partilhaJogo = (PartilhaJogoServidorCliente*)p;

	for (int i = 0; i < N_JOGADORES; ++i) {

		HANDLE hPipe;
		HANDLE hEventTemp;

		hPipe = CreateNamedPipe(
			PIPE_NAME_TABULEIRO, 
			PIPE_ACCESS_OUTBOUND | FILE_FLAG_OVERLAPPED,
			PIPE_WAIT | PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE
			, N_JOGADORES, 
			sizeof(DadosJogo),
			sizeof(DadosJogo),
			NMPWAIT_USE_DEFAULT_WAIT, 
			NULL
		);
		if (hPipe == INVALID_HANDLE_VALUE) {
			exit(-1);
		}

		hEventTemp = CreateEvent(NULL, TRUE, FALSE, NULL);
		if (hEventTemp == NULL) {
			exit(-1);
		}

		ZeroMemory(&partilhaJogo->hPipes[i].overlap, sizeof(&partilhaJogo->hPipes[i].overlap));
		partilhaJogo->hPipes[i].hInstancia = hPipe;
		partilhaJogo->hPipes[i].overlap.hEvent = hEventTemp;
		partilhaJogo->hPipes[i].ativo = FALSE;
		partilhaJogo->hEvents[i] = hEventTemp;

		if (ConnectNamedPipe(hPipe, &partilhaJogo->hPipes[i].overlap)) {
			exit(-1);
		}
	}
	
	DWORD numClientes = 0, offset, nBytes;
	while (partilhaJogo->deveContinuar && numClientes < N_JOGADORES) {
		
		offset = WaitForMultipleObjects(N_JOGADORES, partilhaJogo->hEvents, FALSE, INFINITE);
		DWORD i = offset - WAIT_OBJECT_0;

		if (i >= 0 && i < N_JOGADORES) {
			if (GetOverlappedResult(partilhaJogo->hPipes[i].hInstancia, &partilhaJogo->hPipes[i].overlap, &nBytes, FALSE)) {
				ResetEvent(partilhaJogo->hEvents[i]);
				WaitForSingleObject(partilhaJogo->hReadWriteMutexAtualizacaoNoJogo, INFINITE);
				partilhaJogo->hPipes[i].ativo = TRUE;
				ReleaseMutex(partilhaJogo->hReadWriteMutexAtualizacaoNoJogo);
			}
			numClientes++;
		}
	}

	return TRUE;
}

BOOL WINAPI criaNamedPipeParaClientesMensagens(LPVOID p) {

	MensagensServidorCliente* mensagensServidorCliente = (MensagensServidorCliente*)p;

	for (int i = 0; i < N_JOGADORES; ++i) {

		HANDLE hPipe;
		HANDLE hEventTemp;

		hPipe = CreateNamedPipe(
			PIPE_NAME_MENSAGENS,
			PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
			PIPE_WAIT | PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE, 
			N_JOGADORES,
			sizeof(BufferCell),
			sizeof(BufferCell),
			NMPWAIT_USE_DEFAULT_WAIT,
			NULL
		);
		if (hPipe == INVALID_HANDLE_VALUE) {
			exit(-1);
		}

		hEventTemp = CreateEvent(NULL, TRUE, FALSE, NULL);
		if (hEventTemp == NULL) {
			exit(-1);
		}

		ZeroMemory(&mensagensServidorCliente->hPipes[i].overlap, sizeof(&mensagensServidorCliente->hPipes[i].overlap));
		mensagensServidorCliente->hPipes[i].hInstancia = hPipe;
		mensagensServidorCliente->hPipes[i].overlap.hEvent = hEventTemp;
		mensagensServidorCliente->hPipes[i].ativo = FALSE;
		mensagensServidorCliente->hEvents[i] = hEventTemp;

		if (ConnectNamedPipe(hPipe, &mensagensServidorCliente->hPipes[i].overlap)) {
			exit(-1);
		}
	}
	/*
	DWORD numClientes = 0, offset, nBytes;
	while (mensagensServidorCliente->deveContinuar && numClientes < N_JOGADORES) {

		offset = WaitForMultipleObjects(N_JOGADORES, mensagensServidorCliente->hEvents, FALSE, INFINITE);
		_tprintf(_T("AQUI1\t"));
		DWORD i = offset - WAIT_OBJECT_0;
		if (i >= 0 && i < N_JOGADORES) {

			if (GetOverlappedResult(mensagensServidorCliente->hPipes[i].hInstancia, &mensagensServidorCliente->hPipes[i].overlap, &nBytes, FALSE)) {

				//ResetEvent(mensagensServidorCliente->hEvents[i]);
				WaitForSingleObject(mensagensServidorCliente->hMutex, INFINITE);
				mensagensServidorCliente->hPipes[i].ativo = TRUE;
				ReleaseMutex(mensagensServidorCliente->hMutex);
			}
			numClientes++;
		}

	}*/

	return TRUE;
}

DWORD WINAPI clienteConectaNamedPipeTabuleiro(LPVOID p) {
	PartilhaJogoServidorCliente* dados = (PartilhaJogoServidorCliente*)p;
	TCHAR buf[TAM];
	TCHAR read[TAM];
	DWORD n;
	BOOL ret;

	do {

		WaitForSingleObject(dados->hEventAtualizacaoNoJogo, INFINITE);

		for (int i = 0; i < N_JOGADORES; ++i) {
			WaitForSingleObject(dados->hReadWriteMutexAtualizacaoNoJogo, INFINITE);
			if (dados->hPipes[i].ativo) {
				if (!WriteFile(dados->hPipes[i].hInstancia, dados->jogos[i], sizeof(DadosJogo), &n, NULL)) {
					exit(-1);
				}
			}
			ReleaseMutex(dados->hReadWriteMutexAtualizacaoNoJogo);
		}
	} while (dados->deveContinuar);

	// Desligar todas as instancias de named pipes
	for (int i = 0; i < N_JOGADORES; i++) {
		if (!DisconnectNamedPipe(dados->hPipes[i].hInstancia)) {
			_tprintf(_T("[ERRO] Desligar o pipe!"));
			return -1;
		}
	}

	return 0;
}

DWORD WINAPI clienteConectaNamedPipeMensagem(LPVOID p) {

	MensagensServidorCliente* dadosMensagens = (MensagensServidorCliente*)p;
	
	for (int i = 0; i < N_JOGADORES; ++i) {

		HANDLE hPipe;
		HANDLE hEventTemp;

		hPipe = CreateNamedPipe(
			PIPE_NAME_MENSAGENS,
			PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
			PIPE_WAIT | PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE,
			N_JOGADORES,
			sizeof(BufferCell),
			sizeof(BufferCell),
			NMPWAIT_USE_DEFAULT_WAIT,
			NULL
		);
		if (hPipe == INVALID_HANDLE_VALUE) {
			exit(-1);
		}

		hEventTemp = CreateEvent(NULL, TRUE, FALSE, NULL);
		if (hEventTemp == NULL) {
			exit(-1);
		}

		ZeroMemory(&dadosMensagens->hPipes[i].overlap, sizeof(&dadosMensagens->hPipes[i].overlap));
		dadosMensagens->hPipes[i].hInstancia = hPipe;
		dadosMensagens->hPipes[i].overlap.hEvent = hEventTemp;
		dadosMensagens->hPipes[i].ativo = TRUE;
		dadosMensagens->hEvents[i] = hEventTemp;

		if (ConnectNamedPipe(hPipe, &dadosMensagens->hPipes[i].overlap)) {
			exit(-1);
		}
	}

	TCHAR read[TAM];
	DWORD n;
	BOOL ret;
	
	do {
		_tprintf(_T("111111111111111111\n"));
		
		DWORD dwWait = WaitForMultipleObjects(
			N_JOGADORES,
			dadosMensagens->hEvents,
			FALSE,
			INFINITE
		);
		_tprintf(_T("22222222222222\n"));
		
		if (dwWait == WAIT_FAILED)
			_tprintf(TEXT("Error %d.\n"), GetLastError());
		if (dwWait != WAIT_TIMEOUT) {
			int i = dwWait - WAIT_OBJECT_0;
			if (i < 0 || i >(N_JOGADORES - 1))
			{
				_tprintf(TEXT("Index out of range.\n"));
				return 0;
			}

			ret = GetOverlappedResult(
				dadosMensagens->hPipes[i].hInstancia,
				&dadosMensagens->hPipes[i].overlap,
				&n,
				FALSE
			);

			ResetEvent(dadosMensagens->hEvents[i]);

			//109 Erro cliente desligou-se
			if (!ret && GetLastError() == 109) {
				continue;
			}
				
			else if (!ret) {
				_tprintf(TEXT("Error %d.\n"), GetLastError());
				return 0;
			}
			else if(dadosMensagens->hPipes[i].ativo) {
				
				int ret = ReadFile(dadosMensagens->hPipes[i].hInstancia, 
					&dadosMensagens->mensagens[i], 
					sizeof(BufferCell),
					&n,
					&dadosMensagens->hPipes[i].overlap);
				
				/*
				_tprintf(TEXT("[ESCRITOR] Frase: "));
				_fgetts(dadosMensagens->mensagens[i].mensagem, TAM, stdin);
				dadosMensagens->mensagens[i].mensagem[_tcslen(dadosMensagens->mensagens[i].mensagem) - 1] = '\0';
				_tprintf(_T("%d %s %d"), ret, dadosMensagens->mensagens[i].mensagem, sizeof(BufferCell));

				WriteFile(dadosMensagens->hPipes[i].hInstancia, &dadosMensagens->mensagens[i].mensagem, sizeof(TCHAR) * _tcslen(dadosMensagens->mensagens[i].mensagem), &n, &(dadosMensagens->hPipes[i].overlap));
				*/
				_tprintf(_T("33333333333333333333\n"));
				_tprintf(_T("%s\n"), dadosMensagens->mensagens[i].mensagem);
			}
			else {
				dadosMensagens->hPipes[i].ativo = TRUE;
				_tprintf(_T("44444444444444444444\n"));
			}
		}
	}while (1);
	
	dadosMensagens->deveContinuar = 1;

	// Desligar todas as instancias de named pipes
	for (int i = 0; i < N_JOGADORES; i++) {
		if (!DisconnectNamedPipe(dadosMensagens->hPipes[i].hInstancia)) {
			_tprintf(_T("[ERRO] Desligar o pipe!"));
			return -1;
		}
	}

	return 0;
}

BOOL WINAPI atualizaMapaJogoParaMonitor(LPVOID p) {

	PartilhaJogoServidorMonitor* partilhaJogo = (PartilhaJogoServidorMonitor*)p;

	while (partilhaJogo->deveContinuar) {

		WaitForSingleObject(partilhaJogo->hSemaforoEnviarAtualizacoesJogo, INFINITE);

		LARGE_INTEGER liDueTime;
		liDueTime.QuadPart = -5000000LL;

		SetEvent(partilhaJogo->hEventAtualizacaoNoJogo);
		if (!SetWaitableTimer(partilhaJogo->hTimer, &liDueTime, 0, NULL, NULL, 0)) {
			return FALSE;
		}
		WaitForSingleObject(partilhaJogo->hTimer, INFINITE);
		//Sleep(500);
		ResetEvent(partilhaJogo->hEventAtualizacaoNoJogo);

	}
	return TRUE;
}


BOOL WINAPI decorrerJogo(LPVOID p) {
	PartilhaJogoServidorMonitor* partilhaJogo = (PartilhaJogoServidorMonitor*)p;
	BOOL atualizouAposPausarJogo = FALSE;

	while (partilhaJogo->deveContinuar) {

		WaitForSingleObject(partilhaJogo->hEventJogosDecorrer, INFINITE);

		//quando todos os jogos estão em pausa, serão atualizados os ecrãs dos monitores uma vez de 
		// forma a mostrar ao utilizador a mensagem que os jogos estão em pausa
		BOOL sofreuAlteracoes = FALSE;

		WaitForSingleObject(partilhaJogo->hReadWriteMutexAtualizacaoNoJogo, INFINITE);

		for (DWORD i = 0; i < N_JOGADORES; i++)
		{

			DadosJogo* dadosJogo = partilhaJogo->jogos[i];

			if (!dadosJogo->aJogar || dadosJogo->jogoPausado)
				continue;

			if (dadosJogo->tempoAguaComecaFluir < dadosJogo->tempoDecorrido) {

				if (dadosJogo->tempoPararAgua > 0){
						dadosJogo->tempoPararAgua--;
				
				}else if ((dadosJogo->tempoDecorrido - dadosJogo->tempoAguaComecaFluir) % N_SEGUNDOS_AVANCO_AGUA == 0) {
					//água avança a cada N_SEGUNDOS_AVANCO_AGUA segundos
					if (dadosJogo->ganhou || dadosJogo->perdeu)
						dadosJogo->aJogar = FALSE;

					if (!pathfindAguaJogo(dadosJogo))
						dadosJogo->perdeu = TRUE;
					else
						dadosJogo->pontuacao += 15;
				}
			}

			dadosJogo->tempoDecorrido += 1;
			sofreuAlteracoes = TRUE;
		}

		ReleaseMutex(partilhaJogo->hReadWriteMutexAtualizacaoNoJogo);

		if (sofreuAlteracoes) {
			atualizouAposPausarJogo = FALSE;
			ReleaseSemaphore(partilhaJogo->hSemaforoEnviarAtualizacoesJogo, 1, NULL);
		}

		BOOL todosJogosParados = TRUE;

		for (DWORD i = 0; i < N_JOGADORES; i++)
			if (partilhaJogo->jogos[i]->aJogar)
				todosJogosParados = FALSE;
		
		if (todosJogosParados)
			ResetEvent(partilhaJogo->hEventJogosDecorrer);

		if (!todosJogosParados && !atualizouAposPausarJogo && !sofreuAlteracoes) {
			atualizouAposPausarJogo = TRUE;
			ReleaseSemaphore(partilhaJogo->hSemaforoEnviarAtualizacoesJogo, 1, NULL);
		}

		Sleep(1000);
	}
	return TRUE;
}

BOOL WINAPI recebeMensagemMonitor(LPVOID p) {
	PartilhaJogoServidorMonitor* partilhaJogo = (PartilhaJogoServidorMonitor*)p;

	while (partilhaJogo->deveContinuar) {

		WaitForSingleObject(partilhaJogo->hSemaforoLeituraBufferCircularMonitorParaServidor, INFINITE);
		WaitForSingleObject(partilhaJogo->hMutexBufferCircularMonitorParaServidor, INFINITE);

		WaitForSingleObject(partilhaJogo->hReadWriteMutexAtualizacaoNoJogo, INFINITE);

		trataMensagem(partilhaJogo, partilhaJogo->bufferCircularMonitorParaServidor->buffer[partilhaJogo->bufferCircularMonitorParaServidor->rP].mensagem);
		
		ReleaseMutex(partilhaJogo->hReadWriteMutexAtualizacaoNoJogo);

		(partilhaJogo->bufferCircularMonitorParaServidor->rP) += 1;

		if (partilhaJogo->bufferCircularMonitorParaServidor->rP == BUFFER_SIZE)
			partilhaJogo->bufferCircularMonitorParaServidor->rP = 0;

		ReleaseMutex(partilhaJogo->hMutexBufferCircularMonitorParaServidor);
		ReleaseSemaphore(partilhaJogo->hSemaforoEscritaBufferCircularMonitorParaServidor, 1, NULL);

	}
	return TRUE;
}


DWORD WINAPI leMensagemUtilizador(LPVOID p) {
	PartilhaJogoServidorMonitor* partilhaJogo = (PartilhaJogoServidorMonitor*)p;
	BufferCell cell;

	TCHAR* command;
	TCHAR* next_token = NULL;

	while (partilhaJogo->deveContinuar) {

		_tprintf(_T("Insira um comando: "));
		_getts_s(cell.mensagem, TAM);

		if (_tcslen(cell.mensagem) == 0)
			continue;

		command = _tcstok_s(cell.mensagem, _T(" "), &next_token);

		WaitForSingleObject(partilhaJogo->hReadWriteMutexAtualizacaoNoJogo, INFINITE);

		if (_tcscmp(command, _T("exit")) == 0) {
			SetEvent(partilhaJogo->hEventFecharTudo);
		}
		else if (_tcscmp(command, _T("start")) == 0) {
			for (DWORD i = 0; i < N_JOGADORES; i++)
			{
				if (!partilhaJogo->jogos[i]->aJogar) {
					inicializaJogo(partilhaJogo->jogos[i], partilhaJogo->definicoesJogo);
					
					if (i == 0) {
						SetEvent(partilhaJogo->hEventJogosDecorrer);
						ReleaseSemaphore(partilhaJogo->hSemaforoEnviarAtualizacoesJogo, 1, NULL);
					}
					break;
				}
			}
		}
		else if (_tcscmp(command, _T("pausarJogo")) == 0) {

			command = _tcstok_s(NULL, _T(" "), &next_token);
			
			if (command == NULL) {
				for (DWORD i = 0; i < N_JOGADORES; i++)
				{
					if (partilhaJogo->jogos[i]->aJogar) {
						partilhaJogo->jogos[i]->jogoPausado = !partilhaJogo->jogos[i]->jogoPausado;
					}
				}
			}
			else {
				DWORD nJogador = (DWORD)_tcstod(command, _T('\0'));
				if(nJogador <= N_JOGADORES && nJogador > 0 && partilhaJogo->jogos[nJogador-1]->aJogar)
					partilhaJogo->jogos[nJogador - 1]->jogoPausado = !partilhaJogo->jogos[nJogador - 1]->jogoPausado;
			}
		}
		else if (_tcscmp(command, _T("listar")) == 0) {

			command = _tcstok_s(NULL, _T(" "), &next_token);

			if (command == NULL) {
				for (DWORD i = 0; i < N_JOGADORES; i++)
				{
					if (partilhaJogo->jogos[i]->aJogar) {
						_tprintf(_T("Jogador %d\nPontução: %d\n\n"), partilhaJogo->jogos[i]->idJogador, partilhaJogo->jogos[i]->pontuacao);
					}
				}
			}
			else {
				DWORD nJogador = (DWORD)_tcstod(command, _T('\0'));
				if (nJogador <= N_JOGADORES && nJogador > 0 && partilhaJogo->jogos[nJogador - 1]->aJogar)
					_tprintf(_T("Jogador %d\nPontução: %d\n\n"), partilhaJogo->jogos[nJogador - 1]->idJogador, 
						partilhaJogo->jogos[nJogador - 1]->pontuacao);
			}
		}
		// TESTE COMDANDOS CLIENTE
		else if (_tcscmp(command, _T("coloca")) == 0) {
			TCHAR* _linha = _tcstok_s(NULL, _T(" "), &next_token);
			TCHAR* _coluna = _tcstok_s(NULL, _T(" "), &next_token);

			DWORD linha = (DWORD)_tcstod(_linha, _T('\0'));
			DWORD coluna = (DWORD)_tcstod(_coluna, _T('\0'));

			colocaPeca(partilhaJogo->jogos[0], linha, coluna);

		} else if (_tcscmp(command, _T("limpa")) == 0) {
			TCHAR* _linha = _tcstok_s(NULL, _T(" "), &next_token);
			TCHAR* _coluna = _tcstok_s(NULL, _T(" "), &next_token);

			DWORD linha = (DWORD)_tcstod(_linha, _T('\0'));
			DWORD coluna = (DWORD)_tcstod(_coluna, _T('\0'));

			limpaPeca(partilhaJogo->jogos[0], linha, coluna);
		}
		// FIM TESTE COMDANDOS CLIENTE

		ReleaseMutex(partilhaJogo->hReadWriteMutexAtualizacaoNoJogo);
	}
	return TRUE;
}

DWORD WINAPI acabarThreads(LPVOID p) {

	ThreadsServidor* threadsServidor = (ThreadsServidor*)p;

	WaitForSingleObject(threadsServidor->hEventFecharTudo, INFINITE);

	//*threadsServidor->deveContinuar = 0;

	for (DWORD i = 0; i < N_THREADS_SERVIDOR; i++)
		TerminateThread(threadsServidor->hThreads[i], 0);

	return 0;
}