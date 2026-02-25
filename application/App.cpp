#include "App.h"

App::App()
{
}

App::~App()
{
}

bool App::Init()
{
	DXDebugLayer::Get().Init();

	if (DXWindow::Get().Init())
		m_isRunning = true;

	return m_isRunning;
}

void App::Update()
{
	while (!DXWindow::Get().ShouldClose())
	{
		DXWindow::Get().ProcessMessages();

		DXWindow::Get().Update();
	}

	m_isRunning = false;
}

void App::ShutDown()
{
	DXWindow::Get().ShutDown();

	DXDebugLayer::Get().ShutDown();
}
