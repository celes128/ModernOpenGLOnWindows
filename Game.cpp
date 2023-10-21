#include "Game.h"
#include <cstdio>
#include <iostream>

// Keybinds
const int	KEYBIND_OPEN_CONSOLE	= VK_F1;
const int	KEYBIND_CLOSE			= VK_ESCAPE;

Game::Game()
{
}

miny::Result Game::Initialize(HWND hWnd)
{
	m_hWnd = hWnd;

	return miny::OK;
}

void Game::Shutdown()
{
}

bool Game::Update(u32 dt)
{
	return m_quit;
}

void Game::Render()
{
}

bool Game::HandleKey(WPARAM wParam)
{
	const bool KEY_HANDLED		= true;
	const bool KEY_NOT_HANDLED	= !KEY_HANDLED;

	switch (wParam) {
	case KEYBIND_CLOSE: {
		PostQuitMessage(0);
		m_quit = true;
		return KEY_HANDLED;
	}break;
	}

	return KEY_NOT_HANDLED;
}
