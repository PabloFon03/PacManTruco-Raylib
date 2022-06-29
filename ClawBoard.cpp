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

		rows = std::vector<Row>{ { 0, 15 }, { 0, 14 }, { 1, 12 }, { 1, 11 }, { 2, 9 }, { 2, 8 }, { 3, 6 }, { 3, 5 }, { 4, 3 }, { 4, 2 } };

		targets = std::vector<Target>
		{

			Target{ this, 0, Vector2{ 0, 15 }, 5 },

			Target{ this, 0, Vector2{ 0, 14 }, 5 },

			Target{ this, 1, Vector2{ 0, 12 }, -5 },
			
			Target{ this, 1, Vector2{ 0, 11 }, -5 },

			Target{ this, 2, Vector2{ -7, 9 }, 4 },
			
			Target{ this, 2, Vector2{ 7, 8 }, -4 },

			Target{ this, 3, Vector2{ -3, 6 }, 3 }, Target{ this, 3, Vector2{ 3, 6 }, -3 },

			Target{ this, 3, Vector2{ -7, 5 }, 3 }, Target{ this, 3, Vector2{ 7, 5 }, -3 },

			Target{ this, 4, Vector2{ -2, 3 }, -7 }, Target{ this, 4, Vector2{ 2, 3 }, 7 },

			Target{ this, 4, Vector2{ -7, 2 }, 7 }, Target{ this, 4, Vector2{ 7, 2 }, -7 },

		};

		break;

		// Normal
	case 1:

		rows = std::vector<Row>{ { 0, 15 }, { 1, 12 }, { 2, 9 }, { 3, 6 }, { 4, 3 } };

		targets = std::vector<Target>
		{

			Target{ this, 0, Vector2{ 0, 15 }, -4 }, Target{ this, 0, Vector2{ 0, 15 }, 4 },

			Target{ this, 1, Vector2{ -2, 12 }, -7 }, Target{ this, 1, Vector2{ 2, 12 }, 7 }, Target{ this, 1, Vector2{ 0, 12 }, -12 }, Target{ this, 1, Vector2{ 0, 12 }, 12 },

			Target{ this, 2, Vector2{ -5, 9 }, 6 }, Target{ this, 2, Vector2{ -3, 9 }, 6 }, Target{ this, 2, Vector2{ 3, 9 }, -6 }, Target{ this, 2, Vector2{ 5, 9 }, -6 },

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

			Target{ this, 4, Vector2{ 0, 1 }, -12 }, Target{ this, 4, Vector2{ 0, 1 }, 12 },

		};

		break;

	}

}

void Board::Update()
{

	switch (currentState)
	{

	case Starting:

		stepTimer += GetDeltaTime();

		float delay;
		delay = stepCounter == 2 ? 1 : 1.5f;

		if (stepTimer >= delay)
		{

			// Start Play Mode
			if (stepCounter == 2)
			{

				currentState = Playing;
				stepTimer = 10;

			}

			// Jump To Next Delay
			else
			{

				stepTimer -= delay;
				stepCounter++;

			}

		}

		break;

	case Playing:

		stepTimer -= GetDeltaTime();

		if (IsKeyPressed(KEY_DOWN)) { stepTimer = 0; }

		if (stepTimer > 0)
		{

			player.Update();

			for (int i = 0; i < targets.size(); i++) { targets[i].Update(); }

		}

		else
		{

			// Drop Claw
			player.Drop();

			// Switch To Next State
			currentState = Grabbing;
			stepTimer = 0;

		}

		break;

	case Grabbing:

		player.Update();

		for (int i = 0; i < targets.size(); i++) { targets[i].Update(); }

		break;

	case Retrieving:

		// Delay Before Pulling Claw
		if (stepTimer > 0) { stepTimer -= GetDeltaTime(); }

		// Pull Up Claw
		else { player.Update(); }

		break;

	case Results:

		// Start Delay
		if (stepCounter == 0)
		{

			// Delay
			if (stepTimer > 0) { stepTimer -= GetDeltaTime(); }

			// Go To Next Round
			else if (roundCounter < 2)
			{

				// Update Round Counter
				roundCounter++;

				// Reset
				stepCounter = 0;
				stepTimer = 0;
				currentState = Starting;

				player.Restart();

			}

			// Last Round Cleared
			else
			{

				// Jump To Final Results Display
				stepCounter = 1;
				stepTimer = 2;

			}

		}

		// Displaying Final Results
		else
		{

			// Delay
			if (stepTimer > 0) { stepTimer -= GetDeltaTime(); }

			// Next Step
			else
			{
				stepTimer++;
				stepCounter++;
			}

		}

		break;

	}

}

void Board::OnDraw()
{

	// Draw Rows
	for (int i = 0; i < rows.size(); i++) { for (int x = 0; x < 17; x++) { DrawTextureRec(GetTexture(2), Rectangle{ rows[i].ID * 16.0f, (x == 0 || x == 16 ? 0 : 1) * 16.0f, 16, 16 }, Vector2{ 16 * x + 16.0f, 448.0f - 16 * (rows[i].y + 1) }, Color{ 225, 225, 225, 255 }); } }

	// Draw Targets
	for (int i = 0; i < targets.size(); i++) { targets[i].OnDraw(); }

	// Draw Player
	player.OnDraw();

	// Draw Round Counter
	DrawBox(8, Vector2{ 80, 24 }, Vector2{ 64, 32 }, Color{ 102, 216, 255, 255 });
	DrawTextCharAtlas(TextFormat("Round: %i", roundCounter + 1), Vector2{ 64, 28 }, Color{ 102, 216, 255, 255 }, 1);

	// Draw Timer
	DrawBox(8, Vector2{ 40, 24 }, Vector2{ 152, 32 }, Color{ 255, 228, 102, 255 });
	DrawTextCharAtlas(currentState != Playing ? "-.-" : stepTimer > 9.8f ? "9.9" : TextFormat("%i.%i", (int)floorf(stepTimer) % 10, (int)floorf(stepTimer * 10) % 10), Vector2{ 152, 28 }, Color{ 255, 228, 102, 255 }, 1);

	// Draw Score
	DrawBox(8, Vector2{ 80, 24 }, Vector2{ 240, 32 }, Color{ 102, 255, 140, 255 });
	DrawTextCharAtlas(TextFormat("%i", score), Vector2{ 240, 28 }, Color{ 102, 255, 140, 255 }, 1);

	if (currentState == Starting && stepCounter == 1)
	{

		// Draw Center Round Counter
		DrawBox(8, Vector2{ 80, 24 }, Vector2{ 152, 224 }, Color{ 200, 200, 200, 255 });
		DrawTextCharAtlas(TextFormat("Round %i", roundCounter + 1), Vector2{ 152, 220 }, Color{ 225, 225, 225, 255 }, 1);

	}

	else if (currentState == Results && stepCounter > 0)
	{

		// Draw Box
		DrawBox(8, Vector2{ 96, 128 }, Vector2{ 152, 224 }, Color{ 200, 200, 200, 255 });

		for (int i = 0; i < stepCounter - 2; i++)
		{

			switch (i)
			{

			case 0: DrawTextCharAtlas("Score", Vector2{ 152, 180 }, Color{ 225, 225, 225, 255 }, 1); break;

			case 1: DrawTextCharAtlas(TextFormat("%i", score), Vector2{ 152, 192 }, Color{ 225, 225, 225, 255 }, 1); break;

			case 2: DrawTextureRec(GetCommonTexture(1), Rectangle{ 0, 0, 16, 16 }, Vector2{ 144, 240 }, Color{ 225, 225, 225, 255 }); break;

			case 3:

				DrawTextCharAtlas(TextFormat("%i", GetTokens()), Vector2{ 152, 260 }, Color{ 225, 225, 225, 255 }, 1);

				if (score >= bonusThreshold)
				{
					DrawTextCharAtlas("[BONUS]", Vector2{ 152, 212 }, Color{ 225, 225, 225, 255 }, 1);
					DrawTextCharAtlas(TextFormat("%i", bonusTokens), Vector2{ 152, 224 }, Color{ 225, 225, 225, 255 }, 1);
				}

				break;

			}

		}

	}

}

void Claw_Board::Board::TargetGrabbed(int _i)
{

	if (_i != -1)
	{

		// Add Points To Score
		int points[5]{ 10, 25, 50, 100, 250 };
		score += points[targets[_i].GetID()];

		// Remove Target From Board
		targets.erase(std::next(targets.begin(), _i));

	}

	currentState = Retrieving;
	stepTimer = 1;

}

void Claw_Board::Board::TargetRetreived()
{

	stepCounter = 0;
	stepTimer = 1;
	currentState = Results;

}