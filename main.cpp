
#include <support/WinInclude.h>

#include <application/App.h>

int main()
{
	std::cout << "Welcome to LMEngine - a D3D12 based graphic engine" << std::endl;

	App app;

	if (app.Init())
	{
		while (app.IsRunning())
		{
			app.Update();
		}

		app.ShutDown();
	}

	return 0;
}