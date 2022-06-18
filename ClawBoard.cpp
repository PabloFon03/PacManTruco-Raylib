#include "Claw.h"

using namespace Claw_Board;

Board::Board(Resources* _res, int _difficulty)
{

	resources = _res;

	player = Player(this);

	switch (_difficulty)
	{

		// Easy
	case 0:

		rows = std::vector<Row>{ { 0, 15 }, { 1, 12 }, { 2, 9 }, { 3, 6 }, { 4, 3 } };

		targets = std::vector<Target>
		{

			Target{ this, 0, Vector2{ 0, 15 }, -4 }, Target{ this, 0, Vector2{ 0, 15 }, 4 },

			Target{ this, 1, Vector2{ -2, 12 }, -7 }, Target{ this, 1, Vector2{ 2, 12 }, 7 }, Target{ this, 1, Vector2{ 0, 12 }, -12 }, Target{ this, 1, Vector2{ 0, 12 }, 12 },

			Target{ this, 2, Vector2{ -7, 9 }, 6 }, Target{ this, 2, Vector2{ -5, 9 }, 6 }, Target{ this, 2, Vector2{ 5, 9 }, -6 }, Target{ this, 2, Vector2{ 7, 9 }, -6 },

			Target{ this, 3, Vector2{ -7, 6 }, 8 }, Target{ this, 3, Vector2{ 0, 6 }, -8 }, Target{ this, 3, Vector2{ 0, 6 }, 8 }, Target{ this, 3, Vector2{ 7, 6 }, -8 },

			Target{ this, 4, Vector2{ 0, 3 }, -10 }, Target{ this, 4, Vector2{ 0, 3 }, 10 },

		};

		break;

		// Normal
	case 1:

		rows = std::vector<Row>{ { 0, 15 }, { 1, 12 }, { 2, 9 }, { 3, 6 }, { 4, 3 } };

		targets = std::vector<Target>
		{

			Target{ this, 0, Vector2{ -2, 15 }, 5 }, Target{ this, 0, Vector2{ 0, 15 }, 5 }, Target{ this, 0, Vector2{ 2, 15 }, 5 },

			Target{ this, 1, Vector2{ -2, 12 }, -7 }, Target{ this, 1, Vector2{ 2, 12 }, 7 }, Target{ this, 1, Vector2{ 0, 12 }, -12 }, Target{ this, 1, Vector2{ 0, 12 }, 12 },

			Target{ this, 2, Vector2{ -7, 9 }, 6 }, Target{ this, 2, Vector2{ -5, 9 }, 6 }, Target{ this, 2, Vector2{ 5, 9 }, -6 }, Target{ this, 2, Vector2{ 7, 9 }, -6 },

			Target{ this, 3, Vector2{ -7, 6 }, 8 }, Target{ this, 3, Vector2{ 0, 6 }, -8 }, Target{ this, 3, Vector2{ 0, 6 }, 8 }, Target{ this, 3, Vector2{ 7, 6 }, -8 },

			Target{ this, 4, Vector2{ 0, 3 }, -10 }, Target{ this, 4, Vector2{ 0, 3 }, 10 },

		};

		break;

		// Hard
	case 2:

		rows = std::vector<Row>{ { 0, 15 }, { 1, 12 }, { 2, 9 }, { 3, 6 }, { 4, 1 } };

		targets = std::vector<Target>
		{

			Target{ this, 0, Vector2{ -2, 15 }, 5 }, Target{ this, 0, Vector2{ 0, 15 }, 5 }, Target{ this, 0, Vector2{ 2, 15 }, 5 },

			Target{ this, 1, Vector2{ -2, 12 }, -7 }, Target{ this, 1, Vector2{ 2, 12 }, 7 }, Target{ this, 1, Vector2{ 0, 12 }, -12 }, Target{ this, 1, Vector2{ 0, 12 }, 12 },

			Target{ this, 2, Vector2{ -7, 9 }, 6 }, Target{ this, 2, Vector2{ -5, 9 }, 6 }, Target{ this, 2, Vector2{ 5, 9 }, -6 }, Target{ this, 2, Vector2{ 7, 9 }, -6 },

			Target{ this, 3, Vector2{ -7, 6 }, 8 }, Target{ this, 3, Vector2{ 0, 6 }, -8 }, Target{ this, 3, Vector2{ 0, 6 }, 8 }, Target{ this, 3, Vector2{ 7, 6 }, -8 },

			Target{ this, 4, Vector2{ 0, 1 }, -10 }, Target{ this, 4, Vector2{ 0, 1 }, 10 },

		};

		break;

	}

}

void Board::Start()
{
}

void Board::Update()
{

	for (int i = 0; i < targets.size(); i++) { targets[i].Update(); }

	player.Update();

}

void Board::OnDraw()
{

	// Draw Rows
	for (int i = 0; i < rows.size(); i++) { for (int x = 0; x < 17; x++) { DrawTextureRec(GetTexture(2), Rectangle{ rows[i].ID * 16.0f, (x == 0 || x == 16 ? 0 : 1) * 16.0f, 16, 16 }, Vector2{ 16 * x + 16.0f, 448.0f - 16 * (rows[i].y + 1) }, WHITE); } }

	// Draw Targets
	for (int i = 0; i < targets.size(); i++) { targets[i].OnDraw(); }

	// Draw Player
	player.OnDraw();

	// Drw Timer
	DrawBox(8, Vector2{ 40, 24 }, Vector2{ 152, 32 });
	DrawTextCharAtlas(TextFormat("%i.%i", 9, 9), Vector2{ 152, 28 }, Color{ 255, 224, 64, 255 }, 1);

}