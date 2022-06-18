#include "Maze.h"

using namespace PacMan_Board;

Board::Entity::Entity()
{
}

Board::Entity::~Entity()
{
}

Board::Entity::Vector2Int Board::Entity::dir(int i)
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

void Board::Entity::DrawCurrentFrame(Texture2D& _animAtlas)
{

	for (int y = -1; y < 2; y++)
	{

		for (int x = -1; x < 2; x++)
		{

			Vector2 rawCoords = { GetRawCoords().x + x * 19, GetRawCoords().y + y * 22 };

			if (rawCoords.x > -1 && rawCoords.x < 20 && rawCoords.y > -1 && rawCoords.y < 23) { DrawTextureRec(_animAtlas, Rectangle{ (float)animIndex * TileSize().x, (float)dirIndex * TileSize().y, (float)TileSize().x, (float)TileSize().y }, Vector2{ rawCoords.x * 16 - (TileSize().x - 16) / 2.0f, rawCoords.y * 16 + 48 - (TileSize().y - 16) / 2.0f }, WHITE); }

		}

	}	

}

void Board::Entity::ChangeDir()
{

	hitWall = !IsValidDir(coords, dirIndex);

}

Board::Entity::Vector2Int Board::Entity::AddDir(Vector2Int _coords, int _dirIndex, bool _modWrap)
{

	Vector2Int returnCoords = Vector2Int{ _coords.x, _coords.y };

	Vector2Int dirVec = dir(_dirIndex);

	if (_modWrap)
	{

		returnCoords.x = TrueMod(returnCoords.x + dirVec.x, 19);

		returnCoords.y = TrueMod(returnCoords.y + dirVec.y, 22);

	}

	return returnCoords;

}

int Board::Entity::CoordsToIndex(Vector2Int coords) { return coords.y * 19 + coords.x; }

bool Board::Entity::IsValidDir(Vector2Int _coords, int _dirIndex)
{

	int nextTileID = grid->GetTileID(CoordsToIndex(AddDir(_coords, _dirIndex)));

	return nextTileID == 0 || nextTileID > 17;

}