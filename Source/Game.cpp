#include "Game.h"
#include "Player.h"
#include "Bonfire.h"
#include "Enemy.h"

#include "Time.h"
#include "Graphics.h"
#include "Input.h"

#include <cstdio>

using namespace Waffle;

Game::Game()
	:m_player(nullptr)
	,m_gameState(State::MainMenu)
{
}

Game::~Game()
{
}

void Game::Run()
{
	Graphics& graphics = Graphics::Get();
	if (!graphics.Init())
	{
		printf("Failed to initialize graphics! \n");
		return;
	}

	// Init after graphics, as we may load images:
	Init();

	float totalTime = 0.0f;
	float deltaTime = 0.0f;

	Timer timer;
	timer.Start();

	while (!graphics.Closed())
	{
		timer.Start();

		// Update phase:
		Update(deltaTime);

		// Drawing phase:
		graphics.ClearScreen(0.0f, 0.0f, 0.0f, 1.0f);
		Draw();
		graphics.Flush();

		// Update frame time:
		deltaTime = timer.Stop() / 1000.0f;
		totalTime += deltaTime;
	}
}

void Game::Init()
{
	m_player = new Player();
	m_player->Init();

	Bonfire* bf = new Bonfire;
	bf->Init();
	m_bonfires.push_back(bf);

	for (int i = 0; i < k_maxEnemies; ++i)
	{
		Enemy* enemy = new Enemy;
		enemy->Init();
		m_enemies.push_back(enemy);
	}
	
	Reset();
}

void Game::Update(float deltaTime)
{
	switch (m_gameState)
	{
		case State::MainMenu:
		{
			if (m_mainMenuTimer > 2.5f) // Some buffer time so the user doesn't accidentally start.
			{
				if (Input::GetKeyPressed(Key::Space))
				{
					m_gameState = State::RoundPrep;
					m_mainMenuTimer = 0.0f;
				}
			}
			else
			{
				m_mainMenuTimer += deltaTime;
			}
			break;
		}

		case State::Pause:
		{
			break;
		}

		case State::RoundPrep:
		{
			// Figure out how many enemies to spawn, this could be non linear aka: roundT * rountT.
			float roundT = (float)m_curRound / (float)k_maxRound;
			m_enemiesToSpawn = (int)clamp((float)k_maxEnemies * roundT, 0.0f, (float)k_maxEnemies);
			
			m_gameState = State::Round;

			printf("Roud prep\n Round num: %i\n Enemies for the round: %i\n", m_curRound, m_enemiesToSpawn);

			break;
		}
		case State::Round:
		{
			m_totalRoundTime += deltaTime;

			// Check if we need to spawn another pack of enemies:
			if (m_enemiesToSpawn != 0 && m_spawnTimer >= m_spawnCD)
			{
				m_spawnTimer = 0.0f;				
				SpawnEnemies();
			}
			else
			{
				m_spawnTimer += deltaTime;
			}


			break;
		}
		case State::Win:
		{

			break;
		}
		case State::Defeat:
		{

			break;
		}
	}

	if (m_gameState == State::Round)
	{
		m_player->Update(deltaTime);

		for (const auto bonfire : m_bonfires)
		{
			bonfire->Update(deltaTime);
		}

		for (const auto enemy : m_enemies)
		{
			enemy->Update(deltaTime, m_player->GetSpells());
		}
	}
}

void Game::Draw()
{
	Graphics* graphics = &Graphics::Get();

	if (m_gameState == State::Round)
	{
		for (const auto bonfire : m_bonfires)
		{
			bonfire->Draw(graphics);
		}

		for (const auto enemy : m_enemies)
		{
			enemy->Draw(graphics);
		}

		m_player->Draw(graphics);
	}
}

void Game::Reset()
{
	m_curRound = 1;
	m_mainMenuTimer = 0.0f;
	m_gameState = State::MainMenu;

	m_spawnCD = 10.0f;
	m_spawnTimer = 0.0f; // init this at 0, so we don't start spawning 
	m_totalRoundTime = 0.0f;

	m_enemiesToSpawn = 0;
	m_spawnedEnemies = 0;

	m_player->Reset();

	for (const auto bf : m_bonfires)
	{
		//bf->Reset();
	}

	for (const auto enemy : m_enemies)
	{
		enemy->Reset();
	}
}

void Game::SpawnEnemies()
{
	int numberToSpawn = 10;
	if (m_enemiesToSpawn <= numberToSpawn)
	{
		numberToSpawn = m_enemiesToSpawn; // This is not ideal, we could spawn 1 enemy only :/
		m_enemiesToSpawn = 0;
	}
	else
	{
		m_enemiesToSpawn -= numberToSpawn;
	}

	printf("Spawned: %i enemies \n", numberToSpawn);

	for (const auto enemy : m_enemies)
	{
		if (!enemy->IsActive())
		{
			float px = (((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f) * 1000.0f;
			px = __min(px, 500.0f);
			float py = (((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f) * 1000.0f;
			py = __min(py, 500.0f);

			enemy->Spawn(Vec2(px, py), m_bonfires);

			--numberToSpawn;
			if (numberToSpawn <= 0)
			{
				break;
			}
		}
	}
}
