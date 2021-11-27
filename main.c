#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

//Constants
#define NEW_FILE 1
#define CHANGE_TITLE 2
#define SAVE_FILE 3
#define RED 4
#define GREEN 5
#define BLUE 6
#define YELLOW 7
#define CYAN 8
#define PURPLE 9
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
//File Save Functions
void save_bitmap(char *path);
void save_file(HWND hwnd);

typedef struct tagRGB{
    char r, g, b;
} RGBCOLOR;
//Global variables
//Main Window values
HMENU hmenu;
HINSTANCE hInst;
HDC hDc;
POINT point;
int width, height;
char file_saved=0;
//Bitmap variables
HWND hBitmap ;
HBITMAP hImage;
BITMAPINFO bi;
int bmpwidth=500, bmpheight=500;
char *bits = NULL;
int drawing = 0;
//Paint style
int type = TRIANGLE_BRUSH;
int paintWidth = 10;
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
    paintColor.g = 0;
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
            if(point.x>=0 && point.x<=bmpwidth && point.y>=0 && point.y<=bmpheight)
            {
                drawing = 1;
                coordCurrent=point.x+(bmpheight-point.y)*bmpwidth;
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
                case SAVE_FILE:
                {
                    file_saved=1;
                    save_file(hwnd);
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
            bits[coord]=255;
            bits[coord+1]=255;
            bits[coord+2]=255;
            bits[coord+3]=oppacity;
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

void save_file(HWND hwnd)
{
    OPENFILENAME ofn;
    char file_name[260];
    ZeroMemory(&ofn, sizeof(OPENFILENAME));

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = file_name;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = 260;
    ofn.lpstrFilter = "Bitmap\0*.bmp\0";
    ofn.nFilterIndex = 1;

    GetOpenFileName(&ofn);
    save_bitmap(ofn.lpstrFile);

}

void newMenu(HWND hwnd)
{
    hmenu = CreateMenu();

    HMENU hFileMenu = CreateMenu();
    AppendMenu(hFileMenu, MF_STRING, NEW_FILE, "New");
    AppendMenu(hFileMenu, MF_STRING, CHANGE_TITLE, "Rename");
    AppendMenu(hFileMenu, MF_STRING, SAVE_FILE, "Save as");
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

void save_bitmap(char *path)
{
    FILE *fi;
    fi = fopen(path, "wb");
    char bmpPad[3] = {0, 0, 0};
    int paddingAmount = ((4-(bmpwidth*4)%4)%4);

    int fileHeaderSize = 14;
    int informationHeaderSize = 40;
    int fileSize = fileHeaderSize + informationHeaderSize + bmpwidth * bmpheight * 4+ paddingAmount * bmpheight;

    char fileHeader[fileHeaderSize];
    //File Type. BM stands for bitmap
    fileHeader[0] = 'B';
    fileHeader[1] = 'M';
    //File Size. sizeof(fileSize) = 4. That's why we have to move by a byte to assign it to Unsigned Char (size is 1)
    fileHeader[2] = fileSize;
    fileHeader[3] = fileSize>>8;
    fileHeader[4] = fileSize>>16;
    fileHeader[5] = fileSize>>24;
    //Reserved 1. (No idea)
    fileHeader[6] = 0;
    fileHeader[7] = 0;
    //Reserved 2. (No idea as well)
    fileHeader[8] = 0;
    fileHeader[9] = 0;
    //Pixel data offset. Used to determine when the real pixel data is starting.
    fileHeader[10] = fileHeaderSize + informationHeaderSize;
    fileHeader[11] = 0;
    fileHeader[12] = 0;
    fileHeader[13] = 0;

    unsigned char informationHeader[informationHeaderSize];
    //Header Size.
    informationHeader[0] = informationHeaderSize;
    informationHeader[1] = 0;
    informationHeader[2] = 0;
    informationHeader[3] = 0;
    //Image Width.
    informationHeader[4] = bmpwidth;
    informationHeader[5] = bmpwidth>>8;
    informationHeader[6] = bmpwidth>>16;
    informationHeader[7] = bmpwidth>>24;
    //Image Height
    informationHeader[8] = bmpheight;
    informationHeader[9] = bmpheight>>8;
    informationHeader[10] = bmpheight>>16;
    informationHeader[11] = bmpheight>>24;
    //Planes
    informationHeader[12] = 1;
    informationHeader[13] = 0;
    //Bits Per Pixel
    informationHeader[14] = 32;
    informationHeader[15] = 0;
    //Compression. None In This Case.
    informationHeader[16] = 0;
    informationHeader[17] = 0;
    informationHeader[18] = 0;
    informationHeader[19] = 0;
    //Image Size. No Compression -> Unnecessary.
    informationHeader[20] = 0;
    informationHeader[21] = 0;
    informationHeader[22] = 0;
    informationHeader[23] = 0;
    //X Pixels Per Meter.
    informationHeader[24] = 0;
    informationHeader[25] = 0;
    informationHeader[26] = 0;
    informationHeader[27] = 0;
    //Y Pixels Per Meter.
    informationHeader[28] = 0;
    informationHeader[29] = 0;
    informationHeader[30] = 0;
    informationHeader[31] = 0;
    //Total Colors (if with pallet header)
    informationHeader[32] = 0;
    informationHeader[33] = 0;
    informationHeader[34] = 0;
    informationHeader[35] = 0;
    //Important Colors. (No Idea)
    informationHeader[36] = 0;
    informationHeader[37] = 0;
    informationHeader[38] = 0;
    informationHeader[39] = 0;

    fwrite(fileHeader, fileHeaderSize, 1, fi);
    fwrite(informationHeader, informationHeaderSize, 1, fi);

    for(int i=0; i<bmpheight; i++)
    {
        for(int j=0; j<bmpwidth; j++)
        {
            char color[] = {bits[(i*bmpwidth+j)*4], bits[(i*bmpwidth+j)*4+1], bits[(i*bmpwidth+j)*4+2], bits[(i*bmpwidth+j)*4+3]};
            fwrite(color, 4, 1, fi);
        }
        fwrite(bmpPad, paddingAmount, 1, fi);
    }
    fclose(fi);
    printf("FILE UPLOADED\n");
}
