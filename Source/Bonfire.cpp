#include "Bonfire.h"
#include "HealthBar.h"
#include "Enemy.h"

#include "Graphics.h"
#include "Sprite.h"
#include "Image.h"
#include "Logging.h"

using namespace Waffle;

Bonfire::Bonfire()
	: m_active(false)
{
	Reset();
}

Bonfire::~Bonfire()
{
}

bool Bonfire::Init()
{
	m_image = Image::CreateFromFile("data:Grid.png");
	m_sprite = new Sprite(64, 64, m_image);
	
	m_healthBar = new HealthBar;
	m_healthBar->Init(this, 150.0f);

	return true;
}

void Bonfire::Update(float deltaTime, const std::vector<Enemy*> enemies)
{
	if (!m_active)
	{
		return;
	}

	for (const auto enemy : enemies)
	{
		if (enemy->IsActive() && enemy->GetTarget() == this) // Only check if this is the target.. try to make it more fair!
		{
			Vec2 toEnemy = enemy->GetTransform().Position - m_sprite->GetTransform().Position;
			float dist = Length(toEnemy);
			dist = dist - (GetRadius() + enemy->GetRadius());
			if (dist < -10.0f) // Fudge  factor..
			{
				enemy->TakeHit(); // Also kill the  enemy.
				--m_curHits;
				if (m_curHits <= 0)
				{
					m_active = false; // cant reset, or we will show as full HP
					break;
				}
			}
		}
	}

	m_healthBar->Update(deltaTime);
}

void Bonfire::Draw(Graphics* graphics)
{
	if (!m_active)
	{
		return;
	}

	graphics->DrawSprite(m_sprite);
	m_healthBar->Draw(graphics);
}

float Bonfire::GetRadius()
{
	Vec2 spriteScale = m_sprite->GetTransform().Scale;
	Vec2 spriteSize = m_sprite->GetSize();
	spriteSize.X *= spriteScale.X * 0.5f;
	spriteSize.Y *= spriteScale.Y * 0.5f;
	float radius = sqrt(spriteSize.X * spriteSize.X + spriteSize.Y * spriteSize.Y);
	return radius;
}

Transform Bonfire::GetTransform() const
{
	return m_sprite->GetTransform();
}

void Bonfire::Activate(Vec2 position)
{
	if (m_active)
	{
		ERR("Bonfire already active!! \n");
		return;
	}
	m_active = true;
	m_sprite->SetPosition(position.X, position.Y);
}

bool Bonfire::IsActive() const
{
	return m_active;
}

void Bonfire::Reset()
{
	m_active = false;
	m_maxHits = 20;
	m_curHits = m_maxHits;
}

int Bonfire::GetMaxHP() const
{
	return m_maxHits;
}

int Bonfire::GetCurHP() const
{
	return m_curHits;
}
