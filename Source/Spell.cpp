#include "Spell.h"
#include "Enemy.h"

#include "Graphics.h"
#include "Sprite.h"
#include "Image.h"
#include "Logging.h"
#include "MathUtils.h"

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
	if (!m_image)
	{
		m_image = Image::CreateFromFile("data:ArcaneBall.png");
	}

	m_sprite = new Sprite((float)m_image->GetWidth(), (float)m_image->GetHeight(), m_image);
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

	m_sprite->Rotate((2500.0f * deltaTime)); // Not sure why I have to put that size...

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
	m_seed = rand();
}

float Spell::GetRadius()
{
	Vec2 spriteScale = m_sprite->GetTransform().Scale;
	Vec2 spriteSize = m_sprite->GetSize();
	spriteSize.X *= spriteScale.X * 0.5f;
	spriteSize.Y *= spriteScale.Y * 0.5f;
	float radius = sqrt(spriteSize.X * spriteSize.X + spriteSize.Y * spriteSize.Y);
	return radius;
}

void Spell::Use(Waffle::Vec2 pos, Vec2 dir, float speed, float size)
{
	if (m_active)
	{
		ERR("Trying to use spell while still active!!!\n");
		return;
	}
	m_direction = dir;
	m_speed = speed;
	m_active = true;

	m_sprite->SetPosition(pos.X, pos.Y);
	m_sprite->SetScale(size, size);
}

bool Spell::IsActive() const
{
	return m_active;
}

Transform Spell::GetTransform()
{
	return m_sprite->GetTransform();
}
