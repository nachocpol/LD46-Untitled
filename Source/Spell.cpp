#include "Spell.h"

#include "Graphics.h"
#include "Sprite.h"
#include "Image.h"

#include "MathUtils.h"

#include <cstdio>

using namespace Waffle;

Image* Spell::m_image = nullptr;

Spell::Spell()
	: m_sprite(nullptr)
{
	Reset();
}

Spell::~Spell()
{
	if (m_image)
	{
		delete m_image;
	}
	if (m_sprite)
	{
		delete m_sprite;
	}
}

bool Spell::Init()
{
	if (m_image)
	{
		m_image = Image::CreateFromFile("data:Grid.png");
	}

	m_sprite = new Sprite(32, 32, m_image);
	m_sprite->SetTint(1.0f, 0.0f, 1.0f);

	return true;
}

void Spell::Update(float deltaTime)
{
	if (!m_active)
	{
		return;
	}

	Vec2 pos = m_sprite->GetTransform().Position;
	Vec2 delta = pos + m_direction * (m_speed * deltaTime);
	m_sprite->SetPosition(delta.X, delta.Y);

	Vec2 view = Graphics::Get().GetView();
	Vec2 viewSpacePos = pos - view;
	float distFromOrigin = Length(viewSpacePos);
	if (distFromOrigin > 5000.0f)
	{
		Reset();
	}

}

void Spell::Draw(Graphics* graphics)
{
	if (!m_active)
	{
		return;
	}
	graphics->DrawSprite(m_sprite);
}

void Spell::Reset()
{
	m_active = false;
	m_direction = Vec2(0.0f, 0.0f);
	m_speed = 0.0f;
	m_size = 0.0f;
}

void Spell::Use(Waffle::Vec2 pos, Vec2 dir, float speed, float size)
{
	if (m_active)
	{
		printf("Trying to use spell while still active!!!\n");
		return;
	}
	m_direction = dir;
	m_speed = speed;
	m_size = size;
	m_active = true;

	m_sprite->SetPosition(pos.X, pos.Y);
}

bool Spell::IsActive() const
{
	return m_active;
}

Transform Spell::GetTransform()
{
	return m_sprite->GetTransform();
}
