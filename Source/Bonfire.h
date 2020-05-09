#pragma once

#include "Graphics/MathUtils.h"

#include <vector>

namespace Waffle
{
	class Graphics;
	class Sprite;
	class Image;
}

class HealthBar;
class Enemy;
class Bonfire
{
public:
	Bonfire();
	~Bonfire();

	bool Init();
	void Update(float deltaTime, const std::vector<Enemy*> enemies);
	void Draw(Waffle::Graphics* graphics);
	float GetRadius();
	Waffle::Transform GetTransform()const;
	void Activate(Waffle::Vec2 position);
	bool IsActive()const;
	void Reset();
	int GetMaxHP()const;
	int GetCurHP()const;

private:
	Waffle::Sprite* m_sprite;
	std::vector<Waffle::Image*> m_frames;
	int m_curFrame;
	float m_lastTime;
	Waffle::Sprite* m_shadowSprite;

	HealthBar* m_healthBar;

	bool m_active;

	int m_maxHits;
	int m_curHits;
};