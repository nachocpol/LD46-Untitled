#pragma once

class Player;
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
};