#include "Game.h"
#include "Player.h"
#include "Bonfire.h"

#include "Time.h"
#include "Graphics.h"

#include <cstdio>

using namespace Waffle;

Game::Game()
	:m_player(nullptr)
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

}

void Game::Update(float deltaTime)
{
	m_player->Update(deltaTime);

	for (const auto bonfire : m_bonfires)
	{
		bonfire->Update(deltaTime);
	}
}

void Game::Draw()
{
	Graphics* graphics = &Graphics::Get();

	for (const auto bonfire : m_bonfires)
	{
		bonfire->Draw(graphics);
	}
	m_player->Draw(graphics);
}
