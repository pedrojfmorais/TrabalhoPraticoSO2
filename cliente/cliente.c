// cliente.cpp : Define o ponto de entrada para o aplicativo.
//

#include "framework.h"
#include "cliente.h"
#include <windowsx.h>
#include "..\\utils_so2_tp\utils_so2_tp.h"

#define MAX_LOADSTRING 100

// TESTES

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

HBITMAP getPeca(int num) {

    TCHAR pasta[TAM];

    switch (num)
    {
    case tuboVazio:
        return (HBITMAP)LoadImage(NULL, TEXT("images/vazio.bmp"), IMAGE_BITMAP, TAM_BITMAP, TAM_BITMAP, LR_LOADFROMFILE);

    case tuboBloqueado:
        return (HBITMAP)LoadImage(NULL, TEXT("images/parede.bmp"), IMAGE_BITMAP, TAM_BITMAP, TAM_BITMAP, LR_LOADFROMFILE);

    case tuboOrigemAgua:
        return (HBITMAP)LoadImage(NULL, TEXT("images/comecoAgua.bmp"), IMAGE_BITMAP, TAM_BITMAP, TAM_BITMAP, LR_LOADFROMFILE);

    case tuboOrigemAgua * 10:
        return (HBITMAP)LoadImage(NULL, TEXT("images/comecoAgua_comAgua.bmp"), IMAGE_BITMAP, TAM_BITMAP, TAM_BITMAP, LR_LOADFROMFILE);

    case tuboDestinoAgua:
        return (HBITMAP)LoadImage(NULL, TEXT("images/fimAgua.bmp"), IMAGE_BITMAP, TAM_BITMAP, TAM_BITMAP, LR_LOADFROMFILE);

    case tuboDestinoAgua * 10:
        return (HBITMAP)LoadImage(NULL, TEXT("images/fimAgua_comAgua.bmp"), IMAGE_BITMAP, TAM_BITMAP, TAM_BITMAP, LR_LOADFROMFILE);

    case tuboHorizontal:
        return (HBITMAP)LoadImage(NULL, TEXT("images/tuboHorizontal.bmp"), IMAGE_BITMAP, TAM_BITMAP, TAM_BITMAP, LR_LOADFROMFILE);

    case tuboHorizontal * 10:
        return (HBITMAP)LoadImage(NULL, TEXT("images/tuboHorizontal_comAgua.bmp"), IMAGE_BITMAP, TAM_BITMAP, TAM_BITMAP, LR_LOADFROMFILE);

    case tuboVertical:
        return (HBITMAP)LoadImage(NULL, TEXT("images/tuboVertical.bmp"), IMAGE_BITMAP, TAM_BITMAP, TAM_BITMAP, LR_LOADFROMFILE);

    case tuboVertical * 10:
        return (HBITMAP)LoadImage(NULL, TEXT("images/tuboVertical_comAgua.bmp"), IMAGE_BITMAP, TAM_BITMAP, TAM_BITMAP, LR_LOADFROMFILE);

    case tuboCurvaEsquerdaCima:
        return (HBITMAP)LoadImage(NULL, TEXT("images/esquerdaCima.bmp"), IMAGE_BITMAP, TAM_BITMAP, TAM_BITMAP, LR_LOADFROMFILE);

    case tuboCurvaEsquerdaCima * 10:
        return (HBITMAP)LoadImage(NULL, TEXT("images/esquerdaCima_comAgua.bmp"), IMAGE_BITMAP, TAM_BITMAP, TAM_BITMAP, LR_LOADFROMFILE);

    case tuboCurvaEsquerdaBaixo:
        return (HBITMAP)LoadImage(NULL, TEXT("images/esquerdaBaixo.bmp"), IMAGE_BITMAP, TAM_BITMAP, TAM_BITMAP, LR_LOADFROMFILE);

    case tuboCurvaEsquerdaBaixo * 10:
        return (HBITMAP)LoadImage(NULL, TEXT("images/esquerdaBaixo_comAgua.bmp"), IMAGE_BITMAP, TAM_BITMAP, TAM_BITMAP, LR_LOADFROMFILE);

    case tuboCurvaDireitaCima:
        return (HBITMAP)LoadImage(NULL, TEXT("images/direitaCima.bmp"), IMAGE_BITMAP, TAM_BITMAP, TAM_BITMAP, LR_LOADFROMFILE);

    case tuboCurvaDireitaCima * 10:
        return (HBITMAP)LoadImage(NULL, TEXT("images/direitaCima_comAgua.bmp"), IMAGE_BITMAP, TAM_BITMAP, TAM_BITMAP, LR_LOADFROMFILE);

    case tuboCurvaDireitaBaixo:
        return (HBITMAP)LoadImage(NULL, TEXT("images/direitaBaixo.bmp"), IMAGE_BITMAP, TAM_BITMAP, TAM_BITMAP, LR_LOADFROMFILE);

    case tuboCurvaDireitaBaixo * 10:
        return (HBITMAP)LoadImage(NULL, TEXT("images/direitaBaixo_comAgua.bmp"), IMAGE_BITMAP, TAM_BITMAP, TAM_BITMAP, LR_LOADFROMFILE);

    default:
        return (HBITMAP)LoadImage(NULL, TEXT("images/vazio.bmp"), IMAGE_BITMAP, TAM_BITMAP, TAM_BITMAP, LR_LOADFROMFILE);
    }
}

DWORD WINAPI recebeTabuleiroServidor(LPVOID p) {

    DadosThreadDesenharTabuleiro* dados = (DadosThreadDesenharTabuleiro*)p;
    
    DadosJogo recebido;

    HANDLE hPipeTabuleiro;
    BOOL ret;
    DWORD n;

    RECT rect;
    HDC hdc;
    PAINTSTRUCT ps;

    if (!WaitNamedPipe(PIPE_NAME_TABULEIRO, NMPWAIT_WAIT_FOREVER)) {
        exit(-1);
    }

    hPipeTabuleiro = CreateFile(PIPE_NAME_TABULEIRO, GENERIC_READ, 0, NULL, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL, NULL);
    if (hPipeTabuleiro == NULL) {
        exit(-1);
    }

    while (1) {
        Sleep(1000);
        
        ret = ReadFile(hPipeTabuleiro, &recebido, sizeof(DadosJogo), &n, NULL);
        
        if (!ret || !n) {
            break;
        }

        copiarTabuleiroJogoRecebido(recebido, &dados->dadosJogo);
        		
        for (int j = 0; j < dados->dadosJogo.nLinhas; j++)
            for (int k = 0; k < dados->dadosJogo.nColunas; k++)
                dados->dadosJogo.mapaJogo[j][k] = recebido.mapaJogo[j][k];
        
        WaitForSingleObject(dados->hMutex, INFINITE);

        if (*dados->memDC != NULL) {
            FillRect(*dados->memDC, &rect, CreateSolidBrush(RGB(220, 220, 220)));
            for (int j = 0; j < dados->dadosJogo.nLinhas; j++) {
                for (int k = 0; k < dados->dadosJogo.nColunas; k++) {

                    SelectObject(dados->bmpDC, getPeca(dados->dadosJogo.mapaJogo[j][k]));

                    GetClientRect(dados->hWnd, &rect);
                    *dados->xBitmap = rect.left + (k * TAM_BITMAP);
                    *dados->yBitmap = rect.top + (j * TAM_BITMAP);
                    BitBlt(*dados->memDC, *dados->xBitmap, *dados->yBitmap,
                        dados->bmp.bmWidth, dados->bmp.bmHeight, dados->bmpDC, 0, 0, SRCCOPY);
                }
            }

            *dados->xBitmap = rect.left;
            *dados->yBitmap = rect.top + ((dados->dadosJogo.nLinhas + 1) * TAM_BITMAP);

            SelectObject(dados->bmpDC, getPeca(dados->dadosJogo.proximaPeca));
            GetClientRect(dados->hWnd, &rect);
            BitBlt(*dados->memDC, *dados->xBitmap, *dados->yBitmap,
                dados->bmp.bmWidth, dados->bmp.bmHeight, dados->bmpDC, 0, 0, SRCCOPY);

        }
        
        ReleaseMutex(dados->hMutex);

        InvalidateRect(dados->hWnd, NULL, TRUE);
        Sleep(30);
    }        

    CloseHandle(hPipeTabuleiro);
    return 0;
}

void colocaPeca(DWORD x, DWORD y, DadosThreadDesenharTabuleiro* dados) {

    //Verificações realizadas no servidor

    x /= TAM_BITMAP;
    y /= TAM_BITMAP;

    if (y > dados->dadosJogo.nLinhas-1 || x > dados->dadosJogo.nColunas-1)
        return;

    //Teste - Mostrar coordenadas no tabuleiro

    HDC dc = GetDC(dados->hWnd);
    RECT rc;
    GetClientRect(dados->hWnd, &rc);
    TCHAR coords[TAM];
    swprintf_s(coords, TAM, _T("x:%d,y:%d"), x, y);
    DrawText(dc, coords, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    ReleaseDC(dados->hWnd, dc);
    //Fim Teste

    //TODO: envia meter peca nas coordenadas x,y para o servidor pelo named pipe
        
}

void removePeca(DWORD x, DWORD y, DadosThreadDesenharTabuleiro* dados) {
    
    //Verificações realizadas no servidor

    x /= TAM_BITMAP;
    y /= TAM_BITMAP;

    if (y > dados->dadosJogo.nLinhas - 1 || x > dados->dadosJogo.nColunas - 1)
        return;

    //Teste - Mostrar coordenadas no tabuleiro

    HDC dc = GetDC(dados->hWnd);
    RECT rc;
    GetClientRect(dados->hWnd, &rc);
    TCHAR coords[TAM];
    swprintf_s(coords, TAM, _T("x:%d,y:%d"), x, y);
    DrawText(dc, coords, -1, &rc, DT_CENTER | DT_VCENTER | DT_PATH_ELLIPSIS);
    ReleaseDC(dados->hWnd, dc);
    //Fim Teste

    //TODO: envia tirar peca nas coordenadas x,y para o servidor pelo named pipe

}

void pararAgua(DWORD x, DWORD y, DadosThreadDesenharTabuleiro* dados) {

    //Verificações realizadas no servidor
 
    x /= TAM_BITMAP;
    y /= TAM_BITMAP;

    if (y > dados->dadosJogo.nLinhas - 1 || x > dados->dadosJogo.nColunas - 1)
        return;

    //se o rato não estiver na coordenada atual da água não a para
    if (dados->dadosJogo.coordenadaAtualAgua[0] != y || dados->dadosJogo.coordenadaAtualAgua[1] != x)
        return;

    //Teste - Mostrar coordenadas no tabuleiro
    
    HDC dc = GetDC(dados->hWnd);
    RECT rc;
    GetClientRect(dados->hWnd, &rc);
    TCHAR coords[TAM];
    swprintf_s(coords, sizeof(TCHAR) * _tcslen(_T("ASDASD")), _T("ASDASD"), x, y);
    DrawText(dc, coords, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    ReleaseDC(dados->hWnd, dc);
    //Fim Teste

    //TODO: envia tirar peca nas coordenadas x,y para o servidor pelo named pipe

}


// Variáveis Globais:
HINSTANCE hInst;                                // instância atual
WCHAR szTitle[MAX_LOADSTRING];                  // O texto da barra de título
WCHAR szWindowClass[MAX_LOADSTRING];            // o nome da classe da janela principal

// Declarações de encaminhamento de funções incluídas nesse módulo de código:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Coloque o código aqui.

    // Inicializar cadeias de caracteres globais
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CLIENTE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Realize a inicialização do aplicativo:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CLIENTE));

    MSG msg;

    // Loop de mensagem principal:
    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNÇÃO: MyRegisterClass()
//
//  FINALIDADE: Registra a classe de janela.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CLIENTE));
    wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_CLIENTE);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNÇÃO: InitInstance(HINSTANCE, int)
//
//   FINALIDADE: Salva o identificador de instância e cria a janela principal
//
//   COMENTÁRIOS:
//
//        Nesta função, o identificador de instâncias é salvo em uma variável global e
//        crie e exiba a janela do programa principal.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Armazenar o identificador de instância em nossa variável global

   HWND hWnd = CreateWindowW(szWindowClass, _T("Cliente - Jogo dos Tubos"), WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNÇÃO: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  FINALIDADE: Processa as mensagens para a janela principal.
//
//  WM_COMMAND  - processar o menu do aplicativo
//  WM_PAINT    - Pintar a janela principal
//  WM_DESTROY  - postar uma mensagem de saída e retornar
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    RECT rect;
    PAINTSTRUCT ps;
    MINMAXINFO* mmi;
    static int xpto = 0;

    static HDC bmpDC;
    HBITMAP hBmp;
    static BITMAP bmp;
    static int xBitmap;
    static int yBitmap;
    static HANDLE hMutex;

    static HDC memDC = NULL;

    static DadosThreadDesenharTabuleiro desenhoTabuleiro;
    static TRACKMOUSEEVENT tme;

    switch (message)
    {
    
    case WM_CREATE:
        
        tme.cbSize = sizeof(TRACKMOUSEEVENT);
        tme.dwFlags = TME_HOVER;
        tme.hwndTrack = hWnd;
        tme.dwHoverTime = 2000;

        hBmp = (HBITMAP)LoadImage(NULL, _T("images/comecoAgua_comAgua.bmp"), IMAGE_BITMAP, TAM_BITMAP, TAM_BITMAP,
            LR_LOADFROMFILE);
        //BITMAP
        GetObject(hBmp, sizeof(bmp), &bmp);
        hdc = GetDC(hWnd);
        bmpDC = CreateCompatibleDC(hdc);
        SelectObject(bmpDC, hBmp);
        ReleaseDC(hWnd, hdc);

        GetClientRect(hWnd, &rect);
        //apanhar o meio 
        xBitmap = (rect.right / 2) - (bmp.bmWidth / 2);
        yBitmap = (rect.bottom / 2) - (bmp.bmHeight / 2);


        hMutex = CreateMutex(NULL, FALSE, NULL);
        desenhoTabuleiro.hMutex = hMutex;
        desenhoTabuleiro.hWnd = hWnd;
        desenhoTabuleiro.xBitmap = &xBitmap;
        desenhoTabuleiro.yBitmap = &yBitmap;
        desenhoTabuleiro.bmp = bmp;
        desenhoTabuleiro.bmpDC = bmpDC;
        desenhoTabuleiro.memDC = &memDC;

        CreateThread(NULL, 0, recebeTabuleiroServidor, &desenhoTabuleiro, 0, NULL);

        break;

    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);

        GetClientRect(hWnd, &rect);

        if (memDC == NULL) //primeira vez
        {
            memDC = CreateCompatibleDC(hdc);
            hBmp = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
            SelectObject(memDC, hBmp);
            DeleteObject(hBmp);
            FillRect(memDC, &rect, CreateSolidBrush(RGB(220, 220, 220)));
            BitBlt(memDC, xBitmap, yBitmap, bmp.bmWidth, bmp.bmHeight, bmpDC, 0, 0, SRCCOPY);
        }

        BitBlt(hdc, 0, 0, rect.right, rect.bottom, memDC, 0, 0, SRCCOPY);

        EndPaint(hWnd, &ps);
        break;

    case WM_ERASEBKGND: //Sent when the window background must be erased (for example, when a window is resized). 
        return 1;

    case WM_LBUTTONDOWN:

        colocaPeca(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), &desenhoTabuleiro);
        break;

    case WM_RBUTTONDOWN:
        removePeca(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), &desenhoTabuleiro);
        break;

    case  WM_MOUSEMOVE:
        if(TrackMouseEvent(&tme))
            pararAgua(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), &desenhoTabuleiro);
        break;


    case WM_GETMINMAXINFO: //Sent to a window when the size or position of the window is about to change. An application can use this message to override the window's default maximized size and position, or its default minimum or maximum tracking size.
        mmi = (MINMAXINFO*)lParam;
        mmi->ptMinTrackSize.x = bmp.bmWidth + 2;
        mmi->ptMinTrackSize.y = bmp.bmHeight * 2;
        break;

    case WM_SIZE: //Sent to a window after its size has changed.
        xBitmap = (LOWORD(lParam) / 2) - (bmp.bmWidth / 2);
        yBitmap = (HIWORD(lParam) / 2) - (bmp.bmHeight / 2);
        break;

    case WM_CLOSE:
        if (MessageBox(hWnd, TEXT("Tem a certeza que quer sair?"),
            TEXT("Confirmação"), MB_ICONQUESTION | MB_YESNO) == IDYES)
        {
            DestroyWindow(hWnd);
        }
        break;
    case WM_DESTROY: // Destruir a janela e terminar o programa 
    // "PostQuitMessage(Exit Status)"
        PostQuitMessage(0);
        break;
    default:
        // Neste exemplo, para qualquer outra mensagem (p.e. "minimizar","maximizar","restaurar")
        // não é efectuado nenhum processamento, apenas se segue o "default" do Windows
        return(DefWindowProc(hWnd, message, wParam, lParam));
        break;  // break tecnicamente desnecessário por causa do return
    }
    return 0;
}

// Manipulador de mensagem para a caixa 'sobre'.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
