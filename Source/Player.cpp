#include "Player.h"
#include "Spell.h"

#include "Graphics.h"
#include "Sprite.h"
#include "Input.h"

using namespace Waffle;

Player::Player()
	:m_sprite(nullptr)
	,m_moveSpeed(350.0f)
	,m_spellCD(0.5f)
	,m_spellTimer(m_spellCD)
{
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
	m_sprite = new Sprite(64, 64);
	m_sprite->SetTint(0.4f, 0.2f, 0.8f);

	// Init spell pool:
	const int spellPoolSize = 32;
	for (int i = 0; i < spellPoolSize; ++i)
	{
		Spell* spell = new Spell;
		spell->Init();
		m_spellPool.push_back(spell);
	}

	return true;
}

void Player::Update(float deltaTime)
{
	// Movement:
	Vec2 input(0.0f, 0.0f);
	input.X += Input::GetKeyPressed(Key::D) ? 1.0f : 0.0f;
	input.X -= Input::GetKeyPressed(Key::A) ? 1.0f : 0.0f;
	input.Y += Input::GetKeyPressed(Key::W) ? 1.0f : 0.0f;
	input.Y -= Input::GetKeyPressed(Key::S) ? 1.0f : 0.0f;
	m_sprite->Move(input.X * deltaTime * m_moveSpeed, input.Y * deltaTime * m_moveSpeed);

	// Spell cast
	bool castSpell = Input::GetKeyPressed(Key::Space);
	if(castSpell && (m_spellTimer >= m_spellCD))
	{
		CastSpell(1.0f);
		m_spellTimer = 0.0f;
	}
	else
	{
		m_spellTimer += deltaTime;
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
	graphics->DrawSprite(m_sprite);

	// Update spells:
	for (const auto spell : m_spellPool)
	{
		spell->Draw(graphics);
	}
}

void Player::CastSpell(float size)
{
	for (const auto spell : m_spellPool)
	{
		if (!spell->IsActive())
		{
			Vec2 pos = m_sprite->GetTransform().Position;
			Vec2 dir(1.0f, 0.0f);
			spell->Use(pos, dir, 50.0f, 1.0f);
			break;
		}
	}
}
