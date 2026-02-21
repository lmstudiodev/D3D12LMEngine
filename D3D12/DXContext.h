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

private:
	ComPointer<ID3D12Device10> m_device;
};
