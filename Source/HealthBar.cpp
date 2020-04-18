#include "HealthBar.h"
#include "Bonfire.h"

#include "Graphics.h"
#include "Image.h"
#include "Sprite.h"

using namespace Waffle;

HealthBar::HealthBar()
{
}

HealthBar::~HealthBar()
{
}

bool HealthBar::Init(Bonfire* bonfire, float maxWidth)
{
	m_sprite = new Sprite(maxWidth, 16.0f);
	m_sprite->SetTint(1.0f, 0.0f, 0.0f, 1.0f);

	m_spriteBackground = new Sprite(maxWidth + 6.0f, 16.0f + 4.0f);
	m_spriteBackground->SetTint(0.0f, 0.0f, 0.0f, 1.0f);

	m_maxWidth = maxWidth;

	m_bonfire = bonfire;

	return true;
}

void HealthBar::Update(float deltaTime)
{
	if(!m_bonfire->IsActive())
	{
		return;
	}
}

void HealthBar::Draw(Graphics* graphics)
{
	if (!m_bonfire->IsActive())
	{
		return;
	}

	float size = fmax((float)m_bonfire->GetCurHP() / (float)m_bonfire->GetMaxHP(), 0.1f);

	Vec2 bfPos = m_bonfire->GetTransform().Position;
	m_spriteBackground->SetPosition(bfPos.X, bfPos.Y - 70.0f);
	m_sprite->SetScale(size, 1.0f);
	m_sprite->SetPosition(bfPos.X, bfPos.Y - 70.0f);


	graphics->DrawSprite(m_spriteBackground);
	graphics->DrawSprite(m_sprite);
}

Waffle::Transform HealthBar::GetTransform() const
{
	return Waffle::Transform();
}
