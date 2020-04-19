#include "Player.h"
#include "Spell.h"
#include "Game.h"

#include "Graphics.h"
#include "Sprite.h"
#include "Input.h"
#include "Image.h"

#include "Enemy.h"

using namespace Waffle;

Player::Player()
	:m_sprite(nullptr)
{
	Reset();
}

Player::~Player()
{
	if (m_sprite)
	{
		delete m_sprite;
	}

	for (const auto spell : m_spellPool)
	{
		if (spell)
		{
			delete spell;
		}
	}
	m_spellPool.clear();
}

bool Player::Init()
{
	m_image = Image::CreateFromFile("data:UhmWiizard.png");
	m_sprite = new Sprite((float)m_image->GetWidth(), (float)m_image->GetHeight(), m_image);
	m_sprite->SetScale(1.5f, 1.5f);

	Image* dropShadowImg = Game::GetDropShadow();
	m_spriteDropShadow = new Sprite((float)dropShadowImg->GetWidth(), (float)dropShadowImg->GetHeight(), dropShadowImg);
	m_spriteDropShadow->SetScale(5.0f, 1.0f);

	// Init spell pool:
	const int spellPoolSize = 256;
	for (int i = 0; i < spellPoolSize; ++i)
	{
		Spell* spell = new Spell;
		spell->Init();
		m_spellPool.push_back(spell);
	}

	Reset();

	return true;
}

void Player::Update(float deltaTime, int numBonfires)
{
	// Movement:
	Vec2 input(0.0f, 0.0f);
	input.X += Input::GetKeyPressed(Key::D) ? 1.0f : 0.0f;
	input.X -= Input::GetKeyPressed(Key::A) ? 1.0f : 0.0f;
	input.Y += Input::GetKeyPressed(Key::W) ? 1.0f : 0.0f;
	input.Y -= Input::GetKeyPressed(Key::S) ? 1.0f : 0.0f;
	float deltaX = input.X * deltaTime * m_moveSpeed;
	float deltaY = input.Y * deltaTime * m_moveSpeed;
	m_sprite->Move(deltaX, deltaY);
	m_spriteDropShadow->Move(deltaX, deltaY);

	if (input.X != 0.0f)
	{
		if (input.X < 0.0f)
		{
			m_sprite->SetScale(1.5f, 1.5f);
		}
		else
		{
			m_sprite->SetScale(-1.5f, 1.5f);
		}
	}

	// Jumps: TODO

	// Spell cast
	bool spaceDown = Input::GetKeyPressed(Key::Space);
	if (spaceDown && (m_holdTimer >  m_maxHoldTime))
	{
		spaceDown = false; // Force to release... 
	}
	if (m_holding && !spaceDown)
	{
		m_holding = false;

		float sizeMod = (m_holdTimer / m_maxHoldTime) * 3.0f; // Increase the size
		
		// But aslo take into account timer! Avoid spamming it.
		if (m_spellTimer > m_spellCD)
		{
			CastSpell(2.5f + sizeMod, numBonfires);
			m_spellTimer = 0.0f;
		}

		m_holdTimer = 0.0f;
	}
	m_spellTimer += deltaTime;

	// Update state and holding timer:
	m_holding = spaceDown;
	if (m_holding)
	{
		m_holdTimer += deltaTime;
	}

	// Update spells:
	for (const auto spell : m_spellPool)
	{
		spell->Update(deltaTime);
	}

	// Update view:
	Graphics::Get().SetView(m_sprite->GetTransform().Position);
}

void Player::Draw(Waffle::Graphics* graphics)
{
	graphics->DrawSprite(m_spriteDropShadow);
	graphics->DrawSprite(m_sprite);

	// Update spells:
	for (const auto spell : m_spellPool)
	{
		spell->Draw(graphics);
	}
}

const std::vector<Spell*>& Player::GetSpells() const
{
	return m_spellPool;
}

void Player::Reset()
{
	m_moveSpeed = 350.0f;

	m_holding = false;
	m_maxHoldTime = 1.5f;
	m_holdTimer = 0.0f;

	m_spellCD = 0.22f;
	m_spellTimer = m_spellCD;

	if (m_sprite)
	{
		m_sprite->SetPosition(0.0f, -150.0f);
		m_spriteDropShadow->SetPosition(2.0f, -195.0f);
	}

	for (const auto spell : m_spellPool)
	{
		spell->Reset();
	}
}

void Player::CastSpell(float size, int numBonfires)
{
	Vec2 pos = m_sprite->GetTransform().Position;

	Vec2 viewport = Graphics::Get().GetCurViewport();
	float mouseX = (float)Input::GetMouseX();
	float mouseY = (float)Input::GetMouseY();
	Vec2 center(viewport.X * 0.5f, viewport.Y * 0.5f);
	
	Vec2 dir = Vec2(mouseX, mouseY) - center;
	dir = Normalize(dir);
	dir.Y = -dir.Y;

	Vec2 dirs[3];
	dirs[0] = dir;	

	// Lets figure out directions in an arc:
	float rot = atan2(dir.Y, dir.X) * RAD_TO_DEG;
	float rotDelta = 15.0f; // Degrees;
	{
		float beta = (rot + rotDelta) * DEG_TO_RAD;
		dirs[1].X = cos(beta);
		dirs[1].Y = sin(beta);
	}
	{
		float beta = (rot - rotDelta) * DEG_TO_RAD;
		dirs[2].X = cos(beta);
		dirs[2].Y = sin(beta);
	}

	int toSpawn = numBonfires;
	for (const auto spell : m_spellPool)
	{
		if (!spell->IsActive())
		{
			Vec2 curDir = dirs[--toSpawn];
			Vec2 offPos = pos + curDir * 50.0f;
			spell->Use(offPos, curDir, 900.0f, size);
		}
		if (toSpawn == 0)
		{
			break;
		}
	}
}
