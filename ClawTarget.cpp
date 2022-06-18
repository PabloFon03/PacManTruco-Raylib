#include "Claw.h"

using namespace Claw_Board;

Board::Target::Target()
{
}

Board::Target::Target(Board* _board, int _ID, Vector2 _startPos, float _speed)
{

	board = _board;
	animAtlas = board->GetCommonTexture(1);

	ID = _ID;

	pos = _startPos;
	speed = _speed;

}

void Board::Target::Update()
{

	float deltaTime = board->GetDeltaTime();

	pos.x += speed * deltaTime;

	if (abs(pos.x) > 7)
	{

		speed *= -1;

		pos.x = 7 * (pos.x > 0 ? 1 : -1);

	}

	animTimer += deltaTime;

	if (animTimer >= 0.1f)
	{

		animIndex++;

		animIndex %= 4;

		animTimer -= animIndex == 0 ? 0.75f : 0.1f;

	}

}

void Board::Target::OnDraw()
{

	// Draw Target
	DrawTextureRec(animAtlas, Rectangle{ animIndex * TileSize().x, ID * TileSize().y, TileSize().x, TileSize().y }, GetDrawPos(pos), WHITE);

}