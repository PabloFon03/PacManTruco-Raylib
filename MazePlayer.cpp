#include "Maze.h"

using namespace PacMan_Board;

void Board::Player::Update()
{

	float deltaTime = board->GetDeltaTime();

	if (currentState != None)
	{

		stateTimer -= deltaTime;

		if (stateTimer <= 0) { currentState = None; }

	}

	int itemKeys[3]{ KEY_Z, KEY_X, KEY_C };
	for (int i = 0; currentState == None && i < 3; i++)
	{

		switch (items[i])
		{

		case Sword:

			if (IsKeyPressed(itemKeys[i]))
			{
				currentState = Sword;
				stateTimer = 0.2f;
			}

			break;

		case Dash:

			if (IsKeyPressed(itemKeys[i]))
			{
				currentState = Dash;
				stateTimer = 0.5f;
			}

			break;

		case Magic:

			if (IsKeyPressed(itemKeys[i]))
			{
				currentState = Magic;
				stateTimer = 10;
			}

			break;

		case Projectile:

			break;

		case Trade:

			if (IsKeyPressed(itemKeys[i]))
			{

				float dischargeRate = Items::GetItem((int)Trade).GetDischargeRate();

				while (electricEnergy >= dischargeRate)
				{
					electricEnergy -= dischargeRate;
					energy++;
				}

			}

			break;

		case Freeze:

			if (IsKeyDown(itemKeys[i]) && electricEnergy > 0)
			{
				currentState = Freeze;
				stateTimer = 0;

				electricEnergy -= Items::GetItem((int)Freeze).GetDischargeRate() * deltaTime;
				if (electricEnergy < 0) { electricEnergy = 0; }
			}

			break;

		case ElectricDash:

			if (IsKeyDown(itemKeys[i]) && electricEnergy > 0)
			{
				currentState = ElectricDash;
				stateTimer = 0;

				electricEnergy -= Items::GetItem((int)ElectricDash).GetDischargeRate() * deltaTime;
				if (electricEnergy < 0) { electricEnergy = 0; }
			}

			break;

		}

	}

	stepTimer += (currentState == Dash || currentState == ElectricDash ? 12 : 5) * deltaTime;

	if (stepTimer >= 1)
	{

		if (IsValidDir(coords, dirIndex)) { coords = AddDir(coords, dirIndex); }
		else { hitWall = true; }

		ChangeDir();

		int tileID;
		tileID = grid->GetTileID(CoordsToIndex(coords));

		switch (tileID)
		{

			// Blank Vessel
		case 17:

			grid->ClearTile(CoordsToIndex(coords));

			board->DotCollected();
			board->AddScore(10);

			break;

			// Energy Crystal
		case 18:

			grid->ClearTile(CoordsToIndex(coords));

			energy += 3;
			board->AddScore(50);

			break;

			// Dash Booster
		case 19:
		case 20:
		case 21:
		case 22:

			BoostTileCheck();

			break;

		}

		stepTimer--;

	}

	if (hitWall)
	{

		stepTimer = 0;

		ChangeDir();

		if (currentState == Dash && !IsValidDir(coords, dirIndex))
		{
			currentState = None;
			stateTimer = 0;
		}

	}

	else
	{
		// Drift
		int dirKeys[4]{ KEY_UP, KEY_RIGHT, KEY_DOWN, KEY_LEFT };
		for (int i = 0; i < 2; i++)
		{

			if (IsKeyDown(dirKeys[2 * i + (dirIndex % 2 == 0 ? 1 : 0)]))
			{
				electricEnergy += 0.25f * deltaTime;
				if (electricEnergy > 1) { electricEnergy = 1; }
			}

		}

		// Update Animation
		animDelay += deltaTime;

		if (animDelay >= 0.08f)
		{

			animIndex++;
			animIndex %= 4;

			animDelay -= 0.08f;

		}

	}

}

void Board::Player::BoostTileCheck()
{

	if (dirIndex == grid->GetTileID(CoordsToIndex(coords)) - 19)
	{

		grid->ClearTile(CoordsToIndex(coords));

		currentState = Dash;
		stateTimer = 1.5f;

	}

}

void Board::Player::ChangeDir()
{

	int startDirIndex = dirIndex;
	int dirKeys[4]{ KEY_UP, KEY_RIGHT, KEY_DOWN, KEY_LEFT };

	for (int i = 0; i < 4; i++)
	{

		if ((!(i == startDirIndex)) && IsKeyDown(dirKeys[i]) && (IsValidDir(coords, i)))
		{

			dirIndex = i;

			break;

		}

	}

	Entity::ChangeDir();

}

void Board::Player::OnDraw()
{

	// Draw Sword
	if (currentState == Sword) { DrawCurrentFrame(swordAnimAtlas.atlas, (int)floorf((0.2f - stateTimer) / 0.05f), swordAnimAtlas.tileSize, Vector2{ dir(dirIndex).x * 0.75f, dir(dirIndex).y * 0.75f }); }

	// Draw Player
	AnimAtlas currentAtlas = GetCurrentAnimAtlas();
	DrawCurrentFrame(currentAtlas.atlas, animIndex, currentAtlas.tileSize);

	// DrawText(TextFormat("%i\n%f\n%i\n%f", (int)currentState, stateTimer, energy, electricEnergy), 8, 64, 16, WHITE);

}