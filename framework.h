#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclure les en-têtes Windows rarement utilisés
#include "../miny/types.h"

// Fichiers d'en-tête Windows
#include <windows.h>
#include <windowsx.h>	// GET_X_LPARAM, GET_Y_LPARAM

// Fichiers d'en-tête C RunTime
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <cassert>