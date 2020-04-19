#pragma once

#include <vector>

namespace Waffle
{
	class Image;
	class Sprite;
}

class Enemy;
class Player;
class Bonfire;
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
	static Waffle::Image* GetDropShadow();
	
private:
	void Init();
	void Update(float deltaTime);
	void Draw();
	void Reset();

	void SpawnEnemies();

	const int k_maxEnemies = 256;
	const int k_maxRound = 5;
	const int k_maxBonfires = 3;

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

	float m_pauseTimer;

	float m_totalHitPoints;
	float m_curHitPoints;

	// Ground:
	Waffle::Image* m_groundImage;
	Waffle::Sprite* m_groundSprite;

	// UI:
	Waffle::Image* m_controlsImage;
	Waffle::Sprite* m_controlsSrite;
	Waffle::Image* m_logoImage;
	Waffle::Sprite* m_logoSrite; 
	Waffle::Image* m_pauseImage;
	Waffle::Sprite* m_pauseSrite;
	Waffle::Image* m_pressSpaceImage;
	Waffle::Sprite* m_pressSpaceSprite;
	Waffle::Image* m_winImage;
	Waffle::Sprite* m_winSprite;
	Waffle::Image* m_looseImage;
	Waffle::Sprite* m_looseSprite;
};