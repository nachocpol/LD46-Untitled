#include "Game.h"
#include "Player.h"
#include "Bonfire.h"
#include "Enemy.h"
#include "Image.h"
#include "Sprite.h"

#include "Time.h"
#include "Graphics.h"
#include "Input.h"

#include <cstdio>

using namespace Waffle;

#define LOAD_IMG_SPRITE(path,img,spr) img = Image::CreateFromFile(path); spr = new Sprite((float)img->GetWidth(),(float)img->GetHeight(),img);

Game::Game()
	:m_player(nullptr)
	,m_gameState(State::MainMenu)
{
}

Game::~Game()
{
}

float g_totalTime = 0.0f;

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
		g_totalTime += deltaTime;
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
	
	// Setup ground (static background)
	m_groundImage = Image::CreateFromFile("data:GroundCracks.png");
	float groundNumX = 128;
	float groundNumY = 128;
	m_groundSprite = new Sprite(m_groundImage->GetWidth() * groundNumX, m_groundImage->GetHeight() * groundNumY, m_groundImage);
	m_groundSprite->SetImageScaleBias(groundNumX, groundNumY, 0.0f, 0.0f);


	// Setup UI:
	LOAD_IMG_SPRITE("data:PixelControls.png", m_controlsImage, m_controlsSrite);
	m_controlsSrite->SetScale(3.0f, 3.0f);
	m_controlsSrite->SetPosition(0.0f, -50.0f);
	m_controlsSrite->SetIsUI(true);

	LOAD_IMG_SPRITE("data:Frostbite_white.png", m_logoImage, m_logoSrite);
	m_logoSrite->SetScale(1.25f, 1.25f);
	m_logoSrite->SetPosition(0.0f, 200.0f);
	m_logoSrite->SetIsUI(true);

	LOAD_IMG_SPRITE("data:PixelControls.png", m_pauseImage, m_pauseSrite);

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
			if (m_pauseTimer >= 0.35f)
			{
				if (Input::GetKeyPressed(Key::ESC))
				{
					m_gameState = State::Round;
					m_pauseTimer = 0.0f;
				}
			}
			else
			{
				m_pauseTimer += deltaTime;
			}

			deltaTime = 0.0f; // Pause!
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

			// Check pause:
			if (m_pauseTimer >= 0.35f)
			{
				if (Input::GetKeyPressed(Key::ESC))
				{
					m_pauseTimer = 0.0f;
					m_gameState = State::Pause;
				}
			}
			else
			{
				m_pauseTimer += deltaTime;
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

		int enemiesAlive = 0;
		for (const auto enemy : m_enemies)
		{
			if (enemy->IsActive())
			{
				++enemiesAlive;
				enemy->Update(deltaTime, m_player->GetSpells());
			}
		}

		for (const auto bonfire : m_bonfires)
		{
			bonfire->Update(deltaTime);
		}

		if (m_enemiesToSpawn == 0 && enemiesAlive == 0)
		{
			// Round complete:
			printf("Round completed! \n");
			m_gameState = State::RoundPrep;
			++m_curRound;
			m_spawnTimer = 0.0f;

			// You win!
			if (m_curRound > k_maxRound)
			{
				m_gameState = State::Win;
				printf("You won! \n");
			}
		}
	}
}

void Game::Draw()
{
	Graphics* graphics = &Graphics::Get();

	// Always render static background:
	graphics->DrawSprite(m_groundSprite);

	// Main render
	if (m_gameState == State::Round || m_gameState == State::Pause)
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

	// UI:
	if (m_gameState == State::MainMenu || m_gameState == State::Pause || m_gameState == State::Win)
	{
		graphics->DrawSprite(m_logoSrite);
		graphics->DrawSprite(m_controlsSrite);
	}
}

void Game::Reset()
{
	m_curRound = 1;
	m_mainMenuTimer = 0.0f;
	m_gameState = State::MainMenu;

	m_spawnCD = 5.0f;
	m_spawnTimer = 0.0f; // init this at 0, so we don't start spawning 
	m_totalRoundTime = 0.0f;

	m_pauseTimer = 0.0f;

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
	int numberToSpawn = 15 * m_curRound;
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
			float px = (((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f) * 10000.0f;
			px = __min(px, 800.0f);
			float py = (((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f) * 10000.0f;
			py = __min(py, 800.0f);

			enemy->Spawn(Vec2(px, py), m_bonfires);

			--numberToSpawn;
			if (numberToSpawn <= 0)
			{
				break;
			}
		}
	}
}
