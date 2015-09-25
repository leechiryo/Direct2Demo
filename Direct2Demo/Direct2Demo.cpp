// Direct2Demo.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "Direct2Demo.h"

#define MAX_LOADSTRING 100

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
  HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

  if (SUCCEEDED(CoInitialize(NULL))){
    DemoApp app;

    if (SUCCEEDED(app.Initialize())){
      app.RunMessageLoop();
    }

    CoUninitialize();
  }

  return 0;
}