#include <iostream>

#include <support/WInInclude.h>
#include <support/ComPointer.h>
#include <debug/DXDebugLayer.h>

int main()
{
	std::cout << "Welcome to LMEngine - a D3D12 based graphic engine";

	DXDebugLayer::Get().Init();

	DXDebugLayer::Get().ShutDown();

	return 0;
}