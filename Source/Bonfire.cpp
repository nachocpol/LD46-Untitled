#include "Bonfire.h"

#include "Graphics.h"
#include "Sprite.h"
#include "Image.h"

using namespace Waffle;

Bonfire::Bonfire()
{
}

Bonfire::~Bonfire()
{
}

bool Bonfire::Init()
{
	m_image = Image::CreateFromFile("data:Grid.png");
	m_sprite = new Sprite(64, 64, m_image);
	m_sprite->SetPosition(0.0f, 150.0f);

	return true;
}

void Bonfire::Update(float deltaTime)
{
}

void Bonfire::Draw(Graphics* graphics)
{
	graphics->DrawSprite(m_sprite);
}
