#include "Enemy.h"
#include "Spell.h"

#include "Graphics.h"
#include "Sprite.h"
#include "Image.h"
#include "MathUtils.h"

#include <cstdio>
#include <stdlib.h>

using namespace Waffle;

Image* Enemy::m_image = nullptr;

Enemy::Enemy()
	: m_sprite(nullptr)
{
	Reset();
}

Enemy::~Enemy()
{

}

bool Enemy::Init()
{
	if (!m_image)
	{
		m_image = Image::CreateFromFile("data:Grid.png");
	}
	m_sprite = new Sprite(32, 32, m_image);
	m_sprite->SetTint(0.7f, 0.7f, 1.0f);

	return true;
}

void Enemy::Update(float deltaTime, std::vector<Spell*> spells)
{
	if (!m_active)
	{
		return;
	}

	// First check hits with spells:
	bool dead = m_hitPoints == 0;
	for (const auto spell : spells)
	{
		if (spell->IsActive())
		{
			Vec2 spellPos = spell->GetTransform().Position;
			float dist = Length(spellPos - m_sprite->GetTransform().Position);
			if (dist < 55.0f)
			{
				--m_hitPoints;
			}
			dead = m_hitPoints == 0;
			if (dead)
			{
				break;
			}
		}
	}

	if (dead)
	{
		Reset();
	}
	else
	{
		// TODO: move towards target
		// TODO: logic to select other bonfire
	}
}

void Enemy::Draw(Graphics* graphics)
{
	if (!m_active)
	{
		return;
	}

	graphics->DrawSprite(m_sprite);
}

void Enemy::Reset()
{
	m_active = false;
	m_target = nullptr;
	m_hitPoints = 1;
	m_bonfires.clear();
}

bool Enemy::IsActive() const
{
	return m_active;
}

void Enemy::Spawn(Waffle::Vec2 position, const std::vector<Bonfire*>& bonfires)
{
	if (m_active)
	{
		printf("Trying to spawn while still active. \n");
		return;
	}
	m_bonfires = bonfires; // we keep the vector in case one is ded.
	int id = rand() % bonfires.size();
	m_target = m_bonfires[id];
	m_active = true;
	m_sprite->SetPosition(position.X, position.Y);
}
