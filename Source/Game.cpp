#include "Game.h"
#include "Player.h"
#include "Bonfire.h"
#include "Enemy.h"
#include "Image.h"
#include "Sprite.h"

#include "Time.h"
#include "Graphics.h"
#include "Input.h"
#include "Logging.h"

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
		ERR("Failed to initialize graphics! \n");
		return;
	}

	graphics.SetRenderScale(1.0f);

	// Init after graphics, as we may load images:
	Init();

	float deltaTime = 0.0f;

	Timer timer;
	timer.Start();

	while (!graphics.Closed())
	{
		graphics.BeginFrame();

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

Image* Game::GetDropShadow()
{
	static Image* kDropShadow = nullptr;
	if (!kDropShadow)
	{
		kDropShadow = Image::CreateFromFile("data:DropShadow.png");
	}
	return kDropShadow;
}

float Game::GetTotalTime()
{
	return g_totalTime;
}

void Game::Init()
{
	m_player = new Player();
	m_player->Init();

	for (int bfi = 0; bfi < k_maxBonfires; ++bfi)
	{
		Bonfire* bf = new Bonfire;
		bf->Init();
		m_bonfires.push_back(bf);
	}
	
	for (int i = 0; i < k_maxEnemies; ++i)
	{
		Enemy* enemy = new Enemy;
		enemy->Init();
		m_enemies.push_back(enemy);
	}

	// Get it now so it creates the particles pool ahead of time.
	Particles::Get();
	
	// Setup ground (static background)
	m_groundImage = Image::CreateFromFile("data:LargeBricksCracked.png");
	float groundNumX = 128;
	float groundNumY = 128;
	m_groundSprite = new Sprite(m_groundImage->GetWidth() * groundNumX, m_groundImage->GetHeight() * groundNumY, m_groundImage);
	m_groundSprite->SetScale(1.0f, 1.0f);
	m_groundSprite->SetImageScaleBias(groundNumX, groundNumY, 0.0f, 0.0f);


	// Setup UI:
	LOAD_IMG_SPRITE("data:PixelControls.png", m_controlsImage, m_controlsSrite);
	m_controlsSrite->SetScale(3.0f, 3.0f);
	m_controlsSrite->SetPosition(0.0f, -50.0f);
	m_controlsSrite->SetIsUI(true);

	LOAD_IMG_SPRITE("data:Frostbite2.png", m_logoImage, m_logoSrite);
	m_logoSrite->SetScale(1.45f, 1.45f);
	m_logoSrite->SetPosition(0.0f, 200.0f);
	m_logoSrite->SetIsUI(true);

	LOAD_IMG_SPRITE("data:PressSpaceToBegin.png", m_pressSpaceImage, m_pressSpaceSprite);
	m_pressSpaceSprite->SetPosition(0.0f, -200.0f);
	m_pressSpaceSprite->SetIsUI(true);

	LOAD_IMG_SPRITE("data:win.png", m_winImage, m_winSprite);
	m_winSprite->SetPosition(0.0f, 0.0f);
	m_winSprite->SetIsUI(true);

	LOAD_IMG_SPRITE("data:lost.png", m_looseImage, m_looseSprite);
	m_looseSprite->SetPosition(0.0f, 0.0f);
	m_looseSprite->SetIsUI(true);

	Reset();
}

void Game::Update(float deltaTime)
{
	switch (m_gameState)
	{
		case State::MainMenu:
		{
			float fadeTime = 2.0f;
			float logoFadeIn = saturate(m_mainMenuTimer / fadeTime);
			logoFadeIn *= logoFadeIn;
			m_logoSrite->SetTint(1.0f, 1.0f, 1.0f, logoFadeIn);
			m_controlsSrite->SetTint(1.0f, 1.0f, 1.0f, logoFadeIn);
			m_pressSpaceSprite->SetTint(1.0f, 1.0f, 1.0f, 0.0f);
			if (m_mainMenuTimer > fadeTime) // Some buffer time so the user doesn't accidentally start.
			{
				m_pressSpaceSprite->SetTint(1.0f, 1.0f, 1.0f, 1.0f);
				if (Input::GetKeyPressed(Key::Space))
				{
					m_logoSrite->SetTint(1.0f, 1.0f, 1.0f, 1.0f);
					m_controlsSrite->SetTint(1.0f, 1.0f, 1.0f, 1.0f);
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

			INFO("Roud prep\n Round num: %i\n Enemies for the round: %i\n", m_curRound, m_enemiesToSpawn);

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
		case State::Defeat:
		{
			m_winLostTimer += deltaTime;
			if (m_winLostTimer > 1.5f &&  Input::GetKeyPressed(Key::Space))
			{
				m_winLostTimer = 0.0f;
				m_logoSrite->SetTint(1.0f, 1.0f, 1.0f, 0.0f);
				m_controlsSrite->SetTint(1.0f, 1.0f, 1.0f, 0.0f);
				m_pressSpaceSprite->SetTint(1.0f, 1.0f, 1.0f, 0.0f);
				Reset();
				m_gameState = State::MainMenu;
			}
			break;
		}
	}

	if (m_gameState == State::Round)
	{
		// Update bonfires:
		int numBonFiresActive = 0;
		for (const auto bonfire : m_bonfires)
		{
			bonfire->Update(deltaTime, m_enemies);
			if (bonfire->IsActive())
			{
				++numBonFiresActive;
			}
		}
		if (numBonFiresActive == 0)
		{
			// Game over.
			m_gameState = State::Defeat;
		}

		m_player->Update(deltaTime, numBonFiresActive);

		// Update the enemies:
		int enemiesAlive = 0;
		for (const auto enemy : m_enemies)
		{
			if (enemy->IsActive())
			{
				++enemiesAlive;
				enemy->Update(deltaTime, m_player->GetSpells());
			}
		}

		// Update particles:
		Particles::Get().Update(deltaTime);

		// Check if the round is over:
		if (m_enemiesToSpawn == 0 && enemiesAlive == 0)
		{
			// Round complete:
			INFO("Round completed! \n");
			m_gameState = State::RoundPrep;
			++m_curRound;
			m_spawnTimer = 0.0f;

			if (m_curRound == 2)
			{
				m_bonfires[1]->Activate(Vec2(-250.0f, -250.0f));
			}
			if (m_curRound == 3)
			{
				m_bonfires[2]->Activate(Vec2( 250.0f, -250.0f));
			}

			// You win!
			if (m_curRound > k_maxRound)
			{
				m_gameState = State::Win;
				INFO("You won! \n");
			}
		}
	}
}

void Game::Draw()
{
	Graphics* graphics = &Graphics::Get();

	// Static background
	if (m_gameState == State::Round || m_gameState == State::RoundPrep || m_gameState == State::Pause)
	{
		graphics->DrawSprite(m_groundSprite);
	}

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

		Particles::Get().Draw(graphics);
	}

	// UI:
	if (m_gameState == State::MainMenu || m_gameState == State::Pause)
	{
		if (m_gameState == State::MainMenu)
		{
			graphics->DrawSprite(m_pressSpaceSprite);
		}
		graphics->DrawSprite(m_logoSrite);
		graphics->DrawSprite(m_controlsSrite);
	}
	else if (m_gameState == State::Win)
	{
		graphics->DrawSprite(m_winSprite);
	}
	else if (m_gameState == State::Defeat)
	{
		graphics->DrawSprite(m_looseSprite);
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

	m_winLostTimer = 0.0f;

	m_enemiesToSpawn = 0;
	m_spawnedEnemies = 0;

	m_player->Reset();

	for (const auto bf : m_bonfires)
	{
		bf->Reset();
	}
	m_bonfires[0]->Activate(Vec2(0.0f, 250.0f));
	
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

	INFO("Spawned: %i enemies \n", numberToSpawn);

	for (const auto enemy : m_enemies)
	{
		if (!enemy->IsActive())
		{
			float px = (((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f) * 7000.0f;
			if (px < 0.0f)
			{
				px -= 800.0f;
			}
			else
			{
				px += 800.0f;
			}

			float py = (((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f) * 7000.0f;
			if (py < 0.0f)
			{
				py -= 800.0f;
			}
			else
			{
				py += 800.0f;
			}

			enemy->Spawn(Vec2(px, py), m_bonfires);

			--numberToSpawn;
			if (numberToSpawn <= 0)
			{
				break;
			}
		}
	}
}
