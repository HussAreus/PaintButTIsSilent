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
#define DIAMOND_BRUSH 101
#define DIAMOND_BRUSH_BUTTON 201
#define TRIANGLE_BRUSH 102
#define TRIANGLE_BRUSH_BUTTON 202
#define CIRCLE_BRUSH 103
#define CIRCLE_BRUSH_BUTTON 203
#define LINE_TOOL 104
#define LINE_TOOL_BUTTON 204
//Color structure
// Function Declarations
unsigned int Crc32(char *stream, int offset, int length, unsigned int crc);
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void newMenu(HWND hwnd);
void newPage(HWND hwnd);
int newImage();
int editImage();
void draw(LONG x, LONG y);
//File Save Functions
void save_bitmap(char *path);
void save_png(char *path);
void save_file(HWND hwnd);

typedef struct tagRGBA{
    unsigned char r, g, b, a;
} RGBACOLOR;

//////Global variables//////
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
int bmpwidth=700, bmpheight=700;
unsigned char *bits = NULL;
BOOL drawing = 0;

//Paint style
int paintOppacity=0;
int type = LINE_TOOL;
int paintWidth = 3;
RGBACOLOR paintColor;
POINT coordStart;
POINT coordEnd;

//Palette and CRC for PNG
int paintPaletteSize=1;
RGBACOLOR paintPalette[256];
unsigned int crcTable[256];


//Main Window
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow)
{
    LPCSTR MainWindowClass = "MainWindow";
    hInst = hInstance;

    WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;

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

    hwnd = CreateWindowEx(WS_EX_ACCEPTFILES, MainWindowClass,"Paint, but \"t\" is silent",WS_OVERLAPPEDWINDOW,CW_USEDEFAULT, CW_USEDEFAULT, (height*2)+200, (height*1.5)+20, NULL, NULL, hInstance, NULL);

    if(hwnd == NULL)
    {
        MessageBox(NULL, "Window Creation Failed!", "Error!",MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    while(GetMessage(&Msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return Msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if(msg==WM_LBUTTONUP)
    {

        if(type==LINE_TOOL)
        {
            coordEnd=point;
            printf("%d, %d: %d, %d\n", coordStart.x, coordStart.y, coordEnd.x, coordEnd.y);
            drawing = 1;
        }
        else
        {
            drawing = 0;
        }
        return 0;
    }
    GetCursorPos(&point);
    ScreenToClient(hBitmap, &point);
    if(drawing && point.x>=0 && point.x<=bmpwidth && point.y>=0 && point.y<=bmpheight){
        editImage(point);
        SendMessageW(hBitmap, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImage);
        if (type == LINE_TOOL)
        {
            drawing = 0;
        }
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    switch(msg)
    {
        case WM_LBUTTONDOWN:{
            if(point.x>=0 && point.x<=bmpwidth && point.y>=0 && point.y<=bmpheight)
            {
                if(type==LINE_TOOL)
                {
                    coordStart=point;
                }
                else
                {
                    drawing=1;
                }
            }
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
                case CIRCLE_BRUSH_BUTTON:
                {
                    type=CIRCLE_BRUSH;
                    break;
                }
                case TRIANGLE_BRUSH_BUTTON:
                {
                    type=TRIANGLE_BRUSH;
                    break;
                }
                case DIAMOND_BRUSH_BUTTON:
                {
                    type=DIAMOND_BRUSH;
                    break;
                }
                case LINE_TOOL_BUTTON:
                {
                    type=LINE_TOOL;
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
            RGBACOLOR rgba;
            rgba.r = 0;
            rgba.g = 0;
            rgba.b = 0;
            rgba.a = 0;
            paintPalette[0]= rgba;
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
            bits[coord+3]=paintOppacity;
        }
    }
    void errase(long x1, long y1){
        if(x1<0||x1>=bmpwidth)return;
        size_t coord =(x1 + (bmpheight-y1)*bmpwidth)*4;
        if(coord<=bmpheight*bmpwidth*4&&coord>=0){
            bits[coord]=255;
            bits[coord+1]=255;
            bits[coord+2]=255;
            bits[coord+3]=paintOppacity;
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

    case LINE_TOOL:
        {
            float y, x, length, height;
            float change;
            length=coordEnd.x-coordStart.x;
            height=coordEnd.y-coordStart.y;
            if (length*length>height*height)
            {
                if(coordEnd.x>coordStart.x)
                {
                    y=coordStart.y;
                    change=height/length;
                    for(int x=coordStart.x; x<coordEnd.x; x++)
                    {
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
                        y=y+change;
                    }
                }
                else if(coordEnd.x<coordStart.x)
                {
                    y=coordEnd.y;
                    change=height/length;
                    for(int x=coordEnd.x; x<coordStart.x; x++)
                    {
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
                        y=y+change;
                    }
                }
                else
                {
                    y=coordStart.y;
                    for(int x=coordEnd.x; x<coordStart.x; x++)
                    {
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

                }

            }
            else
            {
                if(coordEnd.y>coordStart.y)
                {
                    x=coordStart.x;
                    change=length/height;
                    for(int y=coordStart.y; y<coordEnd.y; y++)
                    {
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
                        x=x+change;
                    }
                }
                else if(coordEnd.y<coordStart.y)
                {
                    x=coordEnd.x;
                    change=length/height;
                    for(int y=coordEnd.y; y<coordStart.y; y++)
                    {
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
                        x=x+change;
                    }
                }
                else
                {
                    x=coordStart.x;
                    for(int y=coordEnd.y; y<coordStart.y; y++)
                    {
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

                }
            }
            break;

        }

    }

}

void newPage(HWND hwnd)
{
    CreateWindowW(L"Static", NULL, WS_VISIBLE | WS_CHILD | WS_THICKFRAME, 0, 0, 720, 70, hwnd, NULL, NULL, NULL);

    CreateWindowW(L"Button", L"cir", WS_VISIBLE | WS_CHILD, 5, 5, 30, 30, hwnd, (HMENU)CIRCLE_BRUSH_BUTTON, NULL, NULL);
    CreateWindowW(L"Button", L"tri", WS_VISIBLE | WS_CHILD, 5, 35, 30, 30, hwnd, (HMENU)TRIANGLE_BRUSH_BUTTON, NULL, NULL);
    CreateWindowW(L"Button", L"dia", WS_VISIBLE | WS_CHILD, 35, 5, 30, 30, hwnd, (HMENU)DIAMOND_BRUSH_BUTTON, NULL, NULL);
    CreateWindowW(L"Button", L"line", WS_VISIBLE | WS_CHILD, 35, 35, 30, 30, hwnd, (HMENU)LINE_TOOL_BUTTON, NULL, NULL); //BS_ICON

    hBitmap = CreateWindowW(L"Static", NULL, WS_VISIBLE | WS_CHILD | SS_BITMAP | WS_THICKFRAME, 5, 75, bmpwidth, bmpheight, hwnd, NULL, NULL, NULL);
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
    bits = (unsigned char*)malloc(bi.bmiHeader.biSizeImage);
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
    strcpy(ofn.lpstrFile, "untitled.bmp");
    ofn.nMaxFile = 260;
    ofn.lpstrFilter = "Bitmap\0*.bmp\0PNG\0*.png\0";
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

void save_png(char *path)
{
    FILE *fi;
    fi = fopen(path, "wb");
    int fileHeaderSize = 8;
    int IHDRsize = 13;
    int PLTEsize = paintPaletteSize*3;
    int tRNSsize = paintPaletteSize;
    int IDATsize = bmpwidth * bmpheight;


    unsigned char fileHeader[fileHeaderSize];{
    // High Bit
    fileHeader[0] = 137;
    // File format
    fileHeader[1] = 'P';
    fileHeader[2] = 'N';
    fileHeader[3] = 'G';
    // Some DOS bullshit
    fileHeader[4] = 13;
    fileHeader[5] = 10;
    fileHeader[6] = 26;
    fileHeader[7] = 10;
    }

    unsigned char IHDR[IHDRsize];{
    // Width.
    IHDR[0] = bmpwidth>>24;
    IHDR[1] = bmpwidth>>16;
    IHDR[2] = bmpwidth>>8;
    IHDR[3] = bmpwidth;
    // Height.
    IHDR[4] = bmpheight>>24;
    IHDR[5] = bmpheight>>16;
    IHDR[6] = bmpheight>>8;
    IHDR[7] = bmpheight;
    // Bit depth. In this case up to 255 so it's 8 bit.
    IHDR[8] = 8;
    // Color type. We want RGBA. If possible palette would be cool...
    IHDR[9] = 6;
    // Compression, Filter, Interlace.
    IHDR[10] = 0;
    IHDR[11] = 0;
    IHDR[12] = 0;
    }

    unsigned char PLTE[PLTEsize];
    unsigned char tRNS[tRNSsize];
    for(int i=0; i<paintPaletteSize; i++){
        PLTE[i*3]=paintPalette[i].r;
        PLTE[i*3+1]=paintPalette[i].g;
        PLTE[i*3+2]=paintPalette[i].b;
        tRNS[i]=paintPalette[i].a;
    }

    unsigned char IDAT[IDATsize];{
    for(int i=0; i<IDATsize; i++)
    {
        for(int j=0; j<paintPaletteSize; j++)
        {
            if(bits[i*4]==paintPalette[j].b&&bits[i*4+1]==paintPalette[j].r&&bits[i*4+2]==paintPalette[j].g&&bits[i*4+3]==paintPalette[j].a)
            {
                IDAT[i]=j;
                break;
            }
        }
    }}

    unsigned char size[4];
    unsigned char CRC[4];
    unsigned int crc;

    fwrite(fileHeader, fileHeaderSize, 1, fi);
    //IHDR
    size[0]=IHDRsize>>24;
    size[1]=IHDRsize>>16;
    size[2]=IHDRsize>>8;
    size[3]=IHDRsize;
    fwrite(size, 4, 1, fi);
    fwrite("IHDR", 4, 1, fi);
    fwrite(IHDR, IHDRsize, 1, fi);
    //crc=Crc32("IHDR", 0, 4, 0);
    crc = 2829168138;
    CRC[0]=crc>>24;
    CRC[1]=crc>>16;
    CRC[2]=crc>>8;
    CRC[3]=crc;
    fwrite(CRC, 4, 1, fi);
    //PLTE
    size[0]=PLTEsize>>24;
    size[1]=PLTEsize>>16;
    size[2]=PLTEsize>>8;
    size[3]=PLTEsize;
    fwrite(size, 4, 1, fi);
    fwrite("PLTE", 4, 1, fi);
    fwrite(PLTE, PLTEsize, 1, fi);
    //crc=Crc32("PLTE", 0, 4, 0);
    crc = 1269336405;
    CRC[0]=crc>>24;
    CRC[1]=crc>>16;
    CRC[2]=crc>>8;
    CRC[3]=crc;
    fwrite(CRC, 4, 1, fi);
    //tRNS
    size[0]=tRNSsize>>24;
    size[1]=tRNSsize>>16;
    size[2]=tRNSsize>>8;
    size[3]=tRNSsize;
    fwrite(size, 4, 1, fi);
    fwrite("tRNS", 4, 1, fi);
    fwrite(tRNS, tRNSsize, 1, fi);
    //crc=Crc32("tRNS", 0, 4, 0);
    crc = 918122700;
    CRC[0]=crc>>24;
    CRC[1]=crc>>16;
    CRC[2]=crc>>8;
    CRC[3]=crc;
    fwrite(CRC, 4, 1, fi);
    //IDAT
    size[0]=IDATsize>>24;
    size[1]=IDATsize>>16;
    size[2]=IDATsize>>8;
    size[3]=IDATsize;
    fwrite(size, 4, 1, fi);
    fwrite("IDAT", 4, 1, fi);
    fwrite(IDAT, IDATsize, 1, fi);
    //crc=Crc32("IDAT", 0, 4, 0);
    crc = 900662814;
    CRC[0]=crc>>24;
    CRC[1]=crc>>16;
    CRC[2]=crc>>8;
    CRC[3]=crc;
    fwrite(CRC, 4, 1, fi);
    //IEND
    size[0]=0;
    size[1]=0;
    size[2]=0;
    size[3]=0;
    fwrite(size, 4, 1, fi);
    fwrite("IEND", 4, 1, fi);
    //crc=Crc32("IEND", 0, 4, 0);
    crc = 2923585666;
    CRC[0]=crc>>24;
    CRC[1]=crc>>16;
    CRC[2]=crc>>8;
    CRC[3]=crc;
    fwrite(CRC, 4, 1, fi);


    fclose(fi);
    printf("FILE UPLOADED\n");

}

unsigned int Crc32(char *stream, int offset, int length, unsigned int crc)
{
    unsigned int c;
    for(unsigned int n=0; n<=255; n++)
    {
        c = n;
        for(int k=0;k<8;k++){
            if((c & 1) == 1)
                c = 0xEDB88320^((c>>1)&0x7FFFFFFF);
            else
                c = ((c>>1)&0x7FFFFFFF);
        }
        crcTable[n] = c;
    }
    c = crc^0xffffffff;
    int endOffset=offset+length;
    for(int i=offset;i<endOffset;i++){
        c = crcTable[(c^stream[i]) & 255]^((c>>8)&0xFFFFFF);
    }
    return c^0xffffffff;
}
