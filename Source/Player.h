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
	const std::vector<Spell*>& GetSpells()const;
	void Reset();

private:
	void CastSpell(float size);

	Waffle::Sprite* m_sprite;
	std::vector<Spell*> m_spellPool;

	float m_moveSpeed; // pixels/second

	bool m_holding;
	float m_maxHoldTime;
	float m_holdTimer;

	float m_spellCD;
	float m_spellTimer;
};