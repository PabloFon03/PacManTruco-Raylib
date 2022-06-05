#include "Board.h"

using namespace PacMan_Board;

Board::Maze::Maze()
{

}

Board::Maze::Maze(std::vector<int> rawTiles)
{

	tiles = rawTiles;

}

Board::Maze::~Maze() noexcept
{
	UnloadTexture(tilesAtlas);
}

void Board::Maze::LoadTextureAtlas()
{

	tilesAtlas = LoadTexture("img/pacmiku.png");
	SetTextureFilter(tilesAtlas, TEXTURE_FILTER_POINT);

}

void Board::Maze::Update()
{

}

void Board::Maze::OnDraw(int _ColorID)
{

	// Draw Maze Tiles
	for (int y = 0; y < 22; y++)
	{

		for (int x = 0; x < 19; x++)
		{

			int tileID = tiles[19 * y + x];

			DrawTextureRec(tilesAtlas, Rectangle{ (float)tileID * 16, (float)_ColorID * 16, 16, 16 }, Vector2{ (float)x * 16, (float)y * 16 + 48 }, WHITE);

		}

	}

}

int Board::Maze::GetDotCount()
{

	int n = 0;

	for (int y = 0; y < 22; y++)
	{

		for (int x = 0; x < 19; x++)
		{

			int tileID = tiles[19 * y + x];

			if (tileID == 18) { n++; }

		}

	}

	return n;
}

void Board::Maze::ClearTile(int _i)
{

	tiles[_i] = 0;

}

int Board::Maze::GetTileID(int i) { return tiles[i]; }