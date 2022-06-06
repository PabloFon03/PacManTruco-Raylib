#include "Maze.h"

using namespace PacMan_Board;

void Board::Player::Update()
{

	float deltaTime = board->GetDeltaTime();

	stepTimer += 4 * deltaTime;

	if (stepTimer >= 1)
	{

		coords = AddDir(coords, dirIndex);

		switch (grid->GetTileID(CoordsToIndex(coords)))
		{

		case 18:

			grid->ClearTile(CoordsToIndex(coords));

			board->DotCollected();

			board->AddScore(10);

			break;

		case 19:

			grid->ClearTile(CoordsToIndex(coords));

			board->AddScore(50);

			board->OnPowerCollected();

			break;

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

	for (int i = 0; i < 4; i++)
	{

		if (IsKeyDown(std::vector<int> { KEY_UP, KEY_RIGHT, KEY_DOWN, KEY_LEFT }[i]) && IsValidDir(coords, i))
		{

			dirIndex = i;

			break;

		}

	}

	Entity::ChangeDir();

}