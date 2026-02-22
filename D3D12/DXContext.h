#pragma once

#include <support/WInInclude.h>
#include <support/ComPointer.h>

class  DXContext
{
public:
	DXContext(const DXContext&) = delete;
	DXContext& operator=(const DXContext&) = delete;

	inline static DXContext& Get()
	{
		static DXContext instance;

		return instance;
	}

	//~DXDebugLayer();

private:
	DXContext() = default;

public:
	bool Init();
	void ShutDown();

	inline ComPointer<ID3D12Device10>& GetDevice() { return m_device; }
	inline ComPointer<ID3D12CommandQueue>& GetCommandQueue() { return m_cmdQueue; }

private:
	ComPointer<ID3D12Device10> m_device;
	ComPointer<ID3D12CommandQueue> m_cmdQueue;
	ComPointer<ID3D12Fence1> m_fence;

	UINT64 m_fenceValue = 0;
};
