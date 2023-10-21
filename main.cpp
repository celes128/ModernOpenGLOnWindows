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
LPCWSTR     g_windowClassName   = L"MyWindowClass";
LPCWSTR     g_windowTitle       = L"My Application";
bool        quit{ false };
Game        game;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    
    CoreSystems sys;
    
    auto result = InitializeCoreSystems(hInstance, nCmdShow, &sys);
    if (!result) {
        return 1;
    }
    
    MainLoop(&sys);

    //game.Shutdown();
    TerminateCoreSystems(&sys);
    return 0;
}

miny::Result InitializeCoreSystems(in_ HINSTANCE hinstance, in_ int nCmdShow, out_ CoreSystems* sys)
{
    assert(sys != nullptr);
    
    miny::Result result = miny::OK;

    *sys = { 0 };
    sys->hinstance = hinstance;
    sys->winSize = SIZE{ 800, 600 };

    result = CreateTheWindow(in_ sys->winSize, in_ hinstance, out_ & sys->hwnd);
    if (!result) {
        return miny::FAILED;
    }
    
    result = InitializeGraphics(in_ sys->hwnd, in_ sys->winSize, in_ hinstance, out_ &sys->graphics);
    if (!result) {
        DestroyTheWindow(&sys->hwnd);
        return miny::FAILED;
    }

    ShowWindow(sys->hwnd, nCmdShow);
    UpdateWindow(sys->hwnd);

    return miny::OK;
}

void TerminateCoreSystems(in_ out_ CoreSystems* sys)
{
    assert(sys != nullptr);

    TerminateGraphics(&sys->graphics);
    DestroyTheWindow(&sys->hwnd);
}

miny::Result CreateTheWindow(in_ SIZE size, in_ HINSTANCE hinstance, out_ HWND* hwnd)
{
    assert(hwnd != nullptr);

    // Create a window class
    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hinstance;
    wcex.hIcon = LoadIcon(hinstance, MAKEINTRESOURCE(IDI_WINDOWAPP));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = 0;
    wcex.lpszClassName = g_windowClassName;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    // Register this window class
    auto atom = RegisterClassExW(&wcex);

    // Determine the window rectangle size so that the client area has the requested size
    auto winRect = RECT{ 0, 0, size.cx, size.cy };
    DWORD winStyle = WS_OVERLAPPEDWINDOW;
    AdjustWindowRect(&winRect, winStyle, FALSE);

    // Create the app window
    *hwnd = CreateWindowW(g_windowClassName, g_windowTitle, winStyle,
        0, 0, Width(winRect), Height(winRect),
        nullptr, nullptr, hinstance, nullptr
    );
    if (*hwnd == NULL) {
        return miny::FAILED;
    }

    return miny::OK;
}

void DestroyTheWindow(in_ out_ HWND* hwnd)
{
    assert(hwnd != nullptr);

    DestroyWindow(*hwnd);
    *hwnd = nullptr;
}

miny::Result InitializeGraphics(in_ HWND hwnd, in_ SIZE winSize, in_ HINSTANCE hinstance, out_ Graphics** graphics)
{
    assert(graphics != nullptr);

    *graphics = new Graphics();
    if (*graphics == nullptr) {
        return miny::FAILED;
    }

    auto result = (*graphics)->Initialize(hwnd, winSize, hinstance);
    if (!result) {
        delete *graphics;
        *graphics = nullptr;
        return miny::FAILED;
    }

    return miny::OK;
}

void TerminateGraphics(out_ Graphics** graphics)
{
    if (graphics && *graphics) {
        (*graphics)->Terminate();
        delete *graphics;
        *graphics = nullptr;
    }
}

int MainLoop(in_ CoreSystems* sys)
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
            float blue = 0.5f * (1.f + cosf((float)(t / 1e6)));
            glClearColor(0.f, 0.f, blue, 1.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            game.Render();
            sys->graphics->Present();

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