#pragma once

#include "Graphics/MathUtils.h"

namespace Waffle
{
	class Graphics;
	class Sprite;
	class Image;
}

class Spell
{
public:
	Spell();
	~Spell();

	bool Init();
	void Update(float deltaTime);
	void Draw(Waffle::Graphics* graphics);
	void Reset();

	float GetRadius();
	void Use(Waffle::Vec2 pos, Waffle::Vec2 dir, float speed, float size);
	bool IsActive()const;
	Waffle::Transform GetTransform();


private:
	Waffle::Sprite* m_sprite;
	static Waffle::Image* m_image;

	bool m_active;
	Waffle::Vec2 m_direction;
	float m_speed;
	float m_size;

	int m_seed;
};