#include "Bonfire.h"

#include "Graphics.h"
#include "Sprite.h"
#include "Image.h"

#include <cstdio>

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
	
	return true;
}

void Bonfire::Update(float deltaTime)
{
	if (m_active)
	{
		return;
	}
}

void Bonfire::Draw(Graphics* graphics)
{
	if (m_active)
	{
		return;
	}

	graphics->DrawSprite(m_sprite);
}

Transform Bonfire::GetTransform() const
{
	return m_sprite->GetTransform();
}

void Bonfire::Activate(Vec2 position)
{
	if (m_active)
	{
		printf("Bonfire already active!! \n");
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
}
