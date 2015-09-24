#pragma once

#include "resource.h"

template<class Interface>
inline void SafeRelease(Interface *pT) {
  if (pT != nullptr) {
    pT->Release();
    pT = nullptr;
  }
}

#ifndef Assert
#if defined( DEBUG ) || defined( _DEBUG )
#define Assert(b) do {if (!(b)) {OutputDebugStringA("Assert: " #b "\n");}} while(0)
#else
#define Assert(b)
#endif //DEBUG || _DEBUG
#endif

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

class DemoApp
{
public:
  DemoApp(){
    m_hwnd = nullptr;
    m_pDirect2dFactory = nullptr;
    m_pRenderTarget = nullptr;
    m_pLightSlateGrayBrush = nullptr;
    m_pCornflowerBlueBrush = nullptr;
  }

  ~DemoApp(){
    SafeRelease(m_pCornflowerBlueBrush);
    SafeRelease(m_pLightSlateGrayBrush);
    SafeRelease(m_pRenderTarget);
    SafeRelease(m_pDirect2dFactory);
  }

  // Register the window class and call methods for instantiating drawing resources
  HRESULT Initialize(){
    HRESULT hr;

    hr = CreateDeviceIndependentResources();

    if (SUCCEEDED(hr)){
      WNDCLASSEX wcex;

      wcex.cbSize = sizeof(WNDCLASSEX);

      wcex.style = CS_HREDRAW | CS_VREDRAW;
      wcex.lpfnWndProc = WndProc;
      wcex.cbClsExtra = 0;
      wcex.cbWndExtra = sizeof(LONG_PTR);
      wcex.hInstance = HINST_THISCOMPONENT;
      wcex.hIcon = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_DIRECT2DEMO));
      wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
      wcex.hbrBackground = NULL;
      wcex.lpszMenuName = NULL;
      wcex.lpszClassName = TEXT("D2DDemoApp");
      wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

      RegisterClassEx(&wcex);

      // get destop dpi
      FLOAT dpiX, dpiY;
      m_pDirect2dFactory->GetDesktopDpi(&dpiX, &dpiY);

      m_hwnd = CreateWindow(
        TEXT("D2DDemoApp"),
        TEXT("Direct2D Demo App"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,
        NULL,
        HINST_THISCOMPONENT,
        this); // pass the this pointer to window parameter

      if (m_hwnd){
        ShowWindow(m_hwnd, SW_SHOWNORMAL);
        UpdateWindow(m_hwnd);
      }
    }

    return hr;
  }

  // Process and dispatch messages
  void RunMessageLoop(){
    MSG msg;

    while (GetMessage(&msg, NULL, 0, 0)){
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }

private:
  // Initialize device-independent resources.
  HRESULT CreateDeviceIndependentResources(){
    return D2D1CreateFactory(
      D2D1_FACTORY_TYPE_SINGLE_THREADED,
      &m_pDirect2dFactory);
  }

  // Initialize device-dependent resources.
  HRESULT CreateDeviceResources(){
    HRESULT hr = S_OK;

    if (!m_pRenderTarget){
      RECT rc;
      GetClientRect(m_hwnd, &rc);

      D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

      hr = m_pDirect2dFactory->CreateHwndRenderTarget(
        D2D1::RenderTargetProperties(),
        D2D1::HwndRenderTargetProperties(m_hwnd, size),
        &m_pRenderTarget);

      if (SUCCEEDED(hr)){
        hr = m_pRenderTarget->CreateSolidColorBrush(
          D2D1::ColorF(D2D1::ColorF::LightSlateGray),
          &m_pLightSlateGrayBrush);
      }
      if (SUCCEEDED(hr)){
        hr = m_pRenderTarget->CreateSolidColorBrush(
          D2D1::ColorF(D2D1::ColorF::CornflowerBlue),
          &m_pCornflowerBlueBrush);
      }
    }

    return hr;
  }

  // Release device-dependent resource.
  void DiscardDeviceResources(){
    SafeRelease(m_pLightSlateGrayBrush);
    SafeRelease(m_pCornflowerBlueBrush);
    SafeRelease(m_pRenderTarget);
  }

  // Draw content.
  HRESULT OnRender(){
    HRESULT hr = S_OK;

    hr = CreateDeviceResources();

    if (SUCCEEDED(hr)){
      m_pRenderTarget->BeginDraw();
      m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
      m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
      D2D1_SIZE_F rtSize = m_pRenderTarget->GetSize();

      int width = static_cast<int>(rtSize.width);
      int height = static_cast<int>(rtSize.height);

      for (int x = 0; x < width; x += 10){
        m_pRenderTarget->DrawLine(
          D2D1::Point2F(static_cast<FLOAT>(x), 0.0f),
          D2D1::Point2F(static_cast<FLOAT>(x), rtSize.height),
          m_pLightSlateGrayBrush,
          0.5f);
      }

      for (int y = 0; y < height; y += 10){
        m_pRenderTarget->DrawLine(
          D2D1::Point2F(0.0f, static_cast<FLOAT>(y)),
          D2D1::Point2F(rtSize.width, static_cast<FLOAT>(y)),
          m_pLightSlateGrayBrush,
          0.5f);
      }

      m_pRenderTarget->EndDraw();
    }

    if (hr == D2DERR_RECREATE_TARGET){
      hr = S_OK;
      DiscardDeviceResources();
    }

    return hr;
  }

  // Resize the render target.
  void OnResize(UINT width, UINT height){
    if (m_pRenderTarget){
      m_pRenderTarget->Resize(D2D1::SizeU(width, height));
    }
  }

  // The windows procedure.
  static LRESULT CALLBACK WndProc(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam){

    LRESULT result = 0;

    if (message == WM_CREATE){
      LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
      DemoApp *pDemoApp = (DemoApp *)pcs->lpCreateParams;
      ::SetWindowLongPtr(hwnd, GWLP_USERDATA, PtrToUlong(pDemoApp));
      result = 1;
    }
    else{
      DemoApp *pDemoApp = reinterpret_cast<DemoApp *>(static_cast<LONG_PTR>(
        ::GetWindowLongPtr(hwnd, GWLP_USERDATA)));

      bool wasHandled = false;

      if (pDemoApp){
        switch (message){
        case WM_SIZE:
        {
          UINT width = LOWORD(lParam);
          UINT height = HIWORD(lParam);
          pDemoApp->OnResize(width, height);
          result = 0;
          wasHandled = true;
          break;
        }

        case WM_DISPLAYCHANGE:
          InvalidateRect(hwnd, NULL, false);
          result = 0;
          wasHandled = true;
          break;

        case WM_PAINT:
          pDemoApp->OnRender();
          ValidateRect(hwnd, NULL);
          result = 0;
          wasHandled = true;
          break;

        case WM_DESTROY:
          PostQuitMessage(0);
          result = 1;
          wasHandled = true;
          break;
        }
      }

      if (!wasHandled){
        result = DefWindowProc(hwnd, message, wParam, lParam);
      }
    }

    return result;
  }

  HWND m_hwnd;
  ID2D1Factory* m_pDirect2dFactory;
  ID2D1HwndRenderTarget* m_pRenderTarget;
  ID2D1SolidColorBrush* m_pLightSlateGrayBrush;
  ID2D1SolidColorBrush* m_pCornflowerBlueBrush;
};