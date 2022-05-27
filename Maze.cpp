#include "Game.h"

Maze::Maze()
{

}

Maze::Maze(std::vector<int> rawTiles)
{

	tiles = rawTiles;

}

Maze::~Maze() noexcept
{
	UnloadTexture(tilesAtlas);
}

void Maze::LoadTextureAtlas()
{

	tilesAtlas = LoadTexture("img/pacmiku.png");
	SetTextureFilter(tilesAtlas, TEXTURE_FILTER_POINT);

}

void Maze::Update()
{

}

void Maze::OnDraw()
{

	// Draw Maze Tiles
	for (int y = 0; y < 22; y++)
	{
		for (int x = 0; x < 19; x++)
		{
			int tileID = tiles[19 * y + x];
			DrawTextureRec(tilesAtlas, Rectangle{ (float)tileID * 16, 0, 16, 16 }, Vector2{ (float)x * 16, (float)y * 16 + 48 }, WHITE);
		}
	}

}

void Maze::ClearTile(int _i)
{

	tiles[_i] = 0;

}

int Maze::GetTileID(int i) { return tiles[i]; }