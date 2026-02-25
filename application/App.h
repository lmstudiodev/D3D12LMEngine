#pragma once
#include <support/WinInclude.h>
#include <support/DXWindow.h>
#include <debug/DXDebugLayer.h>

class App
{
public:
	App();
	~App();

public:
	bool Init();
	void Update();
	void ShutDown();

	inline bool IsRunning() const { return m_isRunning; }

private:
	bool m_isRunning = false;
};

