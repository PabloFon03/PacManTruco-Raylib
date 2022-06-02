#include "Board.h"

Entity::Entity()
{
}

Entity::~Entity()
{
}

Entity::Vector2Int Entity::dir(int i)
{

	switch (i % 4)
	{
	case 0: return Vector2Int{ 0, -1 };
	case 1: return Vector2Int{ 1, 0 };
	case 2: return Vector2Int{ 0, 1 };
	case 3: return Vector2Int{ -1, 0 };
	default: return Vector2Int{ 0, 0 };
	}

}

void Entity::DrawCurrentFrame(Texture2D& _animAtlas)
{

	for (int y = -1; y < 2; y++)
	{

		for (int x = -1; x < 2; x++)
		{

			Vector2 rawCoords = { GetRawCoords().x + x * 19, GetRawCoords().y + y * 22 };

			if (rawCoords.x > -2 && rawCoords.x < 21 && rawCoords.y > -2 && rawCoords.y < 24) { DrawTextureRec(_animAtlas, Rectangle{ (float)animIndex * TileSize().x, (float)dirIndex * TileSize().y, (float)TileSize().x, (float)TileSize().y }, Vector2{ rawCoords.x * 16 - (TileSize().x - 16) / 2.0f, rawCoords.y * 16 + 48 - (TileSize().y - 16) / 2.0f }, WHITE); }

		}

	}	

}

void Entity::ChangeDir(Maze* _maze)
{

	hitWall = !IsValidDir(_maze, coords, dirIndex);

}

Entity::Vector2Int Entity::AddDir(Vector2Int _coords, int _dirIndex)
{

	Vector2Int returnCoords = Vector2Int{ _coords.x, _coords.y };

	Vector2Int dirVec = dir(_dirIndex);

	returnCoords.x = TrueMod(returnCoords.x + dirVec.x, 19);

	returnCoords.y = TrueMod(returnCoords.y + dirVec.y, 22);

	return returnCoords;

}

int Entity::CoordsToIndex(Vector2Int coords) { return coords.y * 19 + coords.x; }

bool Entity::IsValidDir(Maze* _maze, Vector2Int _coords, int _dirIndex)
{

	int nextTileID = _maze->GetTileID(CoordsToIndex(AddDir(_coords, _dirIndex)));

	return nextTileID == 0 || nextTileID > 17;

}

float Entity::Vector2Int::magnitude() { return sqrt(powf(x, 2) + powf(y, 2)); }

float Entity::Vector2Int::distanceTo(Vector2Int _target)
{

	Vector2Int diff = Vector2Int{ _target.x - x, _target.y - y };

	return diff.magnitude();

}