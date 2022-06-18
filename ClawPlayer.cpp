#include "Claw.h"

using namespace Claw_Board;

Board::Player::Player()
{
}

Board::Player::Player(Board* _board)
{

	board = _board;

	animAtlas = board->GetTexture(0);
	ropeTile = board->GetTexture(1);

	pos = Vector2{ 0, 20 };

}

void Board::Player::Update()
{

	float deltaTime = board->GetDeltaTime();

	switch (0)
	{
	default:

		pos.x += 8 * deltaTime * (IsKeyDown(KEY_LEFT) ? -1 : IsKeyDown(KEY_RIGHT) ? 1 : 0);

		if (abs(pos.x) > 6) { pos.x = 6 * (pos.x > 0 ? 1 : -1); }

		break;
	}

}

void Board::Player::OnDraw()
{

	Vector2 clawDrawPos = GetDrawPos(pos);

	int ropeLength = ceilf(clawDrawPos.y / 4);

	// Draw Rope
	for (int i = 0; i < ropeLength; i++) { DrawTextureRec(ropeTile, Rectangle{ 0, 0, 4, 4 }, Vector2{ clawDrawPos.x + 22, clawDrawPos.y - 4 * (i + 1) }, WHITE); }

	// Draw Claw
	DrawTextureRec(animAtlas, Rectangle{ 0, 0, TileSize().x, TileSize().y }, clawDrawPos, WHITE);

}