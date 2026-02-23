#pragma once
#include <support/WinInclude.h>
#include <support/ComPointer.h>
#include <D3D12/DXContext.h>

class  DXWindow
{
public:
	DXWindow(const DXWindow&) = delete;
	DXWindow& operator=(const DXWindow&) = delete;

	inline bool shouldClose() const { return m_shouldClose; }

	static constexpr size_t GetFrameCount()
	{
		return 2;
	}

	inline static DXWindow& Get()
	{
		static DXWindow instance;

		return instance;
	}

private:
	DXWindow() = default;

public:
	bool Init();
	void Present();
	void Update();
	void ShutDown();

private:
	static LRESULT CALLBACK OnWindowMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	ATOM m_wndClass = 0;
	HWND m_window = nullptr;

	bool m_shouldClose = false;

	ComPointer<IDXGISwapChain3> m_swapChain;
};