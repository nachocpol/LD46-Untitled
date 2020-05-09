#pragma once

#include "Graphics/MathUtils.h"

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
	float GetRadius();
	void Reset();
	bool IsActive()const;
	void Spawn(Waffle::Vec2 position, const std::vector<Bonfire*>& bonfires);
	const Bonfire* GetTarget()const;
	Waffle::Transform GetTransform();
	void TakeHit(bool bonfire);

private:
	void FindTarget();
	Waffle::Sprite* m_sprite;
	Waffle::Sprite* m_spriteDropShadow;
	static Waffle::Image* m_image;

	std::vector<Bonfire*> m_bonfires;
	Bonfire* m_target;
	int m_hitPoints;
	bool m_active;
	int m_seed;
};

class Particles
{
public:
	static Particles& Get();
	void SpawnParticles(int num, Waffle::Vec2 pos, Waffle::Color initialColor);
	void Update(float deltaTime);
	void Draw(Waffle::Graphics* graphics);
	void Reset();

private:
	void Init();
	struct Particle
	{
		void Reset();
		bool Active;
		Waffle::Sprite* ParticleSprite;
		float LifeTime;
		float MaxLife;
		Waffle::Vec2 MoveDir;
		float Speed;
		Waffle::Color InitColor;
	};
	const static int k_maxParticles = 1024;
	std::vector<Particle> m_particlesPool;

	Waffle::Image* m_circleImage;
};