#include "FileUtils.h"
#include "Logging.h"

#include "Game.h"
#include "Window.h"

#include <windows.h>

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	g_hinstance = hInstance;
	g_cmdShow = nCmdShow;

	Waffle::FileSystem::Get().AddFileDevice({ "Projects/LD46/Data/", Waffle::FileDevice::Type::Assets, "data" });

	Game theGame;
	theGame.Run();

	Logger::Get()->FlushToFile();

	return 0;
}