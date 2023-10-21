#pragma once
#include "framework.h"

class Game {
public:
	// (Fixed) Timestep used in the simulation of the world (the Update function)
	static const u64	FIXED_TIMESTEP_MS = 16;						// in milliseconds
	static const u64	FIXED_TIMESTEP_US = 1000 * FIXED_TIMESTEP_MS;	// in microseconds

	Game();

	miny::Result	Initialize(HWND hWnd);
	void			Shutdown();

	// RETURN VALUE
	//	true iff the game needs to quit.
	bool	Update(u32 dt);

	void	Render();

	// RETURN VALUE
	//	true iff the key was handled
	bool	HandleKey(WPARAM wParam);

	bool Quit() const { return m_quit; }

private:
	HWND	m_hWnd{ nullptr };
	bool	m_quit{ false };
};

