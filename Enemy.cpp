#include "Board.h";

void Enemy::LoadTextureAtlas()
{

	animAtlas = LoadTexture("img/redG.png");
	Entity::LoadTextureAtlas();

}

void Enemy::Update(Board* _board, Maze* _maze, Vector2Int _playerPos)
{

	float deltaTime = GetFrameTime();

	stepTimer += 2.5f * deltaTime;

	if (stepTimer >= 1)
	{

		coords = AddDir(coords, dirIndex);

		ChangeDir(_maze, _playerPos);

		stepTimer--;

	}

	if (hitWall)
	{

		ChangeDir(_maze, _playerPos);

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

void Enemy::ChangeDir(Maze* _maze, Vector2Int _playerPos)
{

	std::vector<float> dirDist(3);

	for (int i = -1; i < 2; i++)
	{

		if (IsValidDir(_maze, coords, TrueMod(dirIndex + i, 4))) { dirDist[i + 1] = AddDir(coords, TrueMod(dirIndex + i, 4)).distanceTo(_playerPos); }

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