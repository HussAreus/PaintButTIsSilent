#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

//Constants
#define NEW_FILE 1
#define CHANGE_TITLE 2
#define PAINT_BRUSH 1
// Function Declarations
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void newMenu(HWND hwnd);
void newPage(HWND hwnd);
int newImage();
int editImage();
void draw(int coord);

typedef struct tagRGB{
    char r, g, b;
} RGBCOLOR;
// Global variables
//Main Window values
HMENU hmenu;
HINSTANCE hInst;
HDC hDc;
POINT point;
int width, height;
//Bitmap variables
HWND hBitmap ;
HBITMAP hImage;
BITMAPINFO bi;
int bmpwidth=500, bmpheight=500;
char *bits = NULL;
int drawing = 0;
//Paint style
int type = PAINT_BRUSH;
int paintWidth = 2;
RGBCOLOR paintColor;
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
    paintColor.r = 255;
    paintColor.g = 255;
    paintColor.b = 0;

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
    if(drawing && point.x>=0 && point.x<=bmpwidth && point.y>=0 && point.y<=bmpheight){
        editImage(point);
        SendMessageW(hBitmap, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImage);
    }
    switch(msg)
    {
        case WM_LBUTTONDOWN:{
            if(point.x>=0 && point.x<=bmpwidth && point.y>=0 && point.y<=bmpheight)drawing = 1;
            break;
            }
        case WM_LBUTTONUP:{
            drawing = 0;
            break;
        }
        case WM_COMMAND:{
            switch(wParam)
            {
                case NEW_FILE:
                {
                    newImage();
                    break;
                }
                case CHANGE_TITLE:
                {
                    SetWindowTextW(hwnd, L"untitled.bmp");
                    break;
                }
                default:break;
            }
        }
        case WM_CREATE:{
            newImage();
            newMenu(hwnd);
            newPage(hwnd);
            break;
        }
        default:return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

void draw(int coord)
{
    void paint(int coord){
        bits[coord]=paintColor.b;
        bits[coord+1]=paintColor.g;
        bits[coord+2]=paintColor.r;
    }
    switch(type)
    {
    case PAINT_BRUSH:
        paint(coord);
        for(int i=1; i<paintWidth; i++)
        {
            paint(coord+4*i);
            paint(coord-4*i);
            paint(coord-bmpwidth*4*i);
            paint(coord+bmpwidth*4*i);
        }
    }

}

void newPage(HWND hwnd)
{
    hBitmap = CreateWindowW(L"Static", NULL, WS_VISIBLE | WS_CHILD | SS_BITMAP, 5, 5, bmpwidth, bmpheight/2, hwnd, NULL, NULL, NULL);
    SendMessageW(hBitmap, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImage);
}

int editImage(POINT point)
{
    int coord=(point.x + (bmpheight-point.y)*bmpwidth)*4;
    draw(coord);
    SetDIBits( hDc, hImage, 0, bmpheight, bits, &bi, DIB_RGB_COLORS );
    return 1;
}

int newImage()
{
    hDc = CreateCompatibleDC (NULL);
    if(!hDc)printf("No HDC");
    hImage = (HBITMAP)LoadImageW(NULL, L"default.bmp", IMAGE_BITMAP, bmpwidth, bmpheight, LR_LOADFROMFILE);
    if(!hImage)printf("No IMAGE");

    //Setting basic header info.
    ZeroMemory(&bi, sizeof(BITMAPINFO));
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    //Getting header info for BITMAP:
    if (!GetDIBits(hDc, hImage, 0, 0, NULL, &bi, DIB_RGB_COLORS))return 0;
    //Checking and adjusting header info:
    if (bi.bmiHeader.biBitCount!=32)
	{
		printf("%d", bi.bmiHeader.biBitCount);
		return 0;
	}
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
    return 1;
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
