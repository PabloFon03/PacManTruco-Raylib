#include "Baseball.h"
#include "Random.h"

using namespace Baseball_Arena;

Arena::Arena(Resources* _res, int _difficulty)
{

	resources = _res;

	difficulty = _difficulty;

	player = Player(this);

	balls.clear();

	roundSpawns.clear();

	Random rng = Random();

	for (int r = 0; r < (difficulty == 0 ? 3 : (difficulty == 1 ? 5 : 10)); r++)
	{

		std::vector<SpawnData> currentRoundSpawns = std::vector<SpawnData>();

		bool specialDelay = false;

		for (int i = 0; i < (r < (difficulty == 0 ? 2 : (difficulty == 1 ? 7 : 5)) ? 5 : 10); i++)
		{

			bool tutorialRound = r == 0 && difficulty == 0;

			int ballType;

			if (tutorialRound) { ballType = i; }

			else
			{

				do { ballType = rng.RandomInt(6); } while (specialDelay && (ballType == 1 || ballType == 4));

				specialDelay = false;

			}


			switch (ballType)
			{

				// RNG Delay Variation (50% Chance)
			case 1:

				specialDelay = !tutorialRound && rng.RandomInt(2) == 0;

				currentRoundSpawns.push_back(SpawnData{ 1, specialDelay ? 0.75f : 4 });

				break;

				// RNG Delay Variation (40% Chance)
			case 4:

				specialDelay = !tutorialRound && rng.RandomInt(5) < 2;

				currentRoundSpawns.push_back(SpawnData{ 4, specialDelay ? 0.25f : 2.5f });

				break;

			default:

				currentRoundSpawns.push_back(SpawnData{ ballType, 2 });

				break;

			}

		}

		roundSpawns.push_back(currentRoundSpawns);

	}

}

void Arena::Update()
{

	float deltaTime = GetDeltaTime();

	switch (currentState)
	{

	case Playing:

		if (roundSpawns[0].size() > 0)
		{

			spawnDelay -= deltaTime;

			if (spawnDelay <= 0)
			{

				// Spawn Next Ball
				balls.push_back(Ball{ this, roundSpawns[0][0].GetID() });

				// Add Next Spawn Delay
				spawnDelay += roundSpawns[0][0].GetDelay();

				// Delete From Spawn Queue
				roundSpawns[0].erase(roundSpawns[0].begin());

				// Reset Pitcher Animation
				pitcherAnimIndex = 1;
				pitcherAnimTimer = 0;

			}

		}

		if (pitcherAnimIndex > 0)
		{

			pitcherAnimTimer += deltaTime;

			if (pitcherAnimTimer >= 0.05f)
			{

				pitcherAnimTimer -= 0.05f;

				pitcherAnimIndex++;
				if (pitcherAnimIndex > 3) { pitcherAnimIndex = 0; }

			}

		}

		player.Update();

		for (int i = balls.size() - 1; i >= 0; i--)
		{

			// Update Ball
			if (balls[i].GetExitCode() == 0) { balls[i].Update(); }

			else
			{

				// Hit Ball
				if (balls[i].GetExitCode() == 1) { OnBallHit(); }

				// Missed Ball
				else { misses++; totalMisses++; }

				// Delete Ball
				balls.erase(std::next(balls.begin(), i));

			}

		}

		if (roundSpawns[0].size() == 0 && balls.size() == 0 && player.isOnGround())
		{

			// Remove Current Round Spawns
			roundSpawns.erase(roundSpawns.begin());

			// Set Pitcher Reaction
			pitcherAnimIndex = misses == 0 || (roundSpawns.size() == 0 && misses < 3) ? 5 : misses < 3 ? 6 : hits > 0 ? 7 : 8;

			stepCounter = 0;
			stepTimer = 0;

			currentState = Results;

		}

		break;

	case Results:

		if (stepCounter == 0)
		{

			stepTimer += deltaTime;

			if (stepTimer > 2)
			{

				stepTimer -= 2;

				// Game Over
				if (misses > 2) { currentState = GameOver; }

				// More Rounds Left
				else if (roundSpawns.size() > 0)
				{

					clearedRounds++;
					speedMod++;

					totalMisses += misses;
					misses = 0;

					currentState = Playing;

				}

				// Last Round Cleared
				else { stepCounter++; }

			}

		}

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

	case GameOver:

		stepTimer += deltaTime;

		float delays[5] { 3, 1, 2, 1, 3 };

		if (stepTimer > delays[stepCounter])
		{

			// Exit To Main Menu
			if (stepCounter == 4) {}

			// Next Step
			else
			{
				stepTimer -= delays[stepCounter];
				stepCounter++;
			}

		}

		break;

	}

}

void Arena::OnDraw()
{

	// Draw Spawned Balls
	for (int i = 0; i < balls.size(); i++) { balls[i].OnDraw(); }

	// Draw Player
	player.OnDraw();

	// Draw Player Platform
	DrawTexture(GetTexture(2), 32, 278, WHITE);

	// Draw Pitcher
	DrawTextureRec(GetTexture(4), Rectangle{ (float)pitcherAnimIndex * 18, 0, 18, 32 }, Vector2{ 238, 224 }, WHITE);

	// Draw Pitcher Platform
	DrawTexture(GetTexture(5), 224, 256, WHITE);

	// Draw Ashley Platform
	DrawTexture(GetTexture(6), 130, 260, WHITE);

	// Draw Health Bar
	for (int i = 0; i < 3; i++) { DrawTextureRec(GetCommonTexture(2), Rectangle{ (float)(2 - misses < i ? 0 : 1) * 16, 0, 16, 16 }, Vector2{ (float)64 + 20 * i, 96 }, WHITE); }

	// Draw Game Over UI
	if (currentState == GameOver)
	{

		for (int i = -1; i < stepCounter; i++)
		{

			switch (i + 1)
			{

			case 0:

				DrawBox(8, Vector2{ 96, 16 }, Vector2{ 152, 164 }, Color{ 200, 200, 200, 255 });

				DrawTextCharAtlas("Game Over", Vector2{ 152, 160 }, Color{ 225, 225, 225, 255 }, 1);

				break;

			case 1:

				DrawBox(8, Vector2{ 96, 100 }, Vector2{ 152, 242 }, Color{ 200, 200, 200, 255 });

				DrawTextCharAtlas("Score", Vector2{ 152, 208 }, Color{ 225, 225, 225, 255 }, 1);

				break;

			case 2: DrawTextCharAtlas(TextFormat("%i", score), Vector2{ 152, 220 }, Color{ 225, 225, 225, 255 }, 1); break;

			case 3: DrawTextureRec(GetCommonTexture(1), Rectangle{ 0, 0, 16, 16 }, Vector2{ 144, 248 }, Color{ 225, 225, 225, 255 }); break;

			case 4: DrawTextCharAtlas(TextFormat("%i", GetTokens()), Vector2{ 152, 268 }, Color{ 225, 225, 225, 255 }, 1); break;

			}

		}

	}

	// Draw Final Results UI
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

				if (totalMisses == 0)
				{
					DrawTextCharAtlas("[BONUS]", Vector2{ 152, 212 }, Color{ 225, 225, 225, 255 }, 1);
					DrawTextCharAtlas(TextFormat("%i", GetBonus()), Vector2{ 152, 224 }, Color{ 225, 225, 225, 255 }, 1);
				}

				break;

			}

		}

	}

}