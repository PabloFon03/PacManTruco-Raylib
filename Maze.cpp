#include "Maze.h";

Maze::Maze()
{

}

Maze::Maze(std::vector<int> rawTiles)
{
	tiles = rawTiles;

	tilesAtlas = LoadTexture("img/pacmiku.png");
	SetTextureFilter(tilesAtlas, TEXTURE_FILTER_POINT);

	player = Entity();
	player.x = 9;
	player.y = 16;
	player.dir = 3;
}

Maze::~Maze() noexcept
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
			DrawTextureRec(tilesAtlas, Rectangle{ (float)tileID * 16, 0, 16, 16 }, Vector2{ (float)x * 16, (float)y * 16 }, WHITE);
		}
	}

	// Draw Player
	player.Draw();

}