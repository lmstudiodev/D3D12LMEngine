
#include <support/WInInclude.h>
#include <support/ComPointer.h>
#include <debug/DXDebugLayer.h>
#include <D3D12/DXContext.h>

int main()
{
	std::cout << "Welcome to LMEngine - a D3D12 based graphic engine" << std::endl;

	DXDebugLayer::Get().Init();

	if (DXContext::Get().Init())
	{
		while (true)
		{
			auto* cmdList = DXContext::Get().InitCommandList();

			DXContext::Get().ExecuteCommandList();
		}
		
		DXContext::Get().ShutDown();
	}

	DXDebugLayer::Get().ShutDown();

	return 0;
}