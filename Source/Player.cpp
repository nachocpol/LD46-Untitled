#include "Player.h"

#include "Graphics.h"
#include "Sprite.h"

using namespace Waffle;

Player::Player()
	:m_sprite(nullptr)
{
}

Player::~Player()
{
	if (m_sprite)
	{
		delete m_sprite;
	}
}

bool Player::Init()
{
	m_sprite = new Sprite(64, 64);
	m_sprite->SetTint(0.4f, 0.2f, 0.8f);

	return false;
}

void Player::Update(float deltaTime)
{

}

void Player::Draw(Waffle::Graphics* graphics)
{
	graphics->DrawSprite(m_sprite);
}
