#pragma once

class Player;
class Bonfire;

#include <vector>

class Game
{
public: 
	Game();
	~Game();
	
	void Run();

private:
	void Init();
	void Update(float deltaTime);
	void Draw();

	Player* m_player;
	std::vector<Bonfire*>  m_bonfires;
};