#pragma once

#include "MathUtils.h"

#include <vector>

namespace Waffle
{
	class Graphics;
	class Sprite;
	class Image;
}

class Bonfire;
class Spell;
class Enemy
{
public:
	Enemy();
	~Enemy();

	bool Init();
	void Update(float deltaTime, std::vector<Spell*> spells);
	void Draw(Waffle::Graphics* graphics);

	void Reset();
	bool IsActive()const;
	void Spawn(Waffle::Vec2 position, const std::vector<Bonfire*>& bonfires);

private:
	Waffle::Sprite* m_sprite;
	static Waffle::Image* m_image;

	std::vector<Bonfire*> m_bonfires;
	Bonfire* m_target;
	int m_hitPoints;
	bool m_active;
};