/*
	Grand Theft CO-OP Launcher
	==========================
		Written by:
			LemonHaze - 2018
	==========================
	https://github.com/gtaco-op/
*/
#include "config.h"
#include "Textures.h"

enum GameSelection {
	GAME_III,
	GAME_VC,
	GAME_SA
};

bool						g_bDrawMainMenu = true;
bool						g_bDrawIIIMenu = false;
bool						g_bDrawVCMenu = false;
bool						g_bDrawSAMenu = false;
bool						g_bIsSetup = false;
bool						g_bShowDebug = false;
bool						g_bIsPlaying = false;
bool						g_IIISetupActive = false;
bool						g_VCSetupActive = false;
bool						g_SASetupActive = false;

int							nBgID = 0;

LPDIRECT3DTEXTURE9			pBgTex;
LPDIRECT3DTEXTURE9			pIIIMenuTex;
LPDIRECT3DTEXTURE9			pVCMenuTex;
LPDIRECT3DTEXTURE9			pSAMenuTex;
LPDIRECT3DTEXTURE9			pSAMenuTex1;
LPDIRECT3DTEXTURE9			pSAMenuTex2;
LPDIRECT3DTEXTURE9			pSAMenuTex3;
LPDIRECT3DTEXTURE9			pSAMenuTex4;
LPDIRECT3DTEXTURE9			pSAMenuTex5;
LPDIRECT3DTEXTURE9			pSAMenuTex6;
LPDIRECT3DTEXTURE9			pSAMenuTex7;
LPDIRECT3DTEXTURE9			pLaunchButtonTex;
LPDIRECT3DTEXTURE9			pLaunchHoverButtonTex;
LPDIRECT3DTEXTURE9			pLaunchButtonBuffer;
LPDIRECT3DTEXTURE9*			ptr = (LPDIRECT3DTEXTURE9*)pLaunchButtonTex;

HWND						g_hWnd = nullptr;
LPDIRECT3DDEVICE9			g_pd3dDevice;
D3DPRESENT_PARAMETERS		g_d3dpp;
ImVec4						originalWindowBg;

CConfiguration*				gConfig = nullptr;

GameSelection				g_GameSelection;

BOOL Execute(char* cmdLine, char* dir, DWORD & exitCode)
{
	PROCESS_INFORMATION processInformation = { 0 };
	STARTUPINFOA startupInfo = { 0 };
	startupInfo.cb = sizeof(startupInfo);
	startupInfo.wShowWindow = SW_SHOWMAXIMIZED;

	BOOL result = CreateProcessA(NULL, cmdLine,
		NULL, NULL, FALSE,
		NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW,
		NULL, dir, &startupInfo, &processInformation);

	if (!result) {
		LPVOID lpMsgBuf;
		DWORD dw = GetLastError();
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);

		MsgBox("Execute() failed at CreateProcess()");
		LocalFree(lpMsgBuf);
		return FALSE;
	}
	else {
		g_bIsPlaying = true;
		WaitForSingleObject(processInformation.hProcess, INFINITE);
		g_bIsPlaying = false;

		result = GetExitCodeProcess(processInformation.hProcess, &exitCode);
		CloseHandle(processInformation.hProcess);
		CloseHandle(processInformation.hThread);

		if (!result) {
			return FALSE;
		}
		return TRUE;
	}
}
void SwitchTo(HWND hWnd)
{
	HWND hPopup = GetWindow(hWnd, GW_ENABLEDPOPUP);
	if (hPopup)
		hWnd = hPopup;

	if (!IsWindow(hWnd) || IsHungAppWindow(hWnd))
		return;

	if (IsIconic(hWnd))
	{
		PostMessage(hWnd, WM_SYSCOMMAND, SC_RESTORE, 0);
		return;
	}

	HWND hForeground = GetForegroundWindow();
	if (hWnd != hForeground)
	{
		BringWindowToTop(hWnd);
		SetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		if (!SetForegroundWindow(hWnd))
		{
			if (!hForeground)
				hForeground = FindWindow(_T("Shell_TrayWnd"), NULL);

			DWORD idForeground = GetWindowThreadProcessId(hForeground, NULL);
			DWORD idTarget = GetWindowThreadProcessId(hWnd, NULL);

			AttachThreadInput(idForeground, idTarget, TRUE);

			if (!SetForegroundWindow(hWnd))
			{
				INPUT inp[4];
				ZeroMemory(&inp, sizeof(inp));
				inp[0].type = inp[1].type = inp[2].type = inp[3].type = INPUT_KEYBOARD;
				inp[0].ki.wVk = inp[1].ki.wVk = inp[2].ki.wVk = inp[3].ki.wVk = VK_MENU;
				inp[0].ki.dwFlags = inp[2].ki.dwFlags = KEYEVENTF_EXTENDEDKEY;
				inp[1].ki.dwFlags = inp[3].ki.dwFlags = KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP;
				SendInput(4, inp, sizeof(INPUT));

				SetForegroundWindow(hWnd);
			}
			AttachThreadInput(idForeground, idTarget, FALSE);
		}
	}
	Sleep(10);
}
void IIIThread()
{
	int nTimesSet = 0;
	DWORD exitCode = NULL;

	ShowWindow(g_hWnd, SW_HIDE);

	std::experimental::filesystem::path p(gConfig->IIILocation);
	Execute(gConfig->IIILocation, (char*)p.parent_path().string().c_str(), exitCode);

	while (g_bIsPlaying == false &&
		GetActiveWindow() != g_hWnd &&
		nTimesSet != 5)
	{
		SwitchTo(g_hWnd);
		ShowWindow(g_hWnd, SW_SHOWMAXIMIZED);
		nTimesSet++;
	}
}
void VCThread()
{
	int nTimesSet = 0;
	DWORD exitCode = NULL;

	ShowWindow(g_hWnd, SW_HIDE);

	std::experimental::filesystem::path p(gConfig->VCLocation);
	Execute(gConfig->VCLocation, (char*)p.parent_path().string().c_str(), exitCode);

	while (g_bIsPlaying == false &&
		GetActiveWindow() != g_hWnd &&
		nTimesSet != 5)
	{
		SwitchTo(g_hWnd);
		ShowWindow(g_hWnd, SW_SHOWMAXIMIZED);
		nTimesSet++;
	}
}
void SAThread()
{
	int nTimesSet = 0;
	DWORD exitCode = NULL;

	ShowWindow(g_hWnd, SW_HIDE);

	std::experimental::filesystem::path p(gConfig->SALocation);
	Execute(gConfig->SALocation, (char*)p.parent_path().string().c_str(), exitCode);

	while (g_bIsPlaying == false &&
		GetActiveWindow() != g_hWnd &&
		nTimesSet != 5)
	{
		SwitchTo(g_hWnd);
		ShowWindow(g_hWnd, SW_SHOWMAXIMIZED);
		nTimesSet++;
	}
}
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
		{
			ImGui_ImplDX9_InvalidateDeviceObjects();
			g_d3dpp.BackBufferWidth = LOWORD(lParam);
			g_d3dpp.BackBufferHeight = HIWORD(lParam);
			HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
			if (hr == D3DERR_INVALIDCALL)
				IM_ASSERT(0);
			ImGui_ImplDX9_CreateDeviceObjects();
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU)
			return 0;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
void Draw()
{
	ImVec4* colors = ImGui::GetStyle().Colors;
	
	// Background Image
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
	
	ImGui::Begin("BG", NULL, ImVec2(0, 0), 0.0f, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoInputs);
	
	switch (nBgID)
	{
		case 0: ImGui::Image((void*)pBgTex,		 ImVec2(APP_RES_HEIGHT, APP_RES_WIDTH)); break;
		case 1: ImGui::Image((void*)pSAMenuTex1, ImVec2(APP_RES_HEIGHT, APP_RES_WIDTH)); break;
		case 2: ImGui::Image((void*)pSAMenuTex2, ImVec2(APP_RES_HEIGHT, APP_RES_WIDTH)); break;
		case 3: ImGui::Image((void*)pSAMenuTex3, ImVec2(APP_RES_HEIGHT, APP_RES_WIDTH)); break;
		case 4: ImGui::Image((void*)pSAMenuTex4, ImVec2(APP_RES_HEIGHT, APP_RES_WIDTH)); break;
		case 5: ImGui::Image((void*)pSAMenuTex5, ImVec2(APP_RES_HEIGHT, APP_RES_WIDTH)); break;
		case 6: ImGui::Image((void*)pSAMenuTex6, ImVec2(APP_RES_HEIGHT, APP_RES_WIDTH)); break;
		case 7: ImGui::Image((void*)pSAMenuTex7, ImVec2(APP_RES_HEIGHT, APP_RES_WIDTH)); break;
	}
	
	ImGui::End();

	{
		// Debug Menu
		if (g_bShowDebug)
		{
			ImGui::Begin("Debug Window", &g_bShowDebug, ImVec2(500, 500), ImGuiWindowFlags_NoSavedSettings);
			{
				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			}
			ImGui::End();
		}

		// Main Menu
		if (g_bDrawMainMenu)
		{
			ImGui::Begin("Main Menu", &g_bDrawMainMenu, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

			originalWindowBg			= colors[ImGuiCol_WindowBg];
			colors[ImGuiCol_WindowBg]	= ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

			ImGui::SetNextWindowPos(ImVec2(19, 640));

			if (ImGui::ImageButton(ptr, ImVec2(500, 100), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1)))
			{
				CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&VCThread, NULL, 0, NULL);
			}

			if (ImGui::IsItemHovered())
				ptr = (LPDIRECT3DTEXTURE9*)pLaunchHoverButtonTex;
			else
				ptr = (LPDIRECT3DTEXTURE9*)pLaunchButtonTex;

			ImGui::End();
		}
	}
}
void LoadTextures()
{
	HRESULT res;

	// Buttons
	if (pLaunchButtonTex == nullptr) {
		res = D3DXCreateTextureFromFileInMemory(g_pd3dDevice, &launcherBannerTexData, sizeof(launcherBannerTexData), &pLaunchButtonTex);
		if (res != D3D_OK) {
			MsgBox("Unable to load texture data. (pLaunchButtonTex == nullptr)");
		}
	}
	if (pLaunchHoverButtonTex == nullptr) {
		res = D3DXCreateTextureFromFileInMemory(g_pd3dDevice, &launcherBannerHoverTexData, sizeof(launcherBannerHoverTexData), &pLaunchHoverButtonTex);
		if (res != D3D_OK) {
			MsgBox("Unable to load texture data. (pLaunchHoverButtonTex == nullptr)");
		}
	}

	// Background Texture
	if (pBgTex == nullptr) {
		res = D3DXCreateTextureFromFileInMemory(g_pd3dDevice, &bgData, sizeof(bgData), &pBgTex);
		if (res != D3D_OK) {
			MsgBox("Unable to load texture data. (pBgTex == nullptr)");
		}
	}

	if (pSAMenuTex1 == nullptr) {
		res = D3DXCreateTextureFromFileInMemory(g_pd3dDevice, &SAOneTexData, sizeof(SAOneTexData), &pSAMenuTex1);
		if (res != D3D_OK) {
			MsgBox("Unable to load texture data. (pBgTex == nullptr)");
		}
	}
	if (pSAMenuTex2 == nullptr) {
		res = D3DXCreateTextureFromFileInMemory(g_pd3dDevice, &SATwoTexData, sizeof(SATwoTexData), &pSAMenuTex2);
		if (res != D3D_OK) {
			MsgBox("Unable to load texture data. (pBgTex == nullptr)");
		}
	}
	if (pSAMenuTex3 == nullptr) {
		res = D3DXCreateTextureFromFileInMemory(g_pd3dDevice, &SAThreeTexData, sizeof(SAThreeTexData), &pSAMenuTex3);
		if (res != D3D_OK) {
			MsgBox("Unable to load texture data. (pBgTex == nullptr)");
		}
	}
	if (pSAMenuTex4 == nullptr) {
		res = D3DXCreateTextureFromFileInMemory(g_pd3dDevice, &SAFourTexData, sizeof(SAFourTexData), &pSAMenuTex4);
		if (res != D3D_OK) {
			MsgBox("Unable to load texture data. (pBgTex == nullptr)");
		}
	}
	if (pSAMenuTex5 == nullptr) {
		res = D3DXCreateTextureFromFileInMemory(g_pd3dDevice, &SAFiveTexData, sizeof(SAFiveTexData), &pSAMenuTex5);
		if (res != D3D_OK) {
			MsgBox("Unable to load texture data. (pBgTex == nullptr)");
		}
	}
	if (pSAMenuTex6 == nullptr) {
		res = D3DXCreateTextureFromFileInMemory(g_pd3dDevice, &SASixTexData, sizeof(SASixTexData), &pSAMenuTex6);
		if (res != D3D_OK) {
			MsgBox("Unable to load texture data. (pBgTex == nullptr)");
		}
	}
	if (pSAMenuTex7 == nullptr) {
		res = D3DXCreateTextureFromFileInMemory(g_pd3dDevice, &SASevenTexData, sizeof(SASevenTexData), &pSAMenuTex7);
		if (res != D3D_OK) {
			MsgBox("Unable to load texture data. (pBgTex == nullptr)");
		}
	}

	// Menu Textures
	if (pIIIMenuTex == nullptr) {
		res = D3DXCreateTextureFromFileInMemory(g_pd3dDevice, &gta3logoData, sizeof(gta3logoData), &pIIIMenuTex);
		if (res != D3D_OK) {
			MsgBox("Unable to load texture data. (pIIIMenuTex == nullptr)");
		}
	}
	if (pVCMenuTex == nullptr) {
		res = D3DXCreateTextureFromFileInMemory(g_pd3dDevice, &VCMenuTexData, sizeof(VCMenuTexData), &pVCMenuTex);
		if (res != D3D_OK) {
			MsgBox("Unable to load texture data. (pVCMenuTex == nullptr)");
		}
	}
	if (pSAMenuTex == nullptr) {
		res = D3DXCreateTextureFromFileInMemory(g_pd3dDevice, &sanAndreasLogoData, sizeof(sanAndreasLogoData), &pSAMenuTex);
		if (res != D3D_OK) {
			MsgBox("Unable to load texture data. (pSAMenuTex == nullptr)");
		}
	}
}
bool ConfigureConfigFile()
{
	char szFile[MAX_PATH] = { 0 }, szDir[MAX_PATH] = { 0 };
	std::string IIIdir, VCdir, SAdir;

	_getcwd(szDir, MAX_PATH);

	OPENFILENAMEA openFn;
	ZeroMemory(&openFn, sizeof(openFn));
	openFn.lStructSize		= sizeof(openFn);
	openFn.hwndOwner		= NULL;
	openFn.lpstrFile		= szFile;
	openFn.lpstrFile[0]		= '\0';
	openFn.nMaxFile			= sizeof(szFile);
	openFn.lpstrFilter		= "Executable Files\0*.exe\0";
	openFn.nFilterIndex		= 1;
	openFn.lpstrFileTitle	= "";
	openFn.nMaxFileTitle	= 0;
	openFn.lpstrInitialDir	= "C:\\";
	openFn.Flags			= OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	GetOpenFileNameA(&openFn);
	IIIdir = szFile;
	memset(szFile, 0, MAX_PATH);

	GetOpenFileNameA(&openFn);
	VCdir = szFile;
	memset(szFile, 0, MAX_PATH);

	GetOpenFileNameA(&openFn);
	SAdir = szFile;
	memset(szFile, 0, MAX_PATH);

	if (IIIdir.empty() && VCdir.empty() && SAdir.empty())
		return false;

	std::string sPath(szDir);
	sPath.append("\\" DEFAULT_CONFIG);

	FILE* f = fopen(sPath.c_str(), "w");
	if (f) {
		char buffer[4096];
		sprintf(buffer, "[General]\nIII Dir = %s\nVC Dir = %s\nSA Dir = %s\n", IIIdir.c_str(), VCdir.c_str(), SAdir.c_str());
		fputs(buffer, f);
		fclose(f);

		printf("[CConfiguration] Created config file.\n");
	}
	else {
		printf("[CConfiguration] Failed to create config file.\n");
		return false;
	}
}

INT WINAPI WinMain(HINSTANCE, HINSTANCE, PSTR, INT)
{
#ifndef _DEBUG_CONSOLE
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
#endif

	srand(time(NULL));
	nBgID = rand() % 7;

	gConfig = new CConfiguration();
	if (!gConfig->IsConfigReadable()) {
		MsgBox("No configuration file found, please locate the directory of the appropriate games in the following windows.\n\nIf you do not have a game installed, press Cancel on the window.");

		printf("[CLauncher] Config could not be read.\n");
		if (!ConfigureConfigFile()) {
			MsgBox("Please specify at least one game directory to continue.\n\nThe application will now exit.");
			return -1;
		}
	}
	else if (gConfig->PopulateValues()) {
		if (gConfig->IsConfigLoaded()) {
			printf("[CLauncher] Config loaded.\n");
		}
	}
	else {
		MsgBox("No game directories were found in the config file, please locate the directory of the appropriate games in the following windows.\n\nIf you do not have a game installed, press Cancel on the window.");

		printf("[CLauncher] Settings could not be populated.\n");
		if (!ConfigureConfigFile()) {
			MsgBox("Please specify at least one game directory to continue.\n\nThe application will now exit.");
			return -1;
		}
	}

	LPDIRECT3D9 pD3D;
	HWND hwnd = nullptr;

	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, LoadCursor(NULL, IDC_ARROW), NULL, NULL, _T(MAIN_APP_TITLE), NULL };
    RegisterClassEx(&wc);

    hwnd = CreateWindowW(_T(MAIN_APP_TITLE), _T(APP_TITLE), WS_EX_TOPMOST | WS_VISIBLE | WS_POPUP, 0, 0, APP_RES_HEIGHT, APP_RES_WIDTH, NULL, NULL, wc.hInstance, NULL);
	if (hwnd == nullptr)
		return -1;

    if ((pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
    {
        UnregisterClassA(MAIN_APP_TITLE, wc.hInstance);
        return 0;
    }
    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	g_d3dpp.BackBufferHeight = APP_RES_HEIGHT;
	g_d3dpp.BackBufferWidth = APP_RES_HEIGHT;
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

#ifdef VSYNC_DISABLED
	g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE; 
#else
	g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE; 
#endif

    if (pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
    {
        pD3D->Release();
        UnregisterClass(_T(MAIN_APP_TITLE), wc.hInstance);
        return 0;
    }


    ImGui_ImplDX9_Init(hwnd, g_pd3dDevice);
	LoadTextures();

    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontDefault();

    ImVec4 clear_color = ImVec4(0.00f, 0.0f, 0.00f, 1.00f);

    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    ShowWindow(hwnd, SW_SHOWMAXIMIZED);
    UpdateWindow(hwnd);

	g_hWnd			= hwnd;
	g_bDrawIIIMenu	= false;
	g_bDrawVCMenu	= false;
	g_bDrawSAMenu	= false;

    while (msg.message != WM_QUIT)    {
        if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }

		ImGui_ImplDX9_NewFrame();
		Draw();
		ImGui::EndFrame();

		g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, false);
		g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
		
		g_pd3dDevice->SetPixelShader(NULL);
		g_pd3dDevice->SetVertexShader(NULL);
		g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, false);
		g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, false);
		g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
		g_pd3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, false);
		g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, true);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
		g_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		g_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

		D3DXMATRIXA16 mat;
		D3DXMatrixIdentity(&mat);
		g_pd3dDevice->SetTransform(D3DTS_WORLD, &mat);
		g_pd3dDevice->SetTransform(D3DTS_VIEW, &mat);
		D3DXMatrixOrthoOffCenterLH(&mat, 0.5f, ImGui::GetIO().DisplaySize.x + 0.5f, ImGui::GetIO().DisplaySize.y + 0.5f, 0.5f, -1.0f, +1.0f);
		g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &mat);
		

        D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x*255.0f), (int)(clear_color.y*255.0f), (int)(clear_color.z*255.0f), (int)(clear_color.w*255.0f));
        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0)
        {
            ImGui::Render();
            g_pd3dDevice->EndScene();
        }

        HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
        {
            ImGui_ImplDX9_InvalidateDeviceObjects();
            g_pd3dDevice->Reset(&g_d3dpp);
            ImGui_ImplDX9_CreateDeviceObjects();
        }
    }

    ImGui_ImplDX9_Shutdown();
    if (g_pd3dDevice) g_pd3dDevice->Release();
    if (pD3D) pD3D->Release();
    UnregisterClassA(MAIN_APP_TITLE, wc.hInstance);

    return 0;
}
