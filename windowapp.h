#pragma once

#include "resource.h"
#include "framework.h"
#include "Graphics.h"

#define MAX_LOADSTRING 100

#define CLOSE_APPLICATION (-1)

// The core systems is made of the window, the graphics system and the sound system.
struct CoreSystems {
	HINSTANCE	hinstance{ nullptr };
	HWND		hwnd{ nullptr };
	SIZE		winSize{ 0,0 };
	Graphics*	graphics{ nullptr };
};

miny::Result	InitializeCoreSystems(in_ HINSTANCE hinstance, in_ int nCmdShow, out_ CoreSystems *sys);
void			TerminateCoreSystems(in_ out_ CoreSystems *sys);

miny::Result	CreateTheWindow(in_ SIZE size, in_ HINSTANCE hinstance, out_ HWND* hwnd);
void			DestroyTheWindow(in_ out_ HWND* hwnd);

miny::Result	InitializeGraphics(in_ HWND hwnd, in_ SIZE winSize, in_ HINSTANCE hinstance, out_ Graphics** graphics);
void			TerminateGraphics(out_ Graphics** graphics);

LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
int					MainLoop(in_ CoreSystems *sys);

inline void HideWindow(HWND hwnd)
{
	ShowWindow(hwnd, SW_HIDE);
}

inline int Width(const RECT& r)
{
	return r.right - r.left;
}

inline int Height(const RECT& r)
{
	return r.bottom - r.top;
}
