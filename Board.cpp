#include "Game.h"

Board::Board()
{
}

Board::Board(std::vector<Texture2D>& _textures)
{

	textures = _textures;

}

void Board::Start()
{

	// 19x22
	std::vector<int> mazeTiles
	{
		7, 11, 11, 11, 11, 11, 11, 11, 11, 15, 11, 11, 11, 11, 11, 11, 11, 11, 13,
		6, 18, 18, 18, 18, 18, 18, 18, 18, 6, 18, 18, 18, 18, 18, 18, 18, 18, 6,
		6, 19, 7, 13, 18, 7, 11, 13, 18, 6, 18, 7, 11, 13, 18, 7, 13, 19, 6,
		6, 18, 4, 10, 18, 4, 11, 10, 18, 5, 18, 4, 11, 10, 18, 4, 10, 18, 6,
		6, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 6,
		6, 18, 9, 3, 18, 2, 18, 9, 11, 15, 11, 3, 18, 2, 18, 9, 3, 18, 6,
		6, 18, 18, 18, 18, 6, 18, 18, 18, 6, 18, 18, 18, 6, 18, 18, 18, 18, 6,
		4, 11, 11, 13, 18, 8, 11, 3, 0, 5, 0, 9, 11, 14, 18, 7, 11, 11, 10,
		0, 0, 0, 6, 18, 6, 0, 0, 0, 0, 0, 0, 0, 6, 18, 6, 0, 0, 0,
		11, 11, 11, 10, 18, 5, 0, 7, 3, 17, 9, 13, 0, 5, 18, 4, 11, 11, 11,
		0, 0, 0, 0, 18, 0, 0, 6, 0, 0, 0, 6, 0, 0, 18, 0, 0, 0, 0,
		11, 11, 11, 13, 18, 2, 0, 4, 11, 11, 11, 10, 0, 2, 18, 7, 11, 11, 11,
		0, 0, 0, 6, 18, 6, 0, 0, 0, 0, 0, 0, 0, 6, 18, 6, 0, 0, 0,
		7, 11, 11, 10, 18, 5, 0, 9, 11, 15, 11, 3, 0, 5, 18, 4, 11, 11, 13,
		6, 18, 18, 18, 18, 18, 18, 18, 18, 6, 18, 18, 18, 18, 18, 18, 18, 18, 6,
		6, 18, 9, 13, 18, 9, 11, 3, 18, 5, 18, 9, 11, 3, 18, 7, 3, 18, 6,
		6, 19, 18, 6, 18, 18, 18, 18, 18, 0, 18, 18, 18, 18, 18, 6, 18, 19, 6,
		8, 3, 18, 5, 18, 2, 18, 9, 11, 15, 11, 3, 18, 2, 18, 5, 18, 9, 14,
		6, 18, 18, 18, 18, 6, 18, 18, 18, 6, 18, 18, 18, 6, 18, 18, 18, 18, 6,
		6, 18, 9, 11, 11, 12, 11, 3, 18, 5, 18, 9, 11, 12, 11, 11, 3, 18, 6,
		6, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 6,
		4, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 10
	};
	maze = Maze(mazeTiles);
	maze.LoadTextureAtlas();

	player = Player{ this, Entity::Vector2Int{ 9, 16 }, 3 };

	enemies.clear();
	enemies.push_back(Enemy{ this, 0, Entity::Vector2Int{ 9, 8 }, 3 });
	enemies.push_back(Enemy{ this, 1, Entity::Vector2Int{ 8, 10 }, 0 });
	enemies.push_back(Enemy{ this, 2, Entity::Vector2Int{ 9, 10 }, 0 });
	enemies.push_back(Enemy{ this, 3, Entity::Vector2Int{ 10, 10 }, 0 });

}

void Board::Update()
{

	maze.Update();

	player.Update(&maze);

	for (int i = 0; i < enemies.size(); i++) { enemies[i].Update(&maze, player.coords); }

}

void Board::OnDraw()
{

	maze.OnDraw();

	// Draw Entities From Top To Bottom
	std::vector<Entity*> entities = std::vector<Entity*>();

	entities.push_back(&player);
	for (int i = 0; i < enemies.size(); i++) { entities.push_back(&enemies[i]); }

	// Selection Sort
	for (int i = 0; i < entities.size() - 1; i++)
	{

		int targetIndex = i;

		for (int i = targetIndex + 1; i < entities.size(); i++) { if ((*entities[i]).GetRawCoords().y < (*entities[targetIndex]).GetRawCoords().y) { targetIndex = i; } }

		if (targetIndex != i) { std::swap(entities[i], entities[targetIndex]); }

	}

	for (int i = 0; i < entities.size(); i++) { (*entities[i]).OnDraw(); }

	DrawText(("Score: " + std::to_string(score)).c_str(), 8, 8, 16, WHITE);
	DrawText(("Speed: " + std::to_string(speedMod + 1)).c_str(), 128, 8, 16, WHITE);

}

void Board::ClearTile(int _i)
{

	maze.ClearTile(_i);

}

void Board::AddScore(int _s)
{

	score += _s;

	speedScore += _s;

	while (speedScore >= 500)
	{

		speedScore -= 500;

		if (speedMod < 50) { speedMod++; }

		else { score += 2500; }

	}

}

void Board::OnPowerCollected()
{
}