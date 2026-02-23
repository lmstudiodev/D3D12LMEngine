#pragma once

#include <support/WinInclude.h>
#include <support/ComPointer.h>

class  DXDebugLayer
{
public:
	 DXDebugLayer(const DXDebugLayer&) = delete;
	 DXDebugLayer& operator=(const DXDebugLayer&) = delete;

	 inline static DXDebugLayer& Get()
	 {
		 static DXDebugLayer instance;

		 return instance;
	 }

private:
	DXDebugLayer() = default;

public:
	bool Init();
	void ShutDown();

private:
#ifdef _DEBUG
	ComPointer<ID3D12Debug6> m_d3d12Debug;
	ComPointer<IDXGIDebug1> m_dxgiDebug;
#endif

};