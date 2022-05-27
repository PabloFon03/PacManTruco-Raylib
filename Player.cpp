#include "Game.h"

Player::Player(Board* _board, Vector2Int _spawnPos, int _dirIndex)
{

	board = _board;

	coords = _spawnPos;

	dirIndex = _dirIndex;

	mainAnimAtlas = board->GetTexture(0);

}

void Player::Update(Maze* _maze)
{

	float deltaTime = GetFrameTime();

	stepTimer += 3.5f * deltaTime;

	if (stepTimer >= 1)
	{

		coords = AddDir(coords, dirIndex);

		switch (_maze->GetTileID(CoordsToIndex(coords)))
		{

		case 18:

			_maze->ClearTile(CoordsToIndex(coords));

			board->AddScore(10);

			break;

		case 19:

			_maze->ClearTile(CoordsToIndex(coords));

			board->AddScore(50);

			board->OnPowerCollected();

			break;

		}

		ChangeDir(_maze);

		stepTimer--;

	}

	if (hitWall)
	{

		ChangeDir(_maze);

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

void Player::ChangeDir(Maze* _maze)
{

	for (int i = 0; i < 4; i++)
	{

		if (IsKeyDown(std::vector<int> { KEY_UP, KEY_RIGHT, KEY_DOWN, KEY_LEFT }[i]) && IsValidDir(_maze, coords, i))
		{

			dirIndex = i;

			break;

		}

	}

	Entity::ChangeDir(_maze);

}