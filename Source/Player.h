#pragma once

#include <vector>

namespace Waffle
{
	class Graphics;
	class Sprite;
	class Image;
}

class Spell;
class Player
{
public:
	Player();
	~Player();
	bool Init();
	void Update(float deltaTime);
	void Draw(Waffle::Graphics* graphics);

private:
	void CastSpell(float size);

	Waffle::Sprite* m_sprite;
	std::vector<Spell*> m_spellPool;

	float m_moveSpeed; // pixels/second

	float m_spellCD;
	float m_spellTimer;
};