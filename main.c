#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void newMenu(HWND hwnd);
void newPage(HWND hwnd);
void loadImage();

#define CHANGE_TITLE 2


HMENU hmenu;
HWND hEdit, hUwu;
HBITMAP hImage;
HINSTANCE hInst;
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
    int width = GetSystemMetrics(SM_CXFULLSCREEN);
    int height = GetSystemMetrics(SM_CYFULLSCREEN);

    // Step 2: Creating the Window
    hwnd = CreateWindowEx(WS_EX_ACCEPTFILES, MainWindowClass,"Paint, but \"t\" is silent",WS_OVERLAPPEDWINDOW,CW_USEDEFAULT, CW_USEDEFAULT, 500, /*width, height*/ 500, NULL, NULL, hInstance, NULL);

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
    switch(msg)
    {
        case WM_COMMAND:{
            switch(wParam)
            {
                case 1:
                {
                    MessageBeep(MB_DEFBUTTON2);
                    break;
                }
                case CHANGE_TITLE:
                {
                    wchar_t text[100];
                    GetWindowTextW(hEdit, text, 100);
                    SetWindowTextW(hwnd, text);
                    break;
                }
                case 3:
                {
                    MessageBeep(MB_DEFBUTTON2);
                    break;
                }
                case 4:
                {
                    MessageBeep(MB_DEFBUTTON2);
                    break;
                }
                case 5:
                {
                    MessageBeep(MB_DEFBUTTON2);
                    break;
                }
                case 6:
                {
                    MessageBeep(MB_DEFBUTTON2);
                    break;
                }
                case 7:
                {
                    MessageBeep(MB_DEFBUTTON2);
                    break;
                }
                case 8:
                {
                    MessageBeep(MB_DEFBUTTON2);
                    break;
                }

            }
            break;
        }
        case WM_CREATE:{
            loadImage();
            newMenu(hwnd);
            newPage(hwnd);
            break;
        }
        case WM_MOUSEWHEEL:{
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
     HWND Bitmap = CreateWindowEx(WS_EX_ACCEPTFILES, "Static", "", WS_VISIBLE | WS_CHILD, 5, 5, 400, 400, hwnd, NULL, NULL, NULL);
     hEdit = CreateWindowEx(WS_EX_ACCEPTFILES, "Edit", "", WS_VISIBLE | WS_CHILD | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL, 150, 20, 100, 100, Bitmap, NULL, NULL, NULL);
     HWND hButton = CreateWindowW(L"Button", NULL, WS_VISIBLE | WS_CHILD | BS_BITMAP, 155, 125, 100, 50, hwnd, (HMENU)CHANGE_TITLE, NULL, NULL);
     SendMessageW(hButton, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImage);
     hUwu = CreateWindowW(L"Static", NULL, WS_VISIBLE | WS_CHILD | SS_BITMAP, 150, 200, 100, 100, Bitmap, NULL, NULL, NULL);
     SendMessageW(hUwu, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImage);
}

void loadImage()
{
    hImage = (HBITMAP)LoadImageW(NULL, L"UwU.bmp", IMAGE_BITMAP, 100, 100, LR_LOADFROMFILE);
    /*FILE *fi;
    fi = fopen("UwU.bmp", "rb");
    char c = getc(fi);
    while(c != EOF)
    {
        printf("%x ", c);
        c = getc(fi);
    }*/

}

void newMenu(HWND hwnd)
{
    hmenu = CreateMenu();

    HMENU hFileMenu = CreateMenu();
    AppendMenu(hFileMenu, MF_STRING, 1, "New");
    AppendMenu(hFileMenu, MF_STRING, CHANGE_TITLE, "Rename");
    AppendMenu(hFileMenu, MF_SEPARATOR, 0, 0);
    AppendMenu(hFileMenu, MF_STRING, 3, "Open");
    AppendMenu(hFileMenu, MF_STRING, 4, "Save");
    AppendMenu(hFileMenu, MF_STRING, 5, "Save as...");

    HMENU hEditMenu = CreateMenu();
    AppendMenu(hEditMenu, MF_STRING, 6, "Contrast");
    AppendMenu(hEditMenu, MF_STRING, 7, "Color temperature");
    AppendMenu(hEditMenu, MF_STRING, 8, "Lighting");
    AppendMenu(hEditMenu, MF_SEPARATOR, 0, 0);
    AppendMenu(hEditMenu, MF_STRING, 9, "Erase Background");

    AppendMenu(hmenu, MF_POPUP, (UINT_PTR)hFileMenu, "File");
    AppendMenu(hmenu, MF_POPUP, (UINT_PTR)hEditMenu, "Edit");

    SetMenu(hwnd, hmenu);
}
