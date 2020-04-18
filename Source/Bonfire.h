#pragma once

#include "MathUtils.h"

namespace Waffle
{
	class Graphics;
	class Sprite;
	class Image;
}

class Bonfire
{
public:
	Bonfire();
	~Bonfire();

	bool Init();
	void Update(float deltaTime);
	void Draw(Waffle::Graphics* graphics);
	Waffle::Transform GetTransform()const;

private:
	Waffle::Sprite* m_sprite;
	Waffle::Image* m_image;
};