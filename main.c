#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

//Constants
#define NEW_FILE 1
#define CHANGE_TITLE 2

// Function Declarations
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void newMenu(HWND hwnd);
void newPage(HWND hwnd);
void newImage();
int editImage();

// Global variables
HWND hBitmap ;
HMENU hmenu;
HBITMAP hImage;
HINSTANCE hInst;
HDC hDc;
POINT point;
COLORREF paintColor = 0x00FFFFFF;
int width, height, bmpwidth=500, bmpheight=500;
//Main Window
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow)
{
    LPCSTR MainWindowClass = "MainWindow";
    hInst = hInstance;

    WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;

    //Step 1: Registering the Window Class
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = MainWindowClass;
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    if(!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "Window Registration Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    width = GetSystemMetrics(SM_CXFULLSCREEN);
    height = GetSystemMetrics(SM_CYFULLSCREEN);
    width/=2;
    height*=0.5;

    // Step 2: Creating the Window
    hwnd = CreateWindowEx(WS_EX_ACCEPTFILES, MainWindowClass,"Paint, but \"t\" is silent",WS_OVERLAPPEDWINDOW,CW_USEDEFAULT, CW_USEDEFAULT, (height*2)+200, (height*1.5)+20, NULL, NULL, hInstance, NULL);

    if(hwnd == NULL)
    {
        MessageBox(NULL, "Window Creation Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Step 3: The Message Loop
    while(GetMessage(&Msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return Msg.wParam;
}

// Step 4: the Window Procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    GetCursorPos(&point);
    ScreenToClient(hBitmap, &point);
    switch(msg)
    {
        case WM_LBUTTONDOWN:{
            if(point.x>=0 && point.x<=bmpwidth && point.y>=0 && point.y<=bmpheight)
            {
                editImage(point);
                //UpdateWindow(hBitmap);
            }
        }
        case WM_COMMAND:{
            switch(wParam)
            {
                case 1:
                {
                    MessageBeep(0xFFFFFFFF);
                    break;
                }
                case CHANGE_TITLE:
                {
                    SetWindowTextW(hwnd, L"untitled.bmp");
                    break;
                }
            }
            break;
        }
        case WM_CREATE:{
            newImage();
            newMenu(hwnd);
            newPage(hwnd);
            break;
        }
        case WM_CLOSE:{
            DestroyWindow(hwnd);
            break;
        }
        case WM_DESTROY:{
            PostQuitMessage(0);
            break;
        }
        default:return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

void newPage(HWND hwnd)
{
    hBitmap = CreateWindowW(L"Static", NULL, WS_VISIBLE | WS_CHILD | SS_BITMAP, 5, 5, bmpwidth, bmpheight/2, hwnd, NULL, NULL, NULL);
    SendMessageW(hBitmap, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImage);
}

int editImage(POINT point)
{
    char *bits = NULL;
    BITMAPINFO bi;
    ZeroMemory(&bi, sizeof(BITMAPINFO));
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    //Getting header info for BITMAP:
    if (!GetDIBits(hDc, hImage, 0, 0, NULL, &bi, DIB_RGB_COLORS))
        return 0;
    //Checking and adjusting header info:
    /*if (bi.bmiHeader.biBitCount!=32)
	{
		printf("%d", bi.bmiHeader.biBitCount);
		return 0;
	}*/
	if(bi.bmiHeader.biCompression != BI_RGB && bi.bmiHeader.biCompression != BI_BITFIELDS)printf("SUS");
	bi.bmiHeader.biCompression = BI_RGB;
    //Allocating required memory based on header provided info:
    bits = (char*)malloc(bi.bmiHeader.biSizeImage);
    if (!bits){
        free( bits );
        return 0;
    }
    //Getting BIT data:
    if (!GetDIBits(hDc, hImage, 0, bmpheight, bits, &bi, DIB_RGB_COLORS)){
        free( bits );
        return 0;
    }
    //Changing BITMAP data:
    printf("%d %d %d\n", bits[(point.x + point.y*bmpwidth)*3], bits[(point.x + point.y*bmpwidth)*3+1], bits[(point.x+ point.y*bmpwidth)*3+2]);
    //Updating BITMAP data:
    SetDIBits( hDc, hImage, 0, bmpheight, bits, &bi, DIB_RGB_COLORS );
    return 1;
}

void newImage()
{
    hDc = CreateCompatibleDC (NULL);
    if(!hDc)printf("No HDC");

    hImage = (HBITMAP)CreateCompatibleBitmap(hDc, bmpwidth, bmpheight);
    if(!hImage)printf("No IMAGE");
}

void newMenu(HWND hwnd)
{
    hmenu = CreateMenu();

    HMENU hFileMenu = CreateMenu();
    AppendMenu(hFileMenu, MF_STRING, NEW_FILE, "New");
    AppendMenu(hFileMenu, MF_STRING, CHANGE_TITLE, "Rename");

    AppendMenu(hmenu, MF_POPUP, (UINT_PTR)hFileMenu, "File");

    SetMenu(hwnd, hmenu);
}
