/*
	Grand Theft CO-OP Launcher
	==========================
		Written by: 
			LemonHaze - 2018
	==========================
	https://github.com/gtaco-op/
*/
#pragma once
#define DIRECTINPUT_VERSION 0x0800

#include <imgui.h>
#include "imgui_impl_dx9.h"

#include "inih\INIReader.h"

#include <vector>
#include <d3d9.h>
#include <d3dx9.h>
#include <dinput.h>
#include <tchar.h>
#include <TlHelp32.h>

#include <sstream>
#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <cassert>
#include <time.h>
#include <algorithm>
#include <cstdlib>
#include <sstream>
#include <list>
#include <experimental\filesystem>

#if defined(WIN32)
#include <windows.h>
#include <direct.h>
#include <tchar.h>
#include <stdlib.h>
#include <io.h>  
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#define GetCurrentDir getcwd
#endif

#include "CConfiguration.h"

#define MAIN_APP_TITLE				"Grand Theft CO-OP"
#define APP_VERSION					"0.0.2a"
#define APP_TITLE					MAIN_APP_TITLE " Launcher " APP_VERSION
#define MsgBox(a)					MessageBoxA(g_hWnd, a, APP_TITLE, MB_OK)
#define DEFAULT_CONFIG				"config.ini"

// Uncomment to disable vsync
//#define VSYNC_DISABLED

// Default resolution
#define APP_RES_HEIGHT				1920
#define APP_RES_WIDTH				1080

extern bool							g_bDrawMainMenu;
extern bool							g_bDrawIIIMenu;
extern bool							g_bDrawVCMenu;
extern bool							g_bDrawSAMenu;
extern bool							g_bIsSetup;
extern CConfiguration*				gConfig;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);