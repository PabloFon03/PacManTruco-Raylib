#include "Maze.h"

using namespace PacMan_Board;

Board::Grid::Grid() { }

Board::Grid::Grid(Board* _board, std::vector<int> rawTiles)
{
	tilesAtlas = _board->GetTexture(4);
	tiles = rawTiles;
}

Board::Grid::~Grid() { }

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
			if (tileID == 17) { n++; }
		}
	}

	return n;
}