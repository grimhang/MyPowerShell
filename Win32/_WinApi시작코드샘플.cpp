// 07_03_가_Edit.cpp : 애플리케이션에 대한 진입점을 정의합니다.
/*
copy ..\00_기본소스\framework.h .
copy ..\00_기본소스\targetver.h .
copy ..\00_기본소스\Main.ico .
copy ..\00_기본소스\이름.h 내이름.h
*/

#include "framework.h"
#include "내이름.h"
BOOL                InitInstance(HINSTANCE, int);
TCHAR* lpszClass = TEXT("Mless");

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {       
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;// MAKEINTRESOURCEW(IDC_MY0703EDIT);
    wcex.lpszClassName = TEXT("MyMainWindow");
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    RegisterClassExW(&wcex);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    MSG msg;
    
    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int) msg.wParam;
}




BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(TEXT("MyMainWindow"), lpszClass, WS_OVERLAPPEDWINDOW,
       100, 90, 1200, 600, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

