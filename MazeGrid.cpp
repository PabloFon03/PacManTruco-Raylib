#include "Maze.h"

using namespace PacMan_Board;

Board::Grid::Grid()
{

}

Board::Grid::Grid(std::vector<int> rawTiles)
{

	tiles = rawTiles;

}

Board::Grid::~Grid() noexcept
{
	UnloadTexture(tilesAtlas);
}

void Board::Grid::LoadTextureAtlas()
{

	tilesAtlas = LoadTexture("img/pacmiku.png");
	SetTextureFilter(tilesAtlas, TEXTURE_FILTER_POINT);

}

void Board::Grid::Update()
{

}

void Board::Grid::OnDraw(int _ColorID)
{

	// Draw Grid Tiles
	for (int y = 0; y < 22; y++)
	{

		for (int x = 0; x < 19; x++)
		{

			int tileID = tiles[19 * y + x];

			DrawTextureRec(tilesAtlas, Rectangle{ (float)tileID * 16, (float)_ColorID * 16, 16, 16 }, Vector2{ (float)x * 16, (float)y * 16 + 48 }, WHITE);

		}

	}

}

int Board::Grid::GetDotCount()
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

void Board::Grid::ClearTile(int _i)
{

	tiles[_i] = 0;

}

int Board::Grid::GetTileID(int i) { return tiles[i]; }