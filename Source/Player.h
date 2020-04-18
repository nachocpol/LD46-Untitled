#pragma once

namespace Waffle
{
	class Graphics;
	class Sprite;
	class Image;
}

class Player
{
public:
	Player();
	~Player();

	bool Init();
	void Update(float deltaTime);
	void Draw(Waffle::Graphics* graphics);

private:
	Waffle::Sprite* m_sprite;
};