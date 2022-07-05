#include "Maze.h"

using namespace PacMan_Board;

void Board::Player::Update()
{

	float deltaTime = board->GetDeltaTime();

	stepTimer += 4 * deltaTime;

	if (stepTimer >= 1)
	{

		if (IsValidDir(coords, dirIndex)) { coords = AddDir(coords, dirIndex); }
		else { hitWall = true; }

		int tileID;
		tileID = grid->GetTileID(CoordsToIndex(coords));

		switch (tileID)
		{

			// Blank Vessel
		case 17:

			grid->ClearTile(CoordsToIndex(coords));

			board->DotCollected();
			board->AddScore(10);

			break;

		case 18:

			grid->ClearTile(CoordsToIndex(coords));

			board->AddScore(50);
			board->OnPowerCollected();

			break;

			// Dash Booster
		case 19:
		case 20:
		case 21:
		case 22:

			if (dirIndex == tileID - 19)
			{



			}

		}

		ChangeDir();

		stepTimer--;

	}

	if (hitWall)
	{

		ChangeDir();

		stepTimer = 0;

	}

	else
	{

		animDelay += deltaTime;

		if (animDelay >= 0.08f)
		{

			animIndex++;

			animIndex %= 4;

			animDelay -= 0.08f;

		}

	}

}

void Board::Player::ChangeDir()
{

	int startDirIndex = dirIndex;

	for (int i = 0; i < 4; i++)
	{

		if ((!(i == startDirIndex)) && IsKeyDown(std::vector<int> { KEY_UP, KEY_RIGHT, KEY_DOWN, KEY_LEFT }[i]) && IsValidDir(coords, i))
		{

			dirIndex = i;

			break;

		}

	}

	Entity::ChangeDir();

}