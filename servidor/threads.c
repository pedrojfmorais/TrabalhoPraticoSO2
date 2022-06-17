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
			sizeof(PartilhaJogoServidorCliente),
			sizeof(PartilhaJogoServidorCliente),
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
			PIPE_ACCESS_OUTBOUND | FILE_FLAG_OVERLAPPED,
			PIPE_WAIT | PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE
			, N_JOGADORES,
			sizeof(MensagensServidorCliente),
			sizeof(MensagensServidorCliente),
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

	DWORD numClientes = 0, offset, nBytes;
	while (mensagensServidorCliente->deveContinuar && numClientes < N_JOGADORES) {

		offset = WaitForMultipleObjects(N_JOGADORES, mensagensServidorCliente->hEvents, FALSE, INFINITE);
		DWORD i = offset - WAIT_OBJECT_0;
		if (i >= 0 && i < N_JOGADORES) {

			if (GetOverlappedResult(mensagensServidorCliente->hPipes[i].hInstancia, &mensagensServidorCliente->hPipes[i].overlap, &nBytes, FALSE)) {

				ResetEvent(mensagensServidorCliente->hEvents[i]);
				WaitForSingleObject(mensagensServidorCliente->hMutex, INFINITE);
				mensagensServidorCliente->hPipes[i].ativo = TRUE;
				ReleaseMutex(mensagensServidorCliente->hMutex);
			}
			numClientes++;
		}
	}

	return TRUE;
}

DWORD WINAPI clienteConectaNamedPipeTabuleiro(LPVOID p) {
	PartilhaJogoServidorCliente* dados = (PartilhaJogoServidorCliente*)p;
	TCHAR buf[TAM];
	TCHAR read[TAM];
	DWORD n;
	BOOL ret;

	do {
		Sleep(5000);
		for (int i = 0; i < N_JOGADORES; ++i) {
			WaitForSingleObject(dados->hReadWriteMutexAtualizacaoNoJogo, INFINITE);
			if (dados->hPipes[i].ativo) {
				if (!WriteFile(dados->hPipes[i].hInstancia, dados->jogos[i], sizeof(DadosJogo), &n, NULL)) {
					exit(-1);
				}
			}
			ReleaseMutex(dados->hReadWriteMutexAtualizacaoNoJogo);
		}
	} while (_tcscmp(buf, TEXT("fim")));

	dados->deveContinuar = 1;

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
	
	TCHAR buf[TAM] = _T("ASD_123");
	TCHAR read[TAM];
	DWORD n;
	BOOL ret;

	do {
		Sleep(5000);
		for (int i = 0; i < N_JOGADORES; ++i) {
			WaitForSingleObject(dadosMensagens->hMutex ,INFINITE);

			//_tprintf(TEXT("[ESCRITOR] Frase: "));
			//_getts_s(dadosMensagens->mensagens[i].mensagem, TAM);
			//dadosMensagens->mensagens[i].mensagem[_tcslen(dadosMensagens->mensagens[i].mensagem) - 1] = '\0';
			

			CopyMemory(dadosMensagens->mensagens[i].mensagem, buf, _tcslen(buf) * sizeof(TCHAR));
			//dadosMensagens->mensagens[i].mensagem[_tcslen(dadosMensagens->mensagens[i].mensagem) - 1] = '\0';

			_tprintf(_T("-%s-"), dadosMensagens->mensagens[i].mensagem);

			if (dadosMensagens->hPipes[i].ativo) {
				if (!WriteFile(dadosMensagens->hPipes[i].hInstancia, &dadosMensagens->mensagens[i], sizeof(BufferCell), &n, NULL)) {
					exit(-1);
				}
			}
			ReleaseMutex(dadosMensagens->hMutex);
		}
	} while (_tcscmp(buf, TEXT("fim")));

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

		//quando todos os jogos est�o em pausa, ser�o atualizados os ecr�s dos monitores uma vez de 
		// forma a mostrar ao utilizador a mensagem que os jogos est�o em pausa
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
					//�gua avan�a a cada N_SEGUNDOS_AVANCO_AGUA segundos
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
						/*
						//debug
						//jogo com solu��o
						for (DWORD j = 0; j < partilhaJogo->jogos[i]->nLinhas; j++)
							for (DWORD k = 0; k < partilhaJogo->jogos[i]->nColunas; k++)
								partilhaJogo->jogos[i]->mapaJogo[j][k] = tuboVazio;

						partilhaJogo->jogos[i]->mapaJogo[0][0] = tuboOrigemAgua * 10;

						partilhaJogo->jogos[i]->coordenadasOrigemAgua[0] = 0;
						partilhaJogo->jogos[i]->coordenadasOrigemAgua[1] = 0;

						partilhaJogo->jogos[i]->coordenadaAtualAgua[0] = 0;
						partilhaJogo->jogos[i]->coordenadaAtualAgua[1] = 0;

						partilhaJogo->jogos[i]->mapaJogo[0][1] = tuboHorizontal;
						partilhaJogo->jogos[i]->mapaJogo[0][2] = tuboHorizontal;
						partilhaJogo->jogos[i]->mapaJogo[0][3] = tuboCurvaEsquerdaBaixo;
						partilhaJogo->jogos[i]->mapaJogo[1][3] = tuboVertical;
						partilhaJogo->jogos[i]->mapaJogo[2][3] = tuboVertical;
						partilhaJogo->jogos[i]->mapaJogo[3][3] = tuboCurvaEsquerdaCima;
						partilhaJogo->jogos[i]->mapaJogo[3][2] = tuboCurvaDireitaBaixo;
						partilhaJogo->jogos[i]->mapaJogo[4][2] = tuboCurvaDireitaCima;
						partilhaJogo->jogos[i]->mapaJogo[4][3] = tuboHorizontal;
						partilhaJogo->jogos[i]->mapaJogo[4][4] = tuboDestinoAgua;
						//fim debug
						*/
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
						_tprintf(_T("Jogador %d\nPontu��o: %d\n\n"), partilhaJogo->jogos[i]->idJogador, partilhaJogo->jogos[i]->pontuacao);
					}
				}
			}
			else {
				DWORD nJogador = (DWORD)_tcstod(command, _T('\0'));
				if (nJogador <= N_JOGADORES && nJogador > 0 && partilhaJogo->jogos[nJogador - 1]->aJogar)
					_tprintf(_T("Jogador %d\nPontu��o: %d\n\n"), partilhaJogo->jogos[nJogador - 1]->idJogador, 
						partilhaJogo->jogos[nJogador - 1]->pontuacao);
			}
		}

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