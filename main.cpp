
#include <support/WinInclude.h>
#include <support/ComPointer.h>
#include <support/DXWindow.h>
#include <debug/DXDebugLayer.h>
#include <D3D12/DXContext.h>

int main()
{
	std::cout << "Welcome to LMEngine - a D3D12 based graphic engine" << std::endl;

	DXDebugLayer::Get().Init();

	if (DXContext::Get().Init() && DXWindow::Get().Init())
	{
		while (!DXWindow::Get().ShouldClose())
		{
			DXWindow::Get().Update();

			if (DXWindow::Get().ShouldResize())
			{
				DXContext::Get().Flush(DXWindow::GetFrameCount());
				DXWindow::Get().Resize();
			}
			
			auto* cmdList = DXContext::Get().InitCommandList();

			DXContext::Get().ExecuteCommandList();

			DXWindow::Get().Present();
		}
		
		DXContext::Get().Flush(DXWindow::GetFrameCount());

		DXWindow::Get().ShutDown();
		DXContext::Get().ShutDown();
	}

	DXDebugLayer::Get().ShutDown();

	return 0;
}