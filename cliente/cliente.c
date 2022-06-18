// cliente.cpp : Define o ponto de entrada para o aplicativo.
//

#include "framework.h"
#include "cliente.h"
#include "..\\utils_so2_tp\utils_so2_tp.h"

#define MAX_LOADSTRING 100

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

DWORD WINAPI recebeTabuleiroServidor(LPVOID p) {

    DadosJogo* dados = (DadosJogo*)p;

    DadosJogo recebido;

    HANDLE hPipeTabuleiro;
    BOOL ret;
    DWORD n;

    if (!WaitNamedPipe(PIPE_NAME_TABULEIRO, NMPWAIT_WAIT_FOREVER)) {
        exit(-1);
    }

    hPipeTabuleiro = CreateFile(PIPE_NAME_TABULEIRO, GENERIC_READ, 0, NULL, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL, NULL);
    if (hPipeTabuleiro == NULL) {
        exit(-1);
    }

    while (1) {
        ret = ReadFile(hPipeTabuleiro, &recebido, sizeof(DadosJogo), &n, NULL);

        if (!ret || !n) {
            break;
        }

        copiarTabuleiroJogoRecebido(recebido, dados);

        for (int j = 0; j < dados->nLinhas; j++)
            for (int k = 0; k < dados->nColunas; k++)
                dados->mapaJogo[j][k] = recebido.mapaJogo[j][k];

        if (dados->aJogar)
            desenharMapaJogo(dados);
    }

    CloseHandle(hPipeTabuleiro);
    return 0;
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
    static HDC bmpDC = NULL;
    HBITMAP hBmp = NULL;
    static BITMAP bmp;
    static int xBitmap;
    static int yBitmap;
    switch (message)
    {
    case WM_CREATE:
        hBmp = (HBITMAP)LoadImage(NULL, TEXT("images/comecoAgua_comAgua.bmp"), IMAGE_BITMAP, 75, 75,
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
        break;

    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        GetClientRect(hWnd, &rect);
        //A função FillRect preenche um retângulo usando o pincel especificado.
        FillRect(hdc, &rect, CreateSolidBrush(RGB(128, 128, 128)));
        BitBlt(hdc, xBitmap, yBitmap, bmp.bmWidth, bmp.bmHeight, bmpDC, 0, 0, SRCCOPY);
        //A função EndPaint marca o fim da pintura na janela especificada.Essa função é necessária para cada chamada à função BeginPaint
        EndPaint(hWnd, &ps);
        break;

    case WM_ERASEBKGND: //Sent when the window background must be erased (for example, when a window is resized). 
        return 1;

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
