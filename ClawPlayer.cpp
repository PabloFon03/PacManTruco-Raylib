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
	targetTiles = board->GetCommonTexture(1);
	noTargets = board->GetTexture(3);

	pos = Vector2{ 0, 20 };

}

void Board::Player::Update()
{

	float deltaTime = board->GetDeltaTime();

	switch (currentState)
	{

	case Playing:

		pos.x += 8 * deltaTime * (IsKeyDown(KEY_LEFT) ? -1 : IsKeyDown(KEY_RIGHT) ? 1 : 0);

		if (abs(pos.x) > 6) { pos.x = 6 * (pos.x > 0 ? 1 : -1); }

		break;

	case Grabbing:

		pos.y -= 12 * deltaTime;

		if (pos.y < -10)
		{

			pos.y = -10;

			board->TargetGrabbed(-1);
			currentState = Retrieving;

		}

		else
		{

			Vector2 checkPos = Vector2{ pos.x, pos.y - 0.5f };

			std::vector<Vector2> targetsPos = board->GetTargetsPos();

			for (int i = 0; grabbedTargetID == -1 && i < targetsPos.size(); i++)
			{

				if (sqrt(pow(targetsPos[i].x - checkPos.x, 2) + pow(targetsPos[i].y - checkPos.y, 2)) < 1.25f)
				{

					grabbedTargetID = board->GetTargetID(i);
					board->TargetGrabbed(i);
					currentState = Retrieving;

				}

			}

		}

		break;

	case Retrieving:

		if (pos.y > 20)
		{

			pos.y = 20;

			board->TargetRetreived();

		}

		else { pos.y += 5 * deltaTime; }

		break;

	}

}

void Board::Player::OnDraw()
{

	Vector2 clawDrawPos = GetDrawPos(pos);

	int ropeLength = ceilf(clawDrawPos.y / 4);

	// Draw Rope
	for (int i = 0; i < ropeLength; i++) { DrawTextureRec(ropeTile, Rectangle{ 0, 0, 4, 4 }, Vector2{ clawDrawPos.x + 22, clawDrawPos.y - 4 * (i + 1) }, WHITE); }

	// Draw Grabbed Target
	if (currentState == Retrieving)
	{

		// No Target
		if (grabbedTargetID == -1) { DrawTextureRec(noTargets, Rectangle{ 0 , 0, NoTargetsTileSize().x, NoTargetsTileSize().y }, Vector2{ clawDrawPos.x - (NoTargetsTileSize().x - TileSize().x) / 2, clawDrawPos.y + (NoTargetsTileSize().y - TileSize().y) / 4 - 8 }, WHITE); }

		// Target Grabbed
		else { DrawTextureRec(targetTiles, Rectangle{ 0, TargetTileSize().y * grabbedTargetID, TargetTileSize().x, TargetTileSize().y }, Vector2{ clawDrawPos.x - (TargetTileSize().x - TileSize().x) / 2, clawDrawPos.y + (TargetTileSize().y - TileSize().y) / 4 + 16 }, WHITE); }

	}

	// Draw Claw
	DrawTextureRec(animAtlas, Rectangle{ currentState == Retrieving ? TileSize().x : 0 , 0, TileSize().x, TileSize().y }, clawDrawPos, WHITE);

}

void Board::Player::Drop()
{

	grabbedTargetID = -1;

	currentState = Grabbing;

}

void Board::Player::Restart() { currentState = Playing; }