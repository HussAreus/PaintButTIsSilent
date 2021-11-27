#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

//Constants
#define NEW_FILE 9
#define CHANGE_TITLE 2
#define RED 3
#define GREEN 4
#define BLUE 5
#define YELLOW 6
#define CYAN 7
#define PURPLE 8
#define ERRASER 10
#define BLACK 11
#define DIAMOND_BRUSH 1
#define TRIANGLE_BRUSH 2
#define CIRCLE_BRUSH 3
// Function Declarations
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void newMenu(HWND hwnd);
void newPage(HWND hwnd);
int newImage();
int editImage();
void draw(LONG x, LONG y);

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
int type = TRIANGLE_BRUSH;
int paintWidth = 20;
RGBCOLOR paintColor;
int coordCurrent;
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
    paintColor.r = 0;
    paintColor.g = 255;
    paintColor.b = 255;

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
            if(point.x>=0 && point.x<=bmpwidth && point.y>=0 && point.y<=bmpheight)
            {
                drawing = 1;
                cordx=point.x+(bmpheight-point.y)*bmpwidth;
            }

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
                    SetWindowTextW(hwnd, L"benediktasNoobas.bmp");
                    break;
                }
                case RED:
                {
                    paintColor.r = 255;
                    paintColor.g = 0;
                    paintColor.b = 0;
                    break;
                }
                case GREEN:
                {
                    paintColor.r = 0;
                    paintColor.g = 255;
                    paintColor.b = 0;
                    break;
                }
                case BLUE:
                {
                    paintColor.r = 0;
                    paintColor.g = 0;
                    paintColor.b = 255;
                    break;
                }
                case YELLOW:
                {
                    paintColor.r = 255;
                    paintColor.g = 255;
                    paintColor.b = 0;
                    break;
                }
                case CYAN:
                {
                    paintColor.r = 0;
                    paintColor.g = 255;
                    paintColor.b = 255;
                    break;
                }
                case PURPLE:
                {
                    paintColor.r = 255;
                    paintColor.g = 0;
                    paintColor.b = 255;
                    break;
                }
                case BLACK:
                {
                    paintColor.r = 0;
                    paintColor.g = 0;
                    paintColor.b = 0;
                    break;
                }
                case ERRASER:
                {
                    paintColor.r = 255;
                    paintColor.g = 255;
                    paintColor.b = 255;
                    break;
                }
                default:break;
            }
            break;
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

void draw(LONG x, LONG y)
{
    void paint(long x1, long y1){
        if(x1<0||x1>=bmpwidth)return;
        size_t coord =(x1 + (bmpheight-y1)*bmpwidth)*4;
        if(coord<=bmpheight*bmpwidth*4&&coord>=0){
            bits[coord]=paintColor.b;
            bits[coord+1]=paintColor.g;
            bits[coord+2]=paintColor.r;
        }
    }
    void errase(long x1, long y1, char oppacity){
        if(x1<0||x1>=bmpwidth)return;
        size_t coord =(x1 + (bmpheight-y1)*bmpwidth)*4;
        if(coord<=bmpheight*bmpwidth*4&&coord>=0){
            bits[coord]=0;
            bits[coord+1]=0;
            bits[coord+2]=0;
            bits[coord+3]=oppacity
        }
    }
    switch(type)
    {
    case DIAMOND_BRUSH:
        paint(x, y);
        for(int i=1; i<paintWidth; i++)
        {
            paint(x, y+i);
            paint(x, y-i);
            paint(x-i, y);
            paint(x+i, y);

            for(int j=1; j<(paintWidth-i); j++)
            {
                paint(x-i, y+j);
                paint(x-i, y-j);
                paint(x+i, y-j);
                paint(x+i, y+j);
            }

        }
        break;
    case TRIANGLE_BRUSH:
        paint(x, y);
        for(int i=1; i<paintWidth-1; i++)
        {
            paint(x, y-i);
            paint(x, y+i);
            for(int j=paintWidth-1; j<(int)paintWidth*1.5; j++)
            {
                paint(x, y+j);
            }
        }
        for(int i=1; i<floor((double)paintWidth*2.5); i++)
        {
            for(int j=1; j<i/2; j++)
            {
                paint(x+j, y-paintWidth+i);
                paint(x-j, y-paintWidth+i);
            }
        }
        break;
    case CIRCLE_BRUSH:
        paint(x, y);
        for(int i=1; i<paintWidth; i++)
        {
            paint(x+i, y);
            paint(x-i, y);
            paint(x, y+i);
            paint(x, y-i);

            for(int j=1; j<(sqrt(pow(paintWidth,2)-pow(i,2))); j++)
            {
                paint(x+j, y-i);
                paint(x-j, y-i);
                paint(x-j, y+i);
                paint(x+j, y+i);
            }

        }
        break;
    }

}

void newPage(HWND hwnd)
{
    hBitmap = CreateWindowW(L"Static", NULL, WS_VISIBLE | WS_CHILD | SS_BITMAP, 5, 5, bmpwidth, bmpheight/2, hwnd, NULL, NULL, NULL);
    SendMessageW(hBitmap, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImage);
}

int editImage(POINT point)
{
    draw(point.x, point.y);
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
    SendMessageW(hBitmap, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImage);
    return 1;
}

void newMenu(HWND hwnd)
{
    hmenu = CreateMenu();

    HMENU hFileMenu = CreateMenu();
    AppendMenu(hFileMenu, MF_STRING, NEW_FILE, "New");
    AppendMenu(hFileMenu, MF_STRING, CHANGE_TITLE, "Rename");
    AppendMenu(hFileMenu, MF_STRING, RED, "Red");
    AppendMenu(hFileMenu, MF_STRING, GREEN, "Green");
    AppendMenu(hFileMenu, MF_STRING, BLUE, "Blue");
    AppendMenu(hFileMenu, MF_STRING, YELLOW, "Yellow");
    AppendMenu(hFileMenu, MF_STRING, CYAN, "Cyan");
    AppendMenu(hFileMenu, MF_STRING, PURPLE, "Purple");
    AppendMenu(hFileMenu, MF_STRING, BLACK, "Black");
    AppendMenu(hFileMenu, MF_STRING, ERRASER, "Erraser");

    AppendMenu(hmenu, MF_POPUP, (UINT_PTR)hFileMenu, "File");

    SetMenu(hwnd, hmenu);
}
