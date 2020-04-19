#include "Bonfire.h"
#include "HealthBar.h"
#include "Enemy.h"
#include "Game.h"

#include "Graphics.h"
#include "Sprite.h"
#include "Image.h"
#include "Logging.h"

using namespace Waffle;

Bonfire::Bonfire()
	: m_active(false)
	, m_sprite(nullptr)
{
	Reset();
}

Bonfire::~Bonfire()
{
}

bool Bonfire::Init()
{
	m_frames.resize(4);
	for (int frame = 0; frame < 4; ++frame)
	{
		char name[64];
		sprintf_s(name, 64, "data:Bonfire/sprite%i.png", frame + 1);
		m_frames[frame] = Image::CreateFromFile(name);
	}

	float w = (float)m_frames[0]->GetWidth();
	float h = (float)m_frames[0]->GetHeight();
	m_sprite = new Sprite(w, h, m_frames[0]);
	m_sprite->SetScale(3.5f, 3.5f);
	
	Image* shadowImage = Game::GetDropShadow();
	m_shadowSprite = new Sprite((float)shadowImage->GetWidth(), (float)shadowImage->GetHeight(), shadowImage);
	m_shadowSprite->SetScale(6.5f, 0.95f);

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
					m_curFrame = 0;
					m_active = false; // cant reset, or we will show as full HP
					break;
				}
			}
		}
	}

	// Advance anim:
	float time = Game::GetTotalTime();
	if ((time - m_lastTime) > 0.15f)
	{
		m_curFrame = (++m_curFrame % 3) + 1;
		m_lastTime = time;
	}

	m_healthBar->Update(deltaTime);
}

void Bonfire::Draw(Graphics* graphics)
{
	graphics->DrawSprite(m_shadowSprite);

	// Always render.. 
	m_sprite->SetImage(m_frames[m_curFrame]);
	graphics->DrawSprite(m_sprite);

	// Health bar only if active
	if (m_active)
	{
		m_healthBar->Draw(graphics);
	}
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
	m_shadowSprite->SetPosition(position.X, position.Y - 50.0f);
	m_curFrame = 1;
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
	m_curFrame = 0;
	m_lastTime = 0.0f;
	if (m_sprite)
	{
		m_sprite->SetPosition(1000.0f, 10000.0f);// aggg. this is a hack, can't disable the sprite (want to render the rocks only!)
		m_shadowSprite->SetPosition(1000.0f, 10000.0f);
	}
}

int Bonfire::GetMaxHP() const
{
	return m_maxHits;
}

int Bonfire::GetCurHP() const
{
	return m_curHits;
}
