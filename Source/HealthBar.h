#pragma once

#include "MathUtils.h"

namespace Waffle
{
	class Graphics;
	class Sprite;
	class Image;
}

class Bonfire;
class HealthBar
{
public:
	HealthBar();
	~HealthBar();

	bool Init(Bonfire* bonfire, float maxWidth);
	void Update(float deltaTime);
	void Draw(Waffle::Graphics* graphics);
	Waffle::Transform GetTransform()const;

private:
	Waffle::Sprite* m_sprite;
	Waffle::Sprite* m_spriteBackground;
	Waffle::Image* m_image;
	Bonfire* m_bonfire;

	float m_maxWidth;
};