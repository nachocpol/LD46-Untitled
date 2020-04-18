#pragma once

#include "MathUtils.h"

namespace Waffle
{
	class Graphics;
	class Sprite;
	class Image;
}

class Enemy;
class Bonfire
{
public:
	Bonfire();
	~Bonfire();

	bool Init();
	void Update(float deltaTime);
	void Draw(Waffle::Graphics* graphics);
	Waffle::Transform GetTransform()const;
	void Activate(Waffle::Vec2 position);
	bool IsActive()const;
	void Reset();

private:
	Waffle::Sprite* m_sprite;
	Waffle::Image* m_image;
	bool m_active;
};