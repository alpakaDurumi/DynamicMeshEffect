#include "App.h"

// 전방 선언
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// 멤버 함수를 간접적으로 호출하기 위한 전역 포인터
App* g_app = nullptr;

App::App()
    : m_screenWidth(1280), m_screenHeight(720) {
    g_app = this;
}

App::~App() {
    DestroyWindow(m_hWnd);
}

bool App::Initialize() {
    if (!InitWindow()) return false;

    return true;
}

int App::Run() {
    // 윈도우 메시지 처리
    MSG msg = {};
    while (WM_QUIT != msg.message) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            // do Something
        }
    }

    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    return g_app->MsgProc(hWnd, msg, wParam, lParam);
}

LRESULT App::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
        return true;

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

bool App::InitWindow() {
    // DPI 인식 설정
    SetProcessDPIAware();

    // 윈도우 클래스 정보
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = GetModuleHandle(NULL);
    wc.hIcon = nullptr;
    wc.hCursor = nullptr;
    wc.hbrBackground = nullptr;
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = L"DMEWindowClass";
    wc.hIconSm = nullptr;

    // 윈도우 클래스 등록
    if (!RegisterClassEx(&wc)) {
        MessageBox(nullptr, L"Window Registration Failed!", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    // 윈도우 스타일
    DWORD dwStyle = WS_OVERLAPPEDWINDOW;
    DWORD dwExStyle = WS_EX_APPWINDOW;      // 작업 표시줄에 윈도우 표시

    // 윈도우 크기 설정
    // 왼쪽 위 -> 오른쪽 아래 순서
    RECT rect = { 0, 0, m_screenWidth, m_screenHeight };
    AdjustWindowRectEx(&rect, dwStyle, false, dwExStyle);

    // 윈도우 생성
    m_hWnd = CreateWindowEx(
        dwExStyle,
        wc.lpszClassName,
        L"Dynamic Mesh Effect",
        dwStyle,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        rect.right - rect.left,
        rect.bottom - rect.top,
        NULL, NULL, wc.hInstance, NULL);

    if (!m_hWnd) {
        MessageBox(nullptr, L"Window Creation Failed!", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    ShowWindow(m_hWnd, SW_SHOWDEFAULT);
    UpdateWindow(m_hWnd);

    return true;
}