#include "Maze.h"

using namespace PacMan_Board;

void Board::Enemy::Update()
{

	float deltaTime = board->GetDeltaTime();

	stepTimer += 4 * deltaTime;

	if (stepTimer >= 1)
	{

		coords = AddDir(coords, dirIndex);

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

		if (animDelay >= 0.1f)
		{

			animIndex++;

			animIndex %= 2;

			animDelay -= 0.1f;

		}

	}

	if (rawDistanceTo(board->GetPlayerRawPos()) < 0.25f)
	{

		board->OnPlayerHit();

	}

}

Board::Entity::Vector2Int Board::Enemy::GetTarget()
{

	Vector2Int playerCoords = board->GetPlayerPos();

	switch (ID)
	{

	case 0: return playerCoords;

	case 1: return AddDir(AddDir(playerCoords, board->GetPlayerDirIndex(), false), board->GetPlayerDirIndex(), false);

	case 3: return coords.distanceTo(playerCoords) > 5 ? playerCoords : Vector2Int{ 0, 0 };

	}

	return Vector2Int{ 0, 0 };

}

void Board::Enemy::ChangeDir()
{

	Vector2Int targetPos = GetTarget();

	std::vector<float> dirDist(4);

	for (int i = -1; i < 2; i++)
	{

		if (IsValidDir(coords, TrueMod(dirIndex + i, 4))) { dirDist[i + 1] = AddDir(coords, TrueMod(dirIndex + i, 4)).distanceTo(targetPos); }

		else { dirDist[i + 1] = -1; }

	}

	// Allow U-Turns
	if (true) { dirDist[3] = -1; }

	int lowestDistIndex = 0;

	// Look For First Possible Direction
	while (lowestDistIndex < 4 && dirDist[lowestDistIndex] < 0) { lowestDistIndex++; }

	// Look For Direction With Lowest Distance
	for (int i = lowestDistIndex + 1; i < 4; i++) { if (dirDist[i] > -1 && dirDist[i] < dirDist[lowestDistIndex]) { lowestDistIndex = i; } }

	// Apply Lowest Distance Direction Offset (U-Turn If None)
	dirIndex = TrueMod(dirIndex + lowestDistIndex - 1, 4);

	Entity::ChangeDir();

}