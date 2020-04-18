#pragma once

class Player;
class Bonfire;
class Enemy;

#include <vector>

class Game
{
public: 
	struct State
	{
		enum T
		{
			MainMenu,
			Pause,
			RoundPrep,
			Round,
			Win,
			Defeat,
			COUNT
		};
	};

	Game();
	~Game();
	void Run();
	
private:
	void Init();
	void Update(float deltaTime);
	void Draw();
	void Reset();

	void SpawnEnemies();

	const int k_maxEnemies = 256;
	const int k_maxRound = 5;

	Player* m_player;
	std::vector<Bonfire*>  m_bonfires;
	std::vector<Enemy*> m_enemies; // All enemies (even in-active ones).
	State::T m_gameState;
	float m_mainMenuTimer;
	int m_curRound;

	int m_enemiesToSpawn;
	int m_spawnedEnemies;

	float m_spawnCD;
	float m_spawnTimer;
	float m_totalRoundTime;
};