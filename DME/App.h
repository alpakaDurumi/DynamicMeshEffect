#pragma once

#include <iostream>

#include <windows.h>
#include <wrl.h>

#include <d3d11.h>
#include <d3dcompiler.h>

#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

using Microsoft::WRL::ComPtr;

class App {
public:
	App();
	~App();

	bool Initialize();

	int Run();

	LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
private:
    bool InitWindow();

    int m_screenWidth;
    int m_screenHeight;
	HWND m_hWnd = nullptr;
};