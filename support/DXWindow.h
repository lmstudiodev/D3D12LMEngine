#pragma once
#include <support/WinInclude.h>
#include <support/ComPointer.h>
#include <D3D12/DXContext.h>

class  DXWindow
{
public:
	DXWindow(const DXWindow&) = delete;
	DXWindow& operator=(const DXWindow&) = delete;

	inline bool IsFullscreen() const { return m_isFullscreen; }
	inline bool ShouldClose() const { return m_shouldClose; }
	inline bool ShouldResize() const { return m_shouldResize; }
	inline bool UseRayTracing() const { return m_useRayTracing; }

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
	//void Present();
	void Draw();
	void Update();
	void ShutDown();
	void Resize();
	void SetFullscreen(bool enabled);

	//void BeginFrame(ID3D12GraphicsCommandList7* cmdList);
	//void EndFrame(ID3D12GraphicsCommandList7* cmdList);

private:
	static LRESULT CALLBACK OnWindowMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);

	bool CreateRTVDescriptorHeap();
	void CreateRendertargetView(size_t bufferIndex);
	void InitRTVHandles();

	bool GetBuffers();
	void ReleaseBuffers();

	void Present();
	void BeginFrame(ID3D12GraphicsCommandList7* cmdList);
	void EndFrame(ID3D12GraphicsCommandList7* cmdList);

private:
	UINT m_width = 1920;
	UINT m_height = 1080;

	ATOM m_wndClass = 0;
	HWND m_window = nullptr;

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