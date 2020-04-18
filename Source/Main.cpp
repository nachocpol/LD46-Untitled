
#include "FileUtils.h"

#include "Game.h"

int main()
{
	Waffle::FileSystem::Get().AddFileDevice({ "Projects/LD46/Data/", Waffle::FileDevice::Type::Assets, "data" });

	Game theGame;
	theGame.Run();
	
	return 0;
}