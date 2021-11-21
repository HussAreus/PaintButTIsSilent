#include <windows.h>

void DrawOntoDC (HDC dc) {

     pen    = CreatePen(...)
     penOld = SelectObject(dc, pen)

     ..... Here is the actual drawing, that
     ..... should be regurarly called, since
     ..... the drawn picture changes as time
     ..... progresses

     SelectObject(dc, pen_old);

     DeleteObject(pen);
}



LRESULT CALLBACK WindowProc(WND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch(msg) {
    case WM_PAINT: {
         PAINTSTRUCT ps;
         dc = BeginPaint(hWnd, &ps);

         HBITMAP PersistenceBitmap;
         PersistenceBitmap = CreateCompatibleBitmap(dc, windowHeight, windowHeight);

         HDC dcMemory =  CreateCompatibleDC(dc);
         HBITMAP oldBmp = (HBITMAP) SelectObject(dcMemory, PersistenceBitmap);

         DrawOntoDC(dcMemory);

         ..... "copying" the memory dc in one go unto dhe window dc:

         BitBlt( dc,0, 0, windowWidth, windowHeight,dcMemory,0, 0,SRCCOPY);

         ..... destroy the allocated bitmap and memory DC
         ..... I have the feeling that this could be implemented
         ..... better, i.e. without allocating and destroying the memroy dc
         ..... and bitmap with each WM_PAINT.

         SelectObject(dcMemory, oldBmp);
         DeleteDC(dcMemory);
         DeleteObject(PersistenceBitmap);

     EndPaint  (hWnd, &ps);
         return 0;
    }
    default:
        return DefWindowProc(hWnd, Msg, wParam, lParam);
    }
}


DWORD WINAPI Timer(LPVOID p) {

  ..... The 'thread' that makes sure that the window
  ..... is regularly painted.

  HWND hWnd = (HWND) *((HWND*) p);

  while (1) {
     Sleep(1000/framesPerSecond);
     InvalidateRect(hWnd, 0, TRUE);
  }
}


int APIENTRY WinMain(...) {

    WNDCLASSEX windowClass;
       windowClass.lpfnWndProc         = WindowProc;
       windowClass.lpszClassName       = className;
       ....

    RegisterClassEx(&windowClass);

    HWND hwnd = CreateWindowEx(
                ....
                 className,
                 ....);


    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    DWORD threadId;
    HANDLE hTimer  = CreateThread(
      0, 0,
      Timer,
     (LPVOID) &hwnd,
     0, &threadId );

    while( GetMessage(&Msg, NULL, 0, 0) ) {
       ....
    }

    return Msg.wParam;
}
