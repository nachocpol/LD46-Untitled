#include "Player.h"
#include "Spell.h"

#include "Graphics.h"
#include "Sprite.h"
#include "Input.h"

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
	m_sprite = new Sprite(64, 64);
	m_sprite->SetTint(0.4f, 0.2f, 0.8f);

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

const std::vector<Spell*>& Player::GetSpells() const
{
	return m_spellPool;
}

void Player::Reset()
{
	m_moveSpeed = 350.0f;
	m_spellCD = 0.35f;
	m_spellTimer = m_spellCD;

	if (m_sprite)
	{
		m_sprite->SetPosition(0.0f, 0.0f);
	}

	for (const auto spell : m_spellPool)
	{
		spell->Reset();
	}
}

void Player::CastSpell(float size)
{
	for (const auto spell : m_spellPool)
	{
		if (!spell->IsActive())
		{
			Vec2 pos = m_sprite->GetTransform().Position;

			Vec2 viewport = Graphics::Get().GetCurViewport();
			float mouseX = (float)Input::GetMouseX();
			float mouseY = (float)Input::GetMouseY();

			Vec2 center(viewport.X * 0.5f, viewport.Y * 0.5f);
			Vec2 dir = Vec2(mouseX, mouseY) - center;
			dir = Normalize(dir);
			dir.Y = -dir.Y;

			spell->Use(pos, dir, 1900.0f, 1.0f);
			break;
		}
	}
}
