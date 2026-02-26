#pragma once
#include <support/WinInclude.h>
#include <support/ComPointer.h>
#include <support/ImageLoader.h>
#include <D3D12/DXContext.h>

class  DXWindow
{
public:
	DXWindow(const DXWindow&) = delete;
	DXWindow& operator=(const DXWindow&) = delete;

	static constexpr size_t FrameCount = 2;
	static constexpr size_t GetFrameCount()
	{
		return FrameCount;
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
	void Update();
	void ProcessMessages();
	void ShutDown();

	inline bool ShouldClose() const { return m_shouldClose; }

private:
	static LRESULT CALLBACK OnWindowMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);

	bool RegisterWindowClass();
	bool CreateMainWindow();
	bool CreateSwapchain();
	bool CreateRTVDescriptorHeap();
	void CreateRendertargetView(size_t bufferIndex);
	void SetFullscreen(bool enabled);
	void InitRTVHandles();

	bool GetBuffers();
	void ReleaseBuffers();

	void BeginFrame(ID3D12GraphicsCommandList7* cmdList);
	void EndFrame(ID3D12GraphicsCommandList7* cmdList);

	void Resize();
	void Present();

private:
	UINT m_width = 1920;
	UINT m_height = 1080;

	ATOM m_wndClass = 0;
	HWND m_window = nullptr;
	HINSTANCE m_wndInstance = nullptr;

	bool m_shouldClose = false;
	bool m_shouldResize = false;
	bool m_isFullscreen = false;
	bool m_useRayTracing = false;

	ComPointer<IDXGISwapChain3> m_swapChain;
	ComPointer<ID3D12Resource2> m_buffers[FrameCount];
	ComPointer<ID3D12DescriptorHeap> m_rtvDescHeap;

	D3D12_CPU_DESCRIPTOR_HANDLE m_retvHandles[FrameCount];

	size_t m_currentBufferIndex = 0;
};