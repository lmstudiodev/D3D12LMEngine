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
        m_width, 
        m_height,
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

    auto& factory = DXContext::Get().GetDXGIFactory();

    DXGI_SWAP_CHAIN_DESC1 scDesc{};
    scDesc.Width = m_width;
    scDesc.Height = m_height;
    scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scDesc.Stereo = false;
    scDesc.SampleDesc.Count = 1;
    scDesc.SampleDesc.Quality = 0;
    scDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scDesc.BufferCount = GetFrameCount();
    scDesc.Scaling = DXGI_SCALING_STRETCH;
    scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    scDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
    scDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

    DXGI_SWAP_CHAIN_FULLSCREEN_DESC scFullScreenDesc{};
    scFullScreenDesc.Windowed = true;

    ComPointer<IDXGISwapChain1> swc;

    factory->CreateSwapChainForHwnd(DXContext::Get().GetCommandQueue(), m_window, &scDesc, &scFullScreenDesc, nullptr, &swc);

    if (!swc.QueryInterface(m_swapChain))
        return false;

    std::cout << "[D3D12] SwapChain created !!" << std::endl;

    return true;
}

void DXWindow::Present()
{
    m_swapChain->Present(1, 0);
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
    m_swapChain.Release();
    
    if (m_window)
    {
        DestroyWindow(m_window);
    }
    
    if (m_wndClass)
    {
        UnregisterClassW((LPCWSTR)m_wndClass, GetModuleHandleW(nullptr));
    }
}

void DXWindow::Resize()
{
    RECT rect;

    if (GetClientRect(m_window, &rect))
    {
        m_width = rect.right - rect.left;
        m_height = rect.bottom - rect.top;

        m_swapChain->ResizeBuffers(GetFrameCount(), 
            m_width, 
            m_height, 
            DXGI_FORMAT_UNKNOWN, 
            DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING);

        m_shouldResize = false;
    }
}

LRESULT DXWindow::OnWindowMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_SIZE:
        if(lParam && (HIWORD(lParam) != Get().m_height || LOWORD(lParam) != Get().m_width))
            Get().m_shouldResize = true;
        break;
    case WM_CLOSE:
        Get().m_shouldClose = true;
        return 0;
    }
    
    return DefWindowProcW(wnd, msg, wParam, lParam);
}
