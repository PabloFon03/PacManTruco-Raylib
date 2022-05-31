#include "Game.h"

Enemy::Enemy()
{
}

Enemy::Enemy(Board* _board, int _ID, Vector2Int _spawnPos, int _dirIndex)
{

	board = _board;

	ID = _ID;

	coords = _spawnPos;

	dirIndex = _dirIndex;

	mainAnimAtlas = board->GetTexture(ID + 1);

}

void Enemy::Update(Maze* _maze, Vector2Int _targetPos)
{

	float deltaTime = GetFrameTime();

	stepTimer += 3.5f * deltaTime;

	if (stepTimer >= 1)
	{

		coords = AddDir(coords, dirIndex);

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

		if (animDelay >= 0.1f)
		{

			animIndex++;

			animIndex %= 2;

			animDelay -= 0.1f;

		}

	}

}

Entity::Vector2Int Enemy::GetTarget()
{

	Vector2Int playerCoords = board->GetPlayerPos();

	switch (ID)
	{

	case 0: return playerCoords;

	case 1: return AddDir(playerCoords, board->GetPlayerDirIndex());

	case 3: return coords.distanceTo(playerCoords) > 5 ? playerCoords : Vector2Int{ 0, 0 };

	}

	return Vector2Int{ 0, 0 };

}

void Enemy::ChangeDir(Maze* _maze)
{

	Vector2Int targetPos = GetTarget();

	std::vector<float> dirDist(3);

	for (int i = -1; i < 2; i++)
	{

		if (IsValidDir(_maze, coords, TrueMod(dirIndex + i, 4))) { dirDist[i + 1] = AddDir(coords, TrueMod(dirIndex + i, 4)).distanceTo(targetPos); }

		else { dirDist[i + 1] = -1; }

	}

	int lowestDistIndex = 0;

	// Look For First Possible Direction
	while (lowestDistIndex < 3 && dirDist[lowestDistIndex] < 0) { lowestDistIndex++; }

	// Look For Direction With Lowest Distance
	for (int i = lowestDistIndex + 1; i < 3; i++) { if (dirDist[i] > -1 && dirDist[i] < dirDist[lowestDistIndex]) { lowestDistIndex = i; } }

	// Apply Lowest Distance Direction Offset (U-Turn If None)
	dirIndex = TrueMod(dirIndex + lowestDistIndex - 1, 4);

	Entity::ChangeDir(_maze);

}