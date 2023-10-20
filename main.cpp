#include "windowapp.h"
#include "Game.h"
#include <string>
#include <iostream>
#include <GL/GL.h>

#include "time.h"
#include "dbg.h"    // debugging

#pragma comment(lib, "OpenGL32.lib")

// GLOBALS
HINSTANCE   hInst;
WCHAR       szTitle[MAX_LOADSTRING];
WCHAR       szWindowClass[MAX_LOADSTRING];
HDC         hdc;
HGLRC       hglrc;
bool        quit{ false };
Game        game;

void InitOpenGL(HWND hwnd, HDC *hdc, HGLRC *hglrc)
{
    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    // Flags
        PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
        32,                   // Colordepth of the framebuffer.
        0, 0, 0, 0, 0, 0,
        0,
        0,
        0,
        0, 0, 0, 0,
        24,                   // Number of bits for the depthbuffer
        8,                    // Number of bits for the stencilbuffer
        0,                    // Number of Aux buffers in the framebuffer.
        PFD_MAIN_PLANE,
        0,
        0, 0, 0
    };

    *hdc = GetDC(hwnd);

    auto pxfmt = ChoosePixelFormat(*hdc, &pfd);
    SetPixelFormat(*hdc, pxfmt, &pfd);

    *hglrc = wglCreateContext(*hdc);
    wglMakeCurrent(*hdc, *hglrc);
}

void ShutdownOpenGL(HWND hwnd, HDC hdc, HGLRC hglrc)
{
    wglMakeCurrent(hdc, NULL);
    wglDeleteContext(hglrc);
    ReleaseDC(hwnd, hdc);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance
    , _In_opt_ HINSTANCE hPrevInstance
    , _In_ LPWSTR    lpCmdLine
    , _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    
    miny::Result res = miny::OK;

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWAPP, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    auto hwnd = InitInstance(hInstance, nCmdShow);
    if (hwnd == NULL) {
        return FALSE;
    }
    
    InitOpenGL(hwnd, &hdc, &hglrc);
    res = game.Initialize(hwnd);
    if (!res) {
        return CLOSE_APPLICATION;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MainLoop(hwnd);

    game.Shutdown();
    ShutdownOpenGL(hwnd, hdc, hglrc);

    return 0;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWAPP));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = 0;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

HWND InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance;

   // Compute the window size so that the client area has the size I want
   RECT winRect{
       0, 0,
       800, 600,
   };
   DWORD winStyle = WS_OVERLAPPEDWINDOW;
   AdjustWindowRect(
       &winRect,
       winStyle,
       FALSE        // no menu
   );

   auto hwnd = CreateWindowW(
       szWindowClass,
       szTitle,
       winStyle,
       0, 0,
       Width(winRect), Height(winRect),
       nullptr,
       nullptr,
       hInstance,
       nullptr
   );
   if (hwnd == NULL) {
       return NULL;
   }

   return hwnd;
}

int MainLoop(HWND hwnd)
{
    MSG msg{ 0 };

    // Les dates t, t1, t2 et le delta time dt sont exprimés en microsecondes.
    u64 t = 0;
    u64 t1 = get_time_usec();
    u64 accumulator = 0;

    quit = false;
    while (!quit) {
        auto t2 = get_time_usec();
        auto dt = t2 - t1;
        accumulator += dt;
        t1 = t2;

        // Process any Windows messages
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (msg.message == WM_QUIT) {
            quit = true;
        }
        else {
            // Update the game with a fixed timestep
            while (accumulator >= game.FIXED_TIMESTEP_US) {
                dbg::log_update(t);
                quit = game.Update(game.FIXED_TIMESTEP_MS);
                accumulator -= game.FIXED_TIMESTEP_US;
                t += game.FIXED_TIMESTEP_US;
            }

            // Rendering however is done at each iteration of this loop
            float blue = 0.5f * (1.f + cosf(t / 1e6));
            glClearColor(0.f, 0.f, blue, 1.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            game.Render();
            SwapBuffers(hdc);

            // Print the framerate
            auto fps = (float)(1e6) / dt;
            dbg::printf("FPS: %.2f\n", fps);
        }
    }

    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case WM_KEYDOWN: {
        auto handled = game.HandleKey(wParam);
    }break;

    case WM_LBUTTONDOWN: {
        POINT p = {
            GET_X_LPARAM(lParam),
            GET_Y_LPARAM(lParam),
        };

        dbg::wprintf(L"Click at (%d,%d)\n", p.x, p.y);
    }break;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        // Draw stuff here...
        EndPaint(hwnd, &ps);
    }break;

    case WM_DESTROY: {
        quit = true;
        PostQuitMessage(0);
    }break;

    default: {
        return DefWindowProc(hwnd, message, wParam, lParam);
    }break;
    }

    return 0;
}