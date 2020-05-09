#include "Enemy.h"
#include "Spell.h"
#include "Bonfire.h"
#include "Game.h"

#include "Graphics/Graphics.h"
#include "Graphics/Sprite.h"
#include "Graphics/Image.h"
#include "Graphics/MathUtils.h"
#include "Logging.h"

#include <stdlib.h>

using namespace Waffle;

Image* Enemy::m_image = nullptr;

Enemy::Enemy()
	: m_sprite(nullptr)
{
	Reset();
}

Enemy::~Enemy()
{

}

bool Enemy::Init()
{
	if (!m_image)
	{
		m_image = Image::CreateFromFile("data:IceShardMonster.png");
	}
	m_sprite = new Sprite((float)m_image->GetWidth(), (float)m_image->GetHeight(),m_image);
	float rndScale = (((float)rand() / (float)RAND_MAX) * 2.0f) + 1.5f;
	m_sprite->SetScale(rndScale, rndScale);

	Image* shadowImg = Game::GetDropShadow();
	m_spriteDropShadow = new Sprite((float)shadowImg->GetWidth(), (float)shadowImg->GetHeight(), shadowImg);
	m_spriteDropShadow->SetScale(rndScale * 1.6f, rndScale * 1.6f);

	return true;
}

void Enemy::Update(float deltaTime, std::vector<Spell*> spells)
{
	if (!m_active)
	{
		return;
	}

	// First check hits with spells:
	bool dead = m_hitPoints == 0;
	for (const auto spell : spells)
	{
		if (spell->IsActive())
		{
			Vec2 spellPos = spell->GetTransform().Position;
			float dist = Length(spellPos - m_sprite->GetTransform().Position);
			dist = dist - (spell->GetRadius() + GetRadius());
			if (dist <= 1.0f) // Add some fudge to make it easier.
			{
				TakeHit(false);
			}
			dead = m_hitPoints == 0;
			if (dead)
			{
				break;
			}
		}
	}

	if (dead)
	{
		Reset();
	}
	else
	{
		// Find a new target:
		if (!m_target->IsActive())
		{
			FindTarget();
		}

		// Move towards it:
		if (m_target)
		{
			Vec2 toTarget = m_target->GetTransform().Position - m_sprite->GetTransform().Position;
			float dist = Length(toTarget);
			toTarget = Normalize(toTarget);

			float speed = 650.0f;
			if (dist < 1700.0f)
			{
				speed *= 0.25f;
			}
			m_sprite->Move(toTarget.X * deltaTime * speed, toTarget.Y * deltaTime * speed);
			m_spriteDropShadow->Move(toTarget.X * deltaTime * speed, toTarget.Y * deltaTime * speed);

			// Juicy rotation:
			extern float g_totalTime;
			float rot = sin((g_totalTime + m_seed) * 15.0f);
			float neg = false;
			if (rot < 0.0f)
			{
				neg = true;
			}
			rot = pow(abs(rot), 0.55f);
			if (neg)
			{
				rot *= -1.0f;
			}
			
			// Align taking into account the direction:
			float alignRot = atan2(toTarget.Y, toTarget.X) * RAD_TO_DEG;
			m_sprite->SetRotation(alignRot + (rot * 10.0f));
		}
	}
}

void Enemy::Draw(Graphics* graphics)
{
	if (!m_active)
	{
		return;
	}
	graphics->DrawSprite(m_spriteDropShadow);
	graphics->DrawSprite(m_sprite);
}

float Enemy::GetRadius()
{
	Vec2 spriteScale = m_sprite->GetTransform().Scale;
	Vec2 spriteSize = m_sprite->GetSize();
	spriteSize.X *= spriteScale.X * 0.5f;
	spriteSize.Y *= spriteScale.Y * 0.5f;
	float radius = sqrt(spriteSize.X * spriteSize.X + spriteSize.Y * spriteSize.Y);
	return radius;
}

void Enemy::Reset()
{
	m_active = false;
	m_target = nullptr;
	m_hitPoints = 1;
	m_bonfires.clear();
	m_seed = rand();
}

bool Enemy::IsActive() const
{
	return m_active;
}

void Enemy::Spawn(Waffle::Vec2 position, const std::vector<Bonfire*>& bonfires)
{
	if (m_active)
	{
		ERR("Trying to spawn while still active. \n");
		return;
	}
	m_bonfires = bonfires; // we keep the vector in case one is ded.
	FindTarget();
	m_active = true;
	m_sprite->SetPosition(position.X, position.Y);
	m_spriteDropShadow->SetPosition(position.X, position.Y);
}

const Bonfire* Enemy::GetTarget() const
{
	return m_target;
}

Transform Enemy::GetTransform()
{
	return m_sprite->GetTransform();
}

void Enemy::TakeHit(bool bonfire)
{
	Color col = bonfire ? Color(0.85f, 0.6f, 0.0f, 1.0f) : Color(0.75f, 0.2f, 0.9f, 1.0f);
	Particles::Get().SpawnParticles(70, m_sprite->GetTransform().Position, col);
	--m_hitPoints;
}

void Enemy::FindTarget()
{
	std::vector<Bonfire*> aliveBonfires;
	for (const auto bf : m_bonfires)
	{
		if (bf->IsActive())
		{
			aliveBonfires.push_back(bf);
		}
	}
	if (!aliveBonfires.empty())
	{
		int curBf = rand() % aliveBonfires.size();
		m_target = aliveBonfires[curBf];
	}
	else
	{
		m_target = nullptr;
	}
}

Particles& Particles::Get()
{
	static Particles* kInstance = nullptr;
	if (!kInstance)
	{
		kInstance = new Particles;
		kInstance->Init();
	}
	return *kInstance;
}

void Particles::SpawnParticles(int num, Vec2 pos, Color initialColor)
{
	for (auto& particle : m_particlesPool)
	{
		if (!particle.Active)
		{
			--num;

			particle.Active = true;
			particle.ParticleSprite->SetPosition(pos.X, pos.Y);

			// HUHUH
			particle.InitColor.R = saturate(initialColor.R + (((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f) * 0.15f);
			particle.InitColor.G = saturate(initialColor.G + (((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f) * 0.15f);
			particle.InitColor.B = saturate(initialColor.B + (((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f) * 0.15f);
			particle.ParticleSprite->SetTint(particle.InitColor.R, particle.InitColor.G, particle.InitColor.B);
			
			float ml= ((float)rand() / (float)RAND_MAX) * 2.0f + 0.2f;
			particle.MaxLife = 1.0f;
			
			float sp = ((float)rand() / (float)RAND_MAX) * 300.0f + 250.0f;
			particle.Speed = sp;

			float rot = ((float)rand() / (float)RAND_MAX) * 2.0f * PI;
			particle.MoveDir.X = cos(rot);
			particle.MoveDir.Y = sin(rot);

			if (num == 0)
			{
				break;
			}
		}
	}
}

void Particles::Update(float deltaTime)
{
	for (auto& particle : m_particlesPool)
	{
		if (particle.Active)
		{
			particle.LifeTime += deltaTime;
			if (particle.LifeTime > particle.MaxLife)
			{
				particle.Reset();
			}
			else
			{
				float lifeDelta = 1.0f - saturate(particle.LifeTime / particle.MaxLife);
				particle.ParticleSprite->SetTint(particle.InitColor.R, particle.InitColor.G, particle.InitColor.B, lifeDelta * lifeDelta);
				particle.ParticleSprite->Move(
					particle.MoveDir.X * particle.Speed * deltaTime,
					particle.MoveDir.Y * particle.Speed * deltaTime
				);
			}
		}
	}
}

void Particles::Draw(Graphics* graphics)
{
	for (auto& particle : m_particlesPool)
	{
		if (particle.Active)
		{
			graphics->DrawSprite(particle.ParticleSprite);
		}
	}
}

void Particles::Reset()
{
	for (auto& particle : m_particlesPool)
	{
		particle.Reset();
	}
}

void Particles::Init()
{
	m_circleImage = Image::CreateFromFile("data:ParticleCircle.png");

	m_particlesPool.resize(k_maxParticles);
	for (int p = 0; p < k_maxParticles; ++p)
	{
		Particle& particle = m_particlesPool[p];
		particle.ParticleSprite = new Sprite((float)m_circleImage->GetWidth(), (float)m_circleImage->GetHeight(), m_circleImage);
		particle.Reset();
	}
}

void Particles::Particle::Reset()
{
	Active = false;
	LifeTime = 0.0f;
	MaxLife = 0.0f;
	Speed = 0.0f;
	MoveDir = Vec2();
	InitColor = Color();
}
