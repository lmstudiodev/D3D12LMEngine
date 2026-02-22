#include "DXWindow.h"

bool DXWindow::Init()
{
	WNDCLASSEXW wcex{};
	wcex.cbSize = sizeof(wcex);
	wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = &DXWindow::OnWindowMessage;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = GetModuleHandleW(nullptr);
    wcex.hIcon = LoadIconW(nullptr, IDI_APPLICATION);
    wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wcex.hbrBackground = nullptr;
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"D3D12WndClass ";
    wcex.hIconSm = LoadIconW(nullptr, IDI_APPLICATION);

    m_wndClass = RegisterClassExW(&wcex);

    if (m_wndClass == 0)
    {
        std::cout << "[WIN32] FFailed to register window class !!" << std::endl;

        return false;
    }

    std::cout << "[WIN32] Window class registered !!" << std::endl;

    POINT pos{ 0,0 };
    GetCursorPos(&pos);
    HMONITOR monitor = MonitorFromPoint(pos, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    GetMonitorInfoW(monitor, &monitorInfo);

    m_window = CreateWindowExW(WS_EX_OVERLAPPEDWINDOW | WS_EX_APPWINDOW, 
        (LPCWSTR)m_wndClass, 
        L"LMEngine - a D3D12 based graphic engine", 
        WS_OVERLAPPEDWINDOW | WS_VISIBLE, 
        monitorInfo.rcWork.left + 100, 
        monitorInfo.rcWork.top + 100, 
        1920, 
        1080,
        nullptr,
        nullptr,
        wcex.hInstance,
        nullptr);

    if (m_window == nullptr)
    {
        std::cout << "[WIN32] Failed to create window !!" << std::endl;

        return false;
    }

    std::cout << "[WIN32] Window created !!" << std::endl;

    return true;
}

void DXWindow::Update()
{
    MSG msg;

    while (PeekMessageW(&msg, m_window, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

void DXWindow::ShutDown()
{
    if (m_window)
    {
        DestroyWindow(m_window);
    }
    
    if (m_wndClass)
    {
        UnregisterClassW((LPCWSTR)m_wndClass, GetModuleHandleW(nullptr));
    }
}

LRESULT DXWindow::OnWindowMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CLOSE:
        Get().m_shouldClose = true;
        return 0;
    }
    
    return DefWindowProcW(wnd, msg, wParam, lParam);
}
