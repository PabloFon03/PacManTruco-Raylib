#include "Maze.h"
#include "Random.h"

using namespace PacMan_Board;

Board::Board(Resources* _res, int _difficulty, int _item1, int _item2, int _item3, int _charm)
{

	resources = _res;

	shaderFlipValLocation = GetShaderLocation(resources->GetShader(1), "flipVal");

	itemIcons = GetTexture(0);
	charmIcons = GetTexture(1);
	uiIcons = GetTexture(2);
	roundCounter = GetTexture(3);
	rewindIcon = GetTexture(28);
	CrystalAnimation.animAtlas = GetTexture(29);

	tokenIcon = GetCommonTexture(1);
	tokenID = Random().RandomInt(5);

	difficulty = _difficulty;

	player = Player{ this, &grid, _item1, _item2, _item3 };

	switch (_charm)
	{

		// Score x2
	case 1: scoreMod = 2; break;

		// 5 Extra Lifes
	case 2: lifesLeft = 5; break;

		// 1 Crystal
	case 3: crystalsLeft = 1; break;

		// Score x4
	case 4: scoreMod = 4; break;

		// 7 Extra Lifes
	case 5: lifesLeft = 7; break;

		// 3 Crystals
	case 6: crystalsLeft = 3; break;

		// Locket
	case 7: lifesLeft = 9; crystalsLeft = 7; break;

	}

	RefillMazeQueue(false);
	SpawnNextMaze();

}

Board::~Board()
{

	RemoveAllEnemies();

}

void Board::Update()
{

	switch (currentState)
	{

	case Starting:
	{
		stepTimer += GetDeltaTime();

		float targetTimer;
		targetTimer = stepCounter == 0 ? 2 : 0.75f;

		if (stepTimer >= targetTimer)
		{
			stepCounter++;
			stepTimer -= targetTimer;
		}

		if (stepCounter == 6) { currentState = Playing; }

		break;
	}
	case Playing:
	{
		// Update Timer
		timeLeft -= GetRawDeltaTime();
		if (timeLeft <= 0)
		{
			timeLeft = 0;

			currentState = TimeUp;
			stepCounter = 0;
			stepTimer = 0;
		}

		// Update Player
		player.Update();

		// Update Player Projectiles
		for (int i = playerProjectiles.size() - 1; i >= 0; i--)
		{
			playerProjectiles[i].Update();
			if (playerProjectiles[i].IsOutOfBounds()) { playerProjectiles.erase(std::next(playerProjectiles.begin(), i)); }
		}

		if (IsKeyPressed(KEY_LEFT_CONTROL) && crystalsLeft > 0)
		{
			CrystalAnimation.isPlaying = true;
			CrystalAnimation.frameIndex = 0;
			CrystalAnimation.frameDelay = 0;
			crystalsLeft--;
		}

		// Update Dispawn Animation
		if (CrystalAnimation.isPlaying)
		{

			CrystalAnimation.frameDelay += GetDeltaTime();

			if (CrystalAnimation.frameDelay >= 0.1f)
			{
				CrystalAnimation.frameIndex++;
				if (CrystalAnimation.frameIndex == 6)
				{
					CrystalAnimation.isPlaying = false;
					RemoveAllEnemies();
				}
				CrystalAnimation.frameDelay -= 0.1f;
			}

		}

		else
		{

			// Update Enemies
			for (int i = enemies.size() - 1; i >= 0; i--)
			{
				(*enemies[i]).Update();
				if ((*enemies[i]).GetRemoveFlag()) { RemoveEnemy(i); }
			}

		}

		break;
	}
	case FlipOut:
	case FlipIn:
	{
		stepTimer += 4 * GetRawDeltaTime();

		if (stepTimer >= 1)
		{

			stepTimer--;

			if (currentState == FlipIn)
			{
				renderShader = 0;
				currentState = Playing;
			}

			else
			{
				SpawnNextMaze();
				currentState = FlipIn;
			}

		}

		else
		{
			float val = currentState == FlipIn ? 1 - stepTimer : stepTimer;
			SetShaderValue(resources->GetShader(1), shaderFlipValLocation, &val, SHADER_UNIFORM_FLOAT);
		}

		break;
	}
	case Defeated:
	{
		stepTimer += GetRawDeltaTime();

		float delay;
		delay = stepCounter == 0 ? 2 : 1;
		if (stepTimer >= delay)
		{

			// Update Counter
			stepCounter++;

			// Resume Play
			if (stepCounter == 2) { currentState = Playing; }

			// Reset Player And Enemies To Spawn
			else
			{
				player.Respawn();
				for (int i = 0; i < enemies.size(); i++) { (*enemies[i]).Respawn(); }
				renderShader = 0;
			}

			// Reset Timer
			stepTimer -= delay;

		}

		break;
	}
	case TimeUp:
	case GameOver:
	{
		stepTimer += GetRawDeltaTime();
		if (stepTimer >= 3)
		{
			stepTimer -= 3;
			currentState = Results;
		}

		break;
	}
	case Results:
	{
		stepTimer += GetRawDeltaTime();
		float delay = stepCounter == 17 ? 5 : stepCounter == 11 ? 3 : stepCounter == 0 || stepCounter == 12 ? 1.5f : 0.75f;
		if (stepTimer >= delay)
		{
			stepCounter++;
			if (stepCounter == 18) { exitFlag = 40; }
			stepTimer -= delay;
		}

		break;
	}
	}

}

void Board::OnDraw()
{

	if (currentState == Results)
	{

		if (stepCounter > 11)
		{

			for (int i = 13; i < stepCounter; i++)
			{

				switch (i - 13)
				{

					// Final Score
				case 0: DrawTextCharAtlas("Final Score", Vector2{ 152, 156 }, WHITE, 1); break;
				case 1: DrawTextCharAtlas(std::to_string(GetFinalScore()), Vector2{ 152, 168 }, WHITE, 1); break;

					// Tokens
				case 2: DrawTextureRec(tokenIcon, Rectangle{ 0, (float)(16 * tokenID), 16, 16 }, Vector2{ 144, 264 }, WHITE); break;
				case 3: DrawTextCharAtlas(std::to_string(GetTokens()), Vector2{ 152, 284 }, WHITE, 1); break;

				}

			}

		}

		else
		{

			for (int i = 1; i < stepCounter; i++)
			{

				switch (i - 1)
				{

					// Score
				case 0: DrawTextCharAtlas("Score", Vector2{ 152, 126 }, Color{ 102, 216, 255, 255 }, 1); break;
				case 1: DrawTextCharAtlas(std::to_string(GetScore()), Vector2{ 152, 138 }, Color{ 102, 216, 255, 255 }, 1); if (scoreMod > 1) { for (int i = 0; i < 2; i++) { DrawTextureRec(charmIcons, Rectangle{ 16 * (float)(scoreMod == 4 ? 4 : 1), 0, 16, 16 }, Vector2{ (float)(104 + 80 * i), 122 }, WHITE); } } break;

					// Speed
				case 2: DrawTextCharAtlas(TextFormat("[%i] Speed Bonus [%i]", speedMod + 1, speedMod + 1), Vector2{ 152, 170 }, Color{ 255, 228, 102, 255 }, 1); break;
				case 3: DrawTextCharAtlas(std::to_string(GetSpeedBonusScore()), Vector2{ 152, 182 }, Color{ 255, 228, 102, 255 }, 1); break;

					// Extra Energy
				case 4: DrawTextCharAtlas(TextFormat("<%i> Energy Crystals Bonus <%i>", player.GetEnergy(), player.GetEnergy()), Vector2{ 152, 214 }, Color{ 102, 255, 140, 255 }, 1); break;
				case 5: DrawTextCharAtlas(std::to_string(GetEnergyBonusScore()), Vector2{ 152, 226 }, Color{ 102, 255, 140, 255 }, 1); break;

					// Extra Lifes
				case 6: DrawTextCharAtlas(TextFormat("(%i) Extra Lifes Bonus (%i)", lifesLeft, lifesLeft), Vector2{ 152, 258 }, Color{ 255, 102, 216, 255 }, 1); break;
				case 7: DrawTextCharAtlas(std::to_string(GetExtraLifeBonusScore()), Vector2{ 152, 270 }, Color{ 255, 102, 216, 255 }, 1); break;

					// Extra Crystals
				case 8: DrawTextCharAtlas(TextFormat("<%i> Crystals Bonus <%i>", crystalsLeft, crystalsLeft), Vector2{ 152, 302 }, Color{ 191, 102, 255, 255 }, 1); break;
				case 9: DrawTextCharAtlas(std::to_string(GetCrystalBonusScore()), Vector2{ 152, 314 }, Color{ 191, 102, 255, 255 }, 1); break;

				}

			}

		}

	}

	else
	{

		// Draw Maze Grid
		grid.OnDraw(clearedRounds % 6);

		if (CrystalAnimation.isPlaying)
		{

			// Draw Player
			player.OnDraw();

			// Draw Enemy Dispawn Animation
			for (int i = 0; i < enemies.size(); i++) { DrawTextureRec(CrystalAnimation.animAtlas, Rectangle{ (float)(16 * CrystalAnimation.frameIndex), 0, 16, 16 }, Vector2{ enemies[i]->GetRawCoords().x * 16, enemies[i]->GetRawCoords().y * 16 + 48 }, WHITE); }

		}

		else
		{

			// List All Entities
			std::vector<Entity*> entities = std::vector<Entity*>();
			entities.push_back(&player);
			for (int i = 0; i < enemies.size(); i++) { entities.push_back(enemies[i]); }

			// Selection Sort
			for (int i = 0; i < entities.size() - 1; i++)
			{
				int targetIndex = i;
				for (int i = targetIndex + 1; i < entities.size(); i++) { if ((*entities[i]).GetRawCoords().y < (*entities[targetIndex]).GetRawCoords().y) { targetIndex = i; } }
				if (targetIndex != i) { std::swap(entities[i], entities[targetIndex]); }
			}

			// Draw Entities From Top To Bottom
			for (int i = 0; i < entities.size(); i++) { (*entities[i]).OnDraw(); }

		}

		// Draw Player Projectiles
		for (int i = 0; i < playerProjectiles.size(); i++) { playerProjectiles[i].OnDraw(); }

		switch (currentState)
		{

		case Starting:

			switch (stepCounter)
			{

			case 0:

				DrawBox(8, Vector2{ 96, 32 }, Vector2{ 152, 224 }, SKYBLUE);
				DrawTextCharAtlas("READY", Vector2{ 152, 220 }, SKYBLUE, 1);
				break;

			case 2:

				DrawBox(8, Vector2{ 32, 32 }, Vector2{ 152, 224 }, RED);
				DrawTextCharAtlas("3", Vector2{ 152, 220 }, RED, 1);
				break;

			case 3:

				DrawBox(8, Vector2{ 32, 32 }, Vector2{ 152, 224 }, YELLOW);
				DrawTextCharAtlas("2", Vector2{ 152, 220 }, YELLOW, 1);
				break;

			case 4:

				DrawBox(8, Vector2{ 32, 32 }, Vector2{ 152, 224 }, GREEN);
				DrawTextCharAtlas("1", Vector2{ 152, 220 }, GREEN, 1);
				break;

			case 5:

				DrawBox(8, Vector2{ 64, 32 }, Vector2{ 152, 224 }, SKYBLUE);
				DrawTextCharAtlas("GO", Vector2{ 152, 220 }, SKYBLUE, 1);
				break;

			}

			break;

		case Defeated:

			if (stepTimer < 3) { DrawTexture(rewindIcon, 16, 64, WHITE); }

			break;

		case GameOver:

			if (stepTimer > 1)
			{
				DrawBox(8, Vector2{ 96, 24 }, Vector2{ 152, 224 }, Color{ 225, 225, 225, 255 });
				DrawTextCharAtlas("Game Over", Vector2{ 152, 220 }, Color{ 225, 225, 225, 255 }, 1);
			}

			break;

		case TimeUp:

			DrawBox(8, Vector2{ 96, 24 }, Vector2{ 152, 224 }, Color{ 225, 225, 225, 255 });
			DrawTextCharAtlas("Time Up!", Vector2{ 152, 220 }, Color{ 225, 225, 225, 255 }, 1);

			break;

		}

		// Top UI
		DrawRectangle(0, 0, 304, 48, BLACK);

		// Draw Score
		DrawBox(8, Vector2{ 80, 28 }, Vector2{ 62, 24 }, Color{ 102, 216, 255, 255 });
		DrawTextCharAtlas("Score", Vector2{ 62, 14 }, Color{ 102, 216, 255, 255 }, 1);
		DrawTextCharAtlas(std::to_string(score), Vector2{ 62, 26 }, Color{ 102, 216, 255, 255 }, 1);

		// Draw Timer
		DrawBox(8, Vector2{ 48, 28 }, Vector2{ 152, 24 }, Color{ 225, 225, 225, 255 });
		DrawTextCharAtlas(GetTimerDisplayValue(), Vector2{ 152, 20 }, Color{ 225, 225, 225, 255 }, 1);

		// Draw Speed
		DrawBox(8, Vector2{ 80, 28 }, Vector2{ 242, 24 }, Color{ 255, 228, 102, 255 });
		DrawTextCharAtlas("Speed", Vector2{ 242, 14 }, Color{ 255, 228, 102, 255 }, 1);
		DrawTextCharAtlas(std::to_string(speedMod + 1), Vector2{ 242, 26 }, Color{ 255, 228, 102, 255 }, 1);

		// Bottom UI
		DrawRectangle(0, 400, 304, 48, BLACK);

		// Draw Item Icons
		Color itemTint = player.UsingItem() ? GRAY : WHITE;
		for (int i = 0; i < 3; i++)
		{
			int currentItem = player.GetItemIndex(i);
			DrawTextureRec(itemIcons, Rectangle{ (float)(16 * currentItem), (float)(16 * i), 16, 16 }, Vector2{ (float)(4 + 20 * i), 416 }, itemTint);
			if (currentItem > 0 && !Items::GetItem(currentItem).IsElectric()) { DrawTextCharAtlas(TextFormat("%i", Items::GetItem(currentItem).GetManaCost()), Vector2{ (float)(12 + 20 * i), 436 }, itemTint, 1); }
		}

		// Draw Electric Energy Bar
		DrawBox(8, Vector2{ 56, 12 }, Vector2{ 98, 424 }, Color{ 255, 228, 102, 255 });
		DrawRectangle(70, 418, round(56 * player.GetElectricCharge()), 12, WHITE);

		// Draw Round Counter
		DrawTextureRec(roundCounter, Rectangle{ (float)(clearedRounds % 6) * 32, 0, 32, 32 }, Vector2{ 136, 408 }, WHITE);

		// Draw Vessels / Gems
		DrawBox(8, Vector2{ 52, 28 }, Vector2{ 204, 424 }, Color{ 102, 255, 140, 255 });

		// Blank Vessels Left
		DrawTextureRec(uiIcons, Rectangle{ 0, 0, 8, 8 }, Vector2{ 182, 414 }, WHITE);
		DrawTextCharAtlas(std::to_string(dotGoal - dotsCollected), Vector2{ 226, 414 }, WHITE, 2);

		// Energy Left
		DrawTextureRec(uiIcons, Rectangle{ 8, 0, 8, 8 }, Vector2{ 182, 426 }, WHITE);
		DrawTextCharAtlas(std::to_string(player.GetEnergy()), Vector2{ 226, 426 }, WHITE, 2);

		// Draw Lifes / Crystals
		DrawBox(8, Vector2{ 52, 28 }, Vector2{ 272, 424 }, Color{ 207, 63, 255, 255 });

		// Lifes Left
		DrawTextureRec(uiIcons, Rectangle{ 0, 8, 8, 8 }, Vector2{ 250, 414 }, WHITE);
		DrawTextCharAtlas(std::to_string(lifesLeft), Vector2{ 294, 414 }, WHITE, 2);

		// Crystals Left
		DrawTextureRec(uiIcons, Rectangle{ 8, 8, 8, 8 }, Vector2{ 250, 426 }, WHITE);
		DrawTextCharAtlas(std::to_string(crystalsLeft), Vector2{ 294, 426 }, WHITE, 2);

	}

}

void Board::ClearTile(int _i) { grid.ClearTile(_i); }

void Board::AddScore(int _s)
{
	score += _s;
	speedScore += _s;
	while (speedScore >= 500)
	{
		speedScore -= 500;
		if (speedMod < 24) { speedMod++; }
		else { score += 2500; }
	}
}

void Board::DotCollected()
{
	dotsCollected++;
	if (dotsCollected == dotGoal)
	{
		renderShader = 1;
		stepTimer = 0;
		currentState = FlipOut;
	}
}

void Board::ShootProjectile(Vector2 _pos, Vector2 _dir, int _dirIndex) { playerProjectiles.push_back(Projectile{ this, _pos, _dir, _dirIndex }); }

void Board::OnPlayerHit()
{

	lifesLeft--;

	currentState = lifesLeft == 0 ? GameOver : Defeated;
	stepCounter = 0;
	stepTimer = 0;

	if (currentState == Defeated) { renderShader = 2; }

}

void Board::RefillMazeQueue(bool _random)
{

	std::vector<int> mazeVec = std::vector<int>(0);

	int ranges[3]{ 15, 10, 5 };

	int startPoint = 0;

	for (int i = 0; i < difficulty; i++) { startPoint += ranges[i]; }

	for (int i = 0; i < ranges[difficulty]; i++) { mazeVec.push_back(i + startPoint); }

	if (_random)
	{

		Random r = Random();

		std::vector<int> shuffledVec = std::vector<int>(0);

		while (mazeVec.size() > 0)
		{

			int n = r.RandomInt(mazeVec.size());

			shuffledVec.push_back(mazeVec[n]);
			mazeVec.erase(std::next(mazeVec.begin(), n));

		}

		mazeIDs = shuffledVec;

	}

	else { mazeIDs = mazeVec; }

}

Board::MazeSpawnData Board::ReturnMazeSpawnData(int _i)
{

	MazeSpawnData returnData;

	switch (_i)
	{

	case 0:

		returnData.gridTiles = std::vector<int>
		{
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 4, 11, 11, 11, 11, 11, 11, 11, 11, 11, 10, 0, 0, 0, 0,
			0, 0, 0, 0, 6, 17, 17, 17, 17, 17, 17, 17, 17, 17, 6, 0, 0, 0, 0,
			0, 0, 0, 0, 6, 17, 4, 11, 11, 11, 11, 11, 10, 17, 6, 0, 0, 0, 0,
			0, 0, 0, 0, 6, 17, 6, 0, 0, 0, 0, 0, 6, 17, 6, 0, 0, 0, 0,
			0, 0, 0, 0, 6, 17, 6, 0, 0, 0, 0, 0, 6, 17, 6, 0, 0, 0, 0,
			0, 0, 0, 0, 6, 17, 7, 11, 11, 11, 11, 11, 13, 17, 6, 0, 0, 0, 0,
			0, 0, 0, 0, 6, 17, 17, 17, 0, 0, 0, 17, 17, 17, 6, 0, 0, 0, 0,
			0, 0, 0, 0, 7, 11, 11, 11, 11, 11, 11, 11, 11, 11, 13, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		};

		returnData.playerSpawn = PlayerSpawnData{ Entity::Vector2Int{ 9, 12 }, 3 };

		break;

	case 1:

		returnData.gridTiles = std::vector<int>
		{
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 4, 11, 11, 11, 11, 11, 11, 11, 10, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 6, 17, 17, 0, 0, 0, 17, 17, 6, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 6, 17, 3, 11, 12, 11, 9, 17, 6, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 6, 17, 17, 17, 6, 17, 17, 17, 6, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 7, 11, 10, 17, 6, 17, 4, 11, 13, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 6, 17, 6, 17, 6, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 6, 17, 5, 17, 6, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 6, 17, 17, 17, 6, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 7, 11, 11, 11, 13, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		};

		returnData.playerSpawn = PlayerSpawnData{ Entity::Vector2Int{ 9, 9 }, 1 };

		break;

	case 2:

		returnData.gridTiles = std::vector<int>
		{
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 4, 11, 11, 11, 11, 11, 11, 11, 10, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 6, 17, 17, 17, 17, 17, 17, 17, 6, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 6, 17, 4, 11, 11, 11, 10, 17, 6, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 6, 17, 7, 11, 11, 11, 13, 17, 6, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 6, 17, 17, 17, 17, 17, 17, 17, 6, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 6, 17, 4, 11, 11, 11, 10, 17, 6, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 6, 17, 7, 11, 11, 11, 13, 17, 6, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 6, 17, 17, 17, 17, 17, 17, 17, 6, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 6, 17, 4, 11, 11, 11, 10, 17, 6, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 6, 17, 7, 11, 11, 11, 13, 17, 6, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 6, 17, 17, 0, 0, 0, 17, 17, 6, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 7, 11, 11, 11, 11, 11, 11, 11, 13, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		};

		returnData.playerSpawn = PlayerSpawnData{ Entity::Vector2Int{ 9, 15 }, 3 };

		break;

	case 3:

		returnData.gridTiles = std::vector<int>
		{
			0, 0, 0, 0, 0, 0, 0, 0, 6, 17, 6, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 6, 17, 6, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 6, 17, 6, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 4, 11, 11, 11, 13, 0, 7, 11, 11, 11, 10, 0, 0, 0, 0,
			0, 0, 0, 0, 6, 17, 17, 17, 0, 0, 0, 17, 17, 17, 6, 0, 0, 0, 0,
			0, 0, 0, 0, 6, 17, 4, 11, 11, 11, 11, 11, 10, 17, 6, 0, 0, 0, 0,
			0, 0, 0, 0, 6, 17, 7, 11, 11, 11, 11, 11, 13, 17, 6, 0, 0, 0, 0,
			0, 0, 0, 0, 6, 17, 17, 17, 0, 0, 0, 17, 17, 17, 6, 0, 0, 0, 0,
			0, 0, 0, 0, 7, 11, 11, 11, 10, 0, 4, 11, 11, 11, 13, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 6, 17, 6, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 6, 17, 6, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 6, 17, 6, 0, 0, 0, 0, 0, 0, 0, 0
		};

		returnData.playerSpawn = PlayerSpawnData{ Entity::Vector2Int{ 9, 12 }, 3 };

		break;

	case 4:

		returnData.gridTiles = std::vector<int>
		{
			0, 0, 0, 0, 0, 6, 17, 7, 13, 17, 7, 13, 17, 6, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 6, 17, 17, 17, 17, 17, 17, 17, 6, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 7, 11, 11, 10, 17, 4, 11, 11, 13, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 6, 17, 6, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 4, 11, 11, 11, 13, 0, 7, 11, 11, 11, 10, 0, 0, 0, 0,
			0, 0, 0, 0, 6, 17, 17, 17, 0, 0, 0, 17, 17, 17, 6, 0, 0, 0, 0,
			0, 0, 0, 0, 6, 17, 4, 11, 11, 11, 11, 11, 10, 17, 6, 0, 0, 0, 0,
			0, 0, 0, 0, 6, 17, 7, 11, 11, 11, 11, 11, 13, 17, 6, 0, 0, 0, 0,
			0, 0, 0, 0, 6, 17, 17, 17, 0, 0, 0, 17, 17, 17, 6, 0, 0, 0, 0,
			0, 0, 0, 0, 7, 11, 11, 11, 10, 0, 4, 11, 11, 11, 13, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 6, 17, 6, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 4, 11, 11, 13, 17, 7, 11, 11, 10, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 6, 17, 17, 17, 17, 17, 17, 17, 6, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 6, 17, 4, 10, 17, 4, 10, 17, 6, 0, 0, 0, 0, 0
		};

		returnData.playerSpawn = PlayerSpawnData{ Entity::Vector2Int{ 9, 12 }, 3 };

		break;

	case 5:

		returnData.gridTiles = std::vector<int>
		{
			4, 11, 11, 11, 11, 11, 11, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			6, 17, 17, 17, 17, 17, 17, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			6, 17, 4, 11, 11, 10, 17, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			6, 17, 6, 0, 0, 6, 17, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			6, 17, 6, 0, 0, 6, 17, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			6, 0, 7, 11, 11, 13, 17, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			6, 0, 0, 17, 17, 17, 17, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			7, 11, 11, 11, 11, 11, 11, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		};

		returnData.playerSpawn = PlayerSpawnData{ Entity::Vector2Int{ 1, 6 }, 0 };

		break;

	case 6:

		returnData.gridTiles = std::vector<int>
		{
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 11, 11, 11, 11, 11, 11, 10,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 17, 17, 17, 17, 17, 17, 6,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 17, 4, 11, 11, 10, 17, 6,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 17, 6, 0, 0, 6, 17, 6,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 17, 6, 0, 0, 6, 17, 6,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 17, 7, 11, 11, 13, 0, 6,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 17, 17, 17, 17, 0, 0, 6,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 11, 11, 11, 11, 11, 11, 13,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		};

		returnData.playerSpawn = PlayerSpawnData{ Entity::Vector2Int{ 17, 6 }, 0 };

		break;

	case 7:

		returnData.gridTiles = std::vector<int>
		{
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			4, 11, 11, 11, 11, 11, 11, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			6, 0, 0, 17, 17, 17, 17, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			6, 0, 4, 11, 11, 10, 17, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			6, 17, 6, 0, 0, 6, 17, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			6, 17, 6, 0, 0, 6, 17, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			6, 17, 7, 11, 11, 13, 17, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			6, 17, 17, 17, 17, 17, 17, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			7, 11, 11, 11, 11, 11, 11, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		};

		returnData.playerSpawn = PlayerSpawnData{ Entity::Vector2Int{ 1, 15 }, 2 };

		break;

	case 8:

		returnData.gridTiles = std::vector<int>
		{
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 11, 11, 11, 11, 11, 11, 10,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 17, 17, 17, 17, 0, 0, 6,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 17, 4, 11, 11, 10, 0, 6,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 17, 6, 0, 0, 6, 17, 6,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 17, 6, 0, 0, 6, 17, 6,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 17, 7, 11, 11, 13, 17, 6,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 17, 17, 17, 17, 17, 17, 6,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 11, 11, 11, 11, 11, 11, 13
		};

		returnData.playerSpawn = PlayerSpawnData{ Entity::Vector2Int{ 17, 15 }, 2 };

		break;

	case 9:

		returnData.gridTiles = std::vector<int>
		{
			4, 11, 11, 11, 11, 11, 11, 10, 0, 2, 0, 4, 11, 11, 11, 11, 11, 11, 10,
			6, 0, 0, 0, 0, 0, 0, 6, 0, 6, 0, 6, 0, 0, 0, 0, 0, 0, 6,
			6, 0, 4, 11, 11, 10, 0, 6, 0, 6, 0, 6, 0, 4, 11, 11, 10, 0, 6,
			6, 0, 6, 0, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 0, 6, 0, 6,
			6, 0, 6, 0, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 0, 6, 0, 6,
			6, 0, 7, 11, 11, 13, 0, 5, 0, 5, 0, 5, 0, 7, 11, 11, 13, 0, 6,
			6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
			7, 11, 11, 11, 9, 0, 4, 11, 11, 11, 11, 11, 10, 0, 3, 11, 11, 11, 13,
			0, 0, 0, 0, 0, 0, 6, 17, 17, 18, 17, 17, 6, 0, 0, 0, 0, 0, 0,
			4, 11, 11, 11, 10, 0, 6, 17, 4, 11, 10, 17, 6, 0, 4, 11, 11, 11, 10,
			6, 0, 0, 0, 6, 0, 6, 17, 6, 0, 6, 17, 6, 0, 6, 0, 0, 0, 6,
			6, 0, 0, 0, 6, 0, 6, 17, 6, 0, 6, 17, 6, 0, 6, 0, 0, 0, 6,
			7, 11, 11, 11, 13, 0, 6, 17, 7, 11, 13, 17, 6, 0, 7, 11, 11, 11, 13,
			0, 0, 0, 0, 0, 0, 6, 17, 0, 0, 0, 17, 6, 0, 0, 0, 0, 0, 0,
			4, 11, 11, 11, 9, 0, 7, 11, 11, 11, 11, 11, 13, 0, 3, 11, 11, 11, 10,
			6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
			6, 0, 4, 11, 11, 10, 0, 2, 0, 2, 0, 2, 0, 4, 11, 11, 10, 0, 6,
			6, 0, 6, 0, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 0, 6, 0, 6,
			6, 0, 6, 0, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 0, 6, 0, 6,
			6, 0, 7, 11, 11, 13, 0, 6, 0, 6, 0, 6, 0, 7, 11, 11, 13, 0, 6,
			6, 0, 0, 0, 0, 0, 0, 6, 0, 6, 0, 6, 0, 0, 0, 0, 0, 0, 6,
			7, 11, 11, 11, 11, 11, 11, 13, 0, 5, 0, 7, 11, 11, 11, 11, 11, 11, 13
		};

		returnData.playerSpawn = PlayerSpawnData{ Entity::Vector2Int{ 9, 13 }, 1 };

		break;

	case 10:

		returnData.gridTiles = std::vector<int>
		{
			0, 0, 0, 0, 0, 0, 0, 6, 17, 6, 17, 6, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 4, 11, 11, 13, 17, 6, 17, 7, 11, 11, 10, 0, 0, 0, 0,
			0, 0, 0, 0, 6, 17, 17, 17, 17, 6, 17, 17, 17, 17, 6, 0, 0, 0, 0,
			0, 0, 0, 0, 6, 17, 3, 11, 11, 16, 11, 11, 9, 17, 6, 0, 0, 0, 0,
			0, 0, 0, 0, 6, 17, 17, 17, 17, 6, 17, 17, 17, 17, 6, 0, 0, 0, 0,
			0, 0, 0, 0, 7, 11, 11, 10, 17, 5, 17, 4, 11, 11, 13, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 6, 17, 17, 17, 6, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 7, 11, 11, 11, 13, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 4, 11, 11, 11, 10, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 4, 11, 11, 13, 17, 2, 17, 7, 11, 11, 10, 0, 0, 0, 0,
			0, 0, 0, 0, 6, 17, 17, 17, 17, 6, 17, 17, 17, 17, 6, 0, 0, 0, 0,
			0, 0, 0, 0, 6, 17, 3, 11, 11, 16, 11, 11, 9, 17, 6, 0, 0, 0, 0,
			0, 0, 0, 0, 6, 17, 17, 17, 17, 6, 17, 17, 17, 17, 6, 0, 0, 0, 0,
			0, 0, 0, 0, 7, 11, 11, 10, 17, 6, 17, 4, 11, 11, 13, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 6, 17, 6, 17, 6, 0, 0, 0, 0, 0, 0, 0
		};

		returnData.playerSpawn = PlayerSpawnData{ Entity::Vector2Int{ 9, 15 }, 3 };

		break;

	case 11:

		returnData.gridTiles = std::vector<int>
		{
			0, 0, 0, 0, 0, 0, 6, 17, 6, 0, 6, 17, 6, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 6, 17, 7, 11, 13, 17, 6, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 6, 17, 17, 17, 17, 17, 6, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 7, 11, 10, 0, 4, 11, 13, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 6, 18, 6, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 4, 11, 13, 0, 7, 11, 10, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 6, 17, 17, 17, 17, 17, 6, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 6, 17, 4, 11, 10, 17, 6, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 6, 17, 6, 0, 6, 17, 6, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 6, 17, 6, 0, 6, 17, 6, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 6, 17, 7, 11, 13, 17, 6, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 6, 17, 0, 0, 0, 17, 6, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 7, 11, 10, 0, 4, 11, 13, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 6, 18, 6, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 4, 11, 13, 0, 7, 11, 10, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 6, 17, 17, 17, 17, 17, 6, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 6, 17, 4, 11, 10, 17, 6, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 6, 17, 6, 0, 6, 17, 6, 0, 0, 0, 0, 0, 0
		};

		returnData.playerSpawn = PlayerSpawnData{ Entity::Vector2Int{ 9, 13 }, 3 };

		break;

	case 12:

		returnData.gridTiles = std::vector<int>
		{
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 4, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 10, 0, 0,
			0, 0, 6, 20, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 21, 6, 0, 0,
			0, 0, 6, 17, 4, 11, 11, 11, 9, 21, 3, 11, 11, 11, 10, 17, 6, 0, 0,
			0, 0, 6, 17, 6, 20, 17, 17, 17, 17, 17, 17, 17, 21, 6, 17, 6, 0, 0,
			0, 0, 6, 17, 6, 17, 4, 11, 9, 21, 3, 11, 10, 17, 6, 17, 6, 0, 0,
			0, 0, 6, 17, 6, 17, 6, 20, 17, 17, 17, 21, 6, 17, 6, 17, 6, 0, 0,
			0, 0, 6, 17, 6, 17, 6, 17, 2, 18, 2, 17, 6, 17, 6, 17, 6, 0, 0,
			0, 0, 6, 17, 6, 17, 6, 17, 6, 17, 6, 17, 6, 17, 6, 17, 6, 0, 0,
			0, 0, 6, 17, 6, 17, 6, 17, 6, 17, 5, 17, 6, 17, 6, 17, 6, 0, 0,
			0, 0, 6, 17, 6, 17, 6, 17, 6, 17, 17, 17, 6, 17, 6, 17, 6, 0, 0,
			0, 0, 6, 17, 6, 17, 6, 17, 5, 19, 3, 11, 13, 17, 6, 17, 6, 0, 0,
			0, 0, 6, 17, 6, 17, 6, 19, 17, 17, 17, 17, 17, 22, 6, 17, 6, 0, 0,
			0, 0, 6, 17, 6, 17, 7, 11, 9, 19, 3, 11, 11, 11, 13, 17, 6, 0, 0,
			0, 0, 6, 19, 6, 19, 17, 17, 17, 17, 17, 17, 17, 17, 17, 22, 6, 0, 0,
			0, 0, 6, 0, 8, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 13, 0, 0,
			0, 0, 7, 11, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		};

		returnData.playerSpawn = PlayerSpawnData{ Entity::Vector2Int{ 3, 17 }, 0 };

		break;

	case 13:

		returnData.gridTiles = std::vector<int>
		{
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 4, 11, 11, 11, 12, 11, 11, 11, 12, 11, 11, 11, 10, 0, 0, 0,
			0, 0, 0, 6, 0, 0, 0, 6, 17, 17, 17, 6, 17, 17, 17, 6, 0, 0, 0,
			0, 0, 0, 8, 11, 12, 11, 13, 17, 2, 17, 8, 11, 9, 17, 6, 0, 0, 0,
			0, 0, 0, 6, 0, 6, 17, 17, 17, 6, 21, 6, 17, 17, 17, 6, 0, 0, 0,
			0, 0, 0, 8, 11, 13, 17, 4, 11, 14, 17, 6, 17, 3, 11, 14, 0, 0, 0,
			0, 0, 0, 6, 17, 17, 17, 6, 0, 6, 17, 6, 17, 17, 17, 6, 0, 0, 0,
			0, 0, 0, 6, 17, 3, 11, 14, 0, 6, 17, 7, 11, 10, 17, 6, 0, 0, 0,
			0, 0, 0, 6, 17, 17, 17, 6, 0, 6, 17, 17, 17, 6, 17, 6, 0, 0, 0,
			0, 0, 0, 8, 11, 10, 17, 8, 11, 15, 11, 9, 17, 6, 17, 6, 0, 0, 0,
			0, 0, 0, 6, 0, 6, 17, 6, 17, 17, 17, 17, 17, 6, 17, 6, 0, 0, 0,
			0, 0, 0, 6, 0, 6, 19, 6, 17, 3, 11, 11, 11, 13, 17, 6, 0, 0, 0,
			0, 0, 0, 6, 0, 6, 0, 6, 17, 17, 20, 17, 17, 17, 17, 6, 0, 0, 0,
			0, 0, 0, 7, 11, 15, 11, 15, 11, 11, 11, 11, 11, 11, 11, 13, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		};

		returnData.playerSpawn = PlayerSpawnData{ Entity::Vector2Int{ 6, 15 }, 0 };

		break;

	case 14:

		returnData.gridTiles = std::vector<int>
		{
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
			0, 17, 17, 17, 17, 17, 17, 0, 0, 0, 0, 0, 17, 17, 17, 17, 17, 17, 0,
			9, 17, 3, 11, 11, 9, 17, 4, 9, 0, 3, 10, 17, 3, 11, 11, 9, 17, 3,
			0, 17, 17, 17, 17, 17, 17, 6, 17, 17, 17, 6, 17, 17, 17, 17, 17, 17, 0,
			10, 17, 4, 11, 11, 10, 17, 6, 17, 2, 17, 6, 17, 4, 11, 11, 10, 17, 4,
			13, 17, 7, 11, 11, 13, 17, 6, 17, 5, 17, 6, 17, 7, 11, 11, 13, 17, 7,
			0, 17, 17, 17, 17, 17, 17, 6, 17, 17, 17, 6, 17, 17, 17, 17, 17, 17, 0,
			9, 17, 3, 11, 11, 9, 17, 7, 9, 0, 3, 13, 17, 3, 11, 11, 9, 17, 3,
			0, 17, 17, 17, 17, 17, 17, 0, 0, 0, 0, 0, 17, 17, 17, 17, 17, 17, 0,
			11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		};

		returnData.playerSpawn = PlayerSpawnData{ Entity::Vector2Int{ 9, 14 }, 1 };

		break;

	case 15:

		returnData.gridTiles = std::vector<int>
		{
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 4, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 10, 0, 0, 0,
			0, 0, 0, 6, 18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 18, 6, 0, 0, 0,
			0, 0, 0, 6, 0, 4, 9, 0, 3, 11, 9, 0, 3, 10, 0, 6, 0, 0, 0,
			0, 0, 0, 6, 0, 5, 17, 17, 17, 17, 17, 17, 17, 5, 0, 6, 0, 0, 0,
			0, 0, 0, 6, 0, 0, 17, 4, 11, 11, 11, 10, 17, 0, 0, 6, 0, 0, 0,
			0, 0, 0, 6, 0, 2, 17, 6, 0, 0, 0, 6, 17, 2, 0, 6, 0, 0, 0,
			0, 0, 0, 6, 0, 5, 17, 6, 0, 0, 0, 6, 17, 5, 0, 6, 0, 0, 0,
			0, 0, 0, 6, 0, 0, 17, 7, 11, 11, 11, 13, 17, 0, 0, 6, 0, 0, 0,
			0, 0, 0, 6, 0, 2, 17, 17, 0, 0, 0, 17, 17, 2, 0, 6, 0, 0, 0,
			0, 0, 0, 6, 0, 7, 9, 0, 3, 11, 9, 0, 3, 13, 0, 6, 0, 0, 0,
			0, 0, 0, 6, 18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 18, 6, 0, 0, 0,
			0, 0, 0, 7, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 13, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		};

		returnData.playerSpawn = PlayerSpawnData{ Entity::Vector2Int{ 9, 13 }, 3 };

		returnData.enemySpawns = std::vector<EnemySpawnData>
		{
			EnemySpawnData{ 0, Entity::Vector2Int{ 7, 7 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 11, 7 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 5, 9 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 13, 9 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 5, 12 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 13, 12 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 7, 14 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 11, 14 }, 0 }
		};

		break;

	case 16:

		returnData.gridTiles = std::vector<int>
		{
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 4, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 10, 0, 0, 0,
			0, 0, 0, 6, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 6, 0, 0, 0,
			0, 0, 0, 6, 17, 4, 9, 0, 3, 11, 9, 0, 3, 10, 17, 6, 0, 0, 0,
			0, 0, 0, 6, 17, 5, 0, 0, 0, 18, 0, 0, 0, 5, 17, 6, 0, 0, 0,
			0, 0, 0, 6, 17, 0, 0, 4, 11, 11, 11, 10, 0, 0, 17, 6, 0, 0, 0,
			0, 0, 0, 6, 17, 2, 0, 6, 0, 0, 0, 6, 0, 2, 17, 6, 0, 0, 0,
			0, 0, 0, 6, 17, 5, 0, 6, 0, 0, 0, 6, 0, 5, 17, 6, 0, 0, 0,
			0, 0, 0, 6, 17, 0, 0, 7, 11, 11, 11, 13, 0, 0, 17, 6, 0, 0, 0,
			0, 0, 0, 6, 17, 2, 0, 0, 0, 18, 0, 0, 0, 2, 17, 6, 0, 0, 0,
			0, 0, 0, 6, 17, 7, 9, 0, 3, 11, 9, 0, 3, 13, 17, 6, 0, 0, 0,
			0, 0, 0, 6, 17, 17, 17, 0, 0, 0, 0, 0, 17, 17, 17, 6, 0, 0, 0,
			0, 0, 0, 7, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 13, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		};

		returnData.playerSpawn = PlayerSpawnData{ Entity::Vector2Int{ 9, 15 }, 3 };

		returnData.enemySpawns = std::vector<EnemySpawnData>
		{
			EnemySpawnData{ 0, Entity::Vector2Int{ 7, 7 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 11, 7 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 5, 9 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 13, 9 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 5, 12 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 13, 12 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 7, 14 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 11, 14 }, 0 }
		};

		break;

	case 17:

		returnData.gridTiles = std::vector<int>
		{
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 4, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 10, 0, 0,
			0, 0, 6, 17, 0, 17, 17, 17, 17, 17, 17, 17, 17, 17, 0, 17, 6, 0, 0,
			0, 0, 6, 17, 2, 17, 2, 17, 3, 11, 9, 17, 2, 17, 2, 17, 6, 0, 0,
			3, 11, 13, 17, 5, 17, 5, 17, 17, 18, 17, 17, 5, 17, 5, 17, 7, 11, 9,
			0, 0, 0, 17, 18, 17, 0, 17, 3, 11, 9, 17, 0, 17, 18, 17, 0, 0, 0,
			3, 11, 10, 17, 2, 17, 2, 17, 0, 0, 0, 17, 2, 17, 2, 17, 4, 11, 9,
			0, 0, 6, 17, 5, 17, 5, 17, 3, 11, 9, 17, 5, 17, 5, 17, 6, 0, 0,
			0, 0, 6, 17, 0, 17, 17, 17, 17, 17, 17, 17, 17, 17, 0, 17, 6, 0, 0,
			0, 0, 7, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 13, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		};

		returnData.playerSpawn = PlayerSpawnData{ Entity::Vector2Int{ 9, 12 }, 3 };

		returnData.enemySpawns = std::vector<EnemySpawnData>
		{
			EnemySpawnData{ 0, Entity::Vector2Int{ 4, 8 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 14, 8 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 6, 11 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 12, 11 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 4, 14 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 14, 14 }, 0 }
		};

		break;

	case 18:

		returnData.gridTiles = std::vector<int>
		{
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 4, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 10, 0, 0, 0,
			0, 0, 0, 6, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 6, 0, 0, 0,
			0, 0, 0, 6, 17, 3, 9, 0, 3, 11, 9, 0, 3, 9, 17, 6, 0, 0, 0,
			0, 0, 0, 6, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 6, 0, 0, 0,
			0, 0, 0, 8, 11, 11, 9, 0, 3, 11, 9, 0, 3, 11, 11, 14, 0, 0, 0,
			0, 0, 0, 6, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 6, 0, 0, 0,
			0, 0, 0, 6, 17, 2, 17, 4, 11, 11, 11, 10, 17, 2, 17, 6, 0, 0, 0,
			0, 0, 0, 6, 17, 6, 17, 6, 0, 0, 0, 6, 17, 6, 17, 6, 0, 0, 0,
			0, 0, 0, 6, 17, 6, 17, 6, 0, 0, 0, 6, 17, 6, 17, 6, 0, 0, 0,
			0, 0, 0, 6, 17, 5, 17, 7, 11, 11, 11, 13, 17, 5, 17, 6, 0, 0, 0,
			0, 0, 0, 6, 17, 17, 17, 0, 0, 0, 0, 0, 17, 17, 17, 6, 0, 0, 0,
			0, 0, 0, 8, 11, 11, 9, 0, 3, 11, 9, 0, 3, 11, 11, 14, 0, 0, 0,
			0, 0, 0, 6, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 6, 0, 0, 0,
			0, 0, 0, 6, 17, 3, 9, 0, 3, 11, 9, 0, 3, 9, 17, 6, 0, 0, 0,
			0, 0, 0, 6, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 6, 0, 0, 0,
			0, 0, 0, 7, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 13, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		};

		returnData.playerSpawn = PlayerSpawnData{ Entity::Vector2Int{ 9, 13 }, 3 };

		returnData.enemySpawns = std::vector<EnemySpawnData>
		{
			EnemySpawnData{ 0, Entity::Vector2Int{ 7, 5 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 11, 5 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 7, 7 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 11, 7 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 7, 14 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 11, 14 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 7, 16 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 11, 16 }, 0 }
		};

		break;

	case 19:

		returnData.gridTiles = std::vector<int>
		{
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 4, 11, 11, 11, 11, 11, 11, 11, 11, 11, 10, 0, 0, 0, 0,
			0, 0, 0, 0, 6, 17, 17, 17, 17, 17, 17, 17, 17, 17, 6, 0, 0, 0, 0,
			0, 0, 0, 0, 6, 17, 2, 17, 4, 11, 10, 17, 2, 17, 6, 0, 0, 0, 0,
			0, 4, 11, 11, 13, 17, 5, 17, 6, 0, 6, 17, 5, 17, 7, 11, 11, 10, 0,
			0, 6, 18, 17, 17, 17, 0, 17, 7, 11, 13, 17, 0, 17, 17, 17, 18, 6, 0,
			0, 6, 17, 4, 10, 17, 2, 17, 17, 17, 17, 17, 2, 17, 4, 10, 17, 6, 0,
			0, 6, 17, 6, 6, 17, 6, 17, 3, 11, 9, 17, 6, 17, 6, 6, 17, 6, 0,
			11, 13, 17, 7, 13, 17, 5, 17, 17, 17, 17, 17, 5, 17, 7, 13, 17, 7, 11,
			0, 0, 17, 17, 17, 17, 0, 17, 4, 11, 10, 17, 0, 17, 17, 17, 17, 0, 0,
			11, 10, 17, 4, 10, 17, 2, 17, 6, 0, 6, 17, 2, 17, 4, 10, 17, 4, 11,
			11, 13, 17, 7, 13, 17, 5, 17, 6, 0, 6, 17, 5, 17, 7, 13, 17, 7, 11,
			0, 0, 17, 17, 17, 17, 0, 17, 7, 11, 13, 17, 0, 17, 17, 17, 17, 0, 0,
			11, 10, 17, 4, 10, 17, 2, 17, 0, 0, 0, 17, 2, 17, 4, 10, 17, 4, 11,
			0, 6, 17, 6, 6, 17, 6, 17, 3, 11, 9, 17, 6, 17, 6, 6, 17, 6, 0,
			0, 6, 17, 7, 13, 17, 5, 17, 17, 17, 17, 17, 5, 17, 7, 13, 17, 6, 0,
			0, 6, 18, 17, 17, 17, 0, 17, 4, 11, 10, 17, 0, 17, 17, 17, 18, 6, 0,
			0, 7, 11, 11, 10, 17, 2, 17, 6, 0, 6, 17, 2, 17, 4, 11, 11, 13, 0,
			0, 0, 0, 0, 6, 17, 5, 17, 7, 11, 13, 17, 5, 17, 6, 0, 0, 0, 0,
			0, 0, 0, 0, 6, 17, 17, 17, 17, 17, 17, 17, 17, 17, 6, 0, 0, 0, 0,
			0, 0, 0, 0, 7, 11, 11, 11, 11, 11, 11, 11, 11, 11, 13, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		};

		returnData.playerSpawn = PlayerSpawnData{ Entity::Vector2Int{ 9, 13 }, 3 };

		returnData.enemySpawns = std::vector<EnemySpawnData>
		{
			EnemySpawnData{ 0, Entity::Vector2Int{ 6, 5 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 12, 5 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 6, 9 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 12, 9 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 6, 12 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 12, 12 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 6, 16 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 12, 16 }, 0 }
		};

		break;

	case 20:

		returnData.gridTiles = std::vector<int>
		{
			0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 4, 11, 11, 11, 11, 11, 13, 0, 7, 11, 11, 11, 11, 11, 10, 0, 0,
			0, 0, 6, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 6, 0, 0,
			0, 0, 6, 17, 3, 11, 9, 17, 3, 11, 9, 17, 3, 11, 9, 17, 6, 0, 0,
			11, 11, 13, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 7, 11, 11,
			0, 0, 0, 17, 3, 9, 0, 3, 11, 11, 11, 9, 0, 3, 9, 17, 0, 0, 0,
			11, 11, 10, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 4, 11, 11,
			0, 0, 6, 17, 3, 11, 9, 17, 3, 11, 9, 17, 3, 11, 9, 17, 6, 0, 0,
			0, 0, 6, 17, 17, 17, 17, 17, 0, 0, 0, 17, 17, 17, 17, 17, 6, 0, 0,
			0, 0, 7, 11, 11, 11, 11, 11, 10, 0, 4, 11, 11, 11, 11, 11, 13, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 4, 11, 11, 11, 11, 11, 13, 0, 7, 11, 11, 11, 11, 11, 10, 0, 0,
			0, 0, 6, 17, 0, 17, 17, 17, 17, 17, 17, 17, 17, 17, 0, 17, 6, 0, 0,
			0, 0, 6, 17, 2, 17, 3, 9, 17, 1, 17, 3, 9, 17, 2, 17, 6, 0, 0,
			11, 11, 13, 17, 6, 17, 17, 17, 17, 17, 17, 17, 17, 17, 6, 17, 7, 11, 11,
			0, 0, 0, 17, 6, 17, 3, 11, 9, 0, 3, 11, 9, 17, 6, 17, 0, 0, 0,
			11, 11, 10, 17, 6, 17, 17, 17, 17, 17, 17, 17, 17, 17, 6, 17, 4, 11, 11,
			0, 0, 6, 17, 5, 17, 3, 9, 17, 1, 17, 3, 9, 17, 5, 17, 6, 0, 0,
			0, 0, 6, 17, 0, 17, 17, 17, 17, 17, 17, 17, 17, 17, 0, 17, 6, 0, 0,
			0, 0, 7, 11, 11, 11, 11, 11, 10, 0, 4, 11, 11, 11, 11, 11, 13, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0
		};

		returnData.playerSpawn = PlayerSpawnData{ Entity::Vector2Int{ 9, 8 }, 3 };

		returnData.enemySpawns = std::vector<EnemySpawnData>
		{
			EnemySpawnData{ 0, Entity::Vector2Int{ 6, 5 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 12, 5 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 4, 13 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 14, 13 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 9, 16 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 4, 19 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 14, 19 }, 0 },
		};

		break;

	case 21:

		returnData.gridTiles = std::vector<int>
		{
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 4, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 10, 0,
			0, 6, 17, 17, 18, 17, 17, 17, 17, 17, 17, 17, 17, 17, 18, 17, 17, 6, 0,
			0, 6, 17, 4, 11, 10, 17, 4, 10, 17, 4, 10, 17, 4, 11, 10, 17, 6, 0,
			0, 6, 17, 6, 0, 6, 0, 8, 14, 0, 8, 14, 0, 6, 0, 6, 17, 6, 0,
			0, 6, 17, 7, 11, 13, 17, 7, 13, 17, 7, 13, 17, 7, 11, 13, 17, 6, 0,
			0, 6, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 6, 0,
			0, 6, 0, 3, 11, 11, 9, 0, 2, 0, 2, 0, 3, 11, 11, 9, 0, 6, 0,
			0, 6, 17, 17, 17, 17, 17, 17, 6, 0, 6, 17, 17, 17, 17, 17, 17, 6, 0,
			0, 6, 17, 2, 17, 4, 10, 17, 6, 0, 6, 17, 4, 10, 17, 2, 17, 6, 0,
			0, 6, 17, 5, 17, 7, 13, 17, 6, 0, 6, 17, 7, 13, 17, 5, 17, 6, 0,
			0, 6, 17, 17, 17, 17, 17, 17, 6, 0, 6, 17, 17, 17, 17, 17, 17, 6, 0,
			0, 6, 0, 3, 11, 11, 9, 0, 5, 0, 5, 0, 3, 11, 11, 9, 0, 6, 0,
			0, 6, 17, 17, 17, 17, 17, 0, 0, 0, 0, 0, 17, 17, 17, 17, 17, 6, 0,
			0, 6, 17, 4, 11, 10, 17, 4, 10, 0, 4, 10, 17, 4, 11, 10, 17, 6, 0,
			0, 6, 17, 6, 0, 6, 0, 8, 14, 0, 8, 14, 0, 6, 0, 6, 17, 6, 0,
			0, 6, 17, 7, 11, 13, 17, 7, 13, 0, 7, 13, 17, 7, 11, 13, 17, 6, 0,
			0, 6, 17, 17, 18, 17, 17, 17, 17, 17, 17, 17, 17, 17, 18, 17, 17, 6, 0,
			0, 7, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 13, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		};

		returnData.playerSpawn = PlayerSpawnData{ Entity::Vector2Int{ 9, 16 }, 2 };

		returnData.enemySpawns = std::vector<EnemySpawnData>
		{
			EnemySpawnData{ 0, Entity::Vector2Int{ 6, 5 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 9, 5 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 12, 5 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 2, 8 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 7, 8 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 11, 8 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 16, 8 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 2, 13 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 7, 13 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 11, 13 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 16, 13 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 6, 16 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 12, 16 }, 0 }
		};

		break;

	case 22:

		returnData.gridTiles = std::vector<int>
		{
			0, 4, 11, 11, 11, 11, 11, 11, 11, 12, 11, 12, 11, 12, 11, 11, 11, 10, 0,
			0, 6, 17, 17, 17, 17, 18, 17, 17, 6, 0, 5, 0, 6, 0, 0, 0, 6, 0,
			0, 6, 17, 2, 0, 4, 11, 10, 17, 5, 0, 0, 0, 5, 0, 2, 0, 6, 0,
			0, 6, 17, 6, 0, 6, 0, 6, 17, 17, 17, 2, 0, 0, 0, 6, 0, 6, 0,
			0, 6, 17, 5, 0, 7, 11, 13, 0, 2, 17, 5, 0, 2, 0, 5, 0, 6, 0,
			0, 6, 17, 0, 0, 0, 0, 0, 0, 6, 17, 17, 17, 6, 0, 0, 0, 6, 0,
			0, 6, 17, 2, 0, 4, 11, 10, 0, 5, 0, 2, 17, 5, 0, 2, 0, 6, 0,
			0, 6, 17, 6, 0, 6, 0, 6, 0, 0, 0, 6, 17, 17, 17, 6, 0, 6, 0,
			0, 6, 17, 5, 0, 7, 11, 13, 0, 2, 0, 5, 0, 2, 17, 5, 0, 6, 0,
			0, 6, 17, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 6, 18, 17, 17, 6, 0,
			0, 6, 17, 2, 0, 4, 11, 10, 0, 6, 0, 2, 0, 7, 11, 9, 17, 6, 0,
			0, 6, 17, 5, 0, 7, 11, 13, 0, 6, 0, 5, 0, 4, 11, 9, 17, 6, 0,
			0, 6, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 6, 18, 17, 17, 6, 0,
			0, 6, 0, 2, 0, 4, 11, 10, 0, 5, 0, 2, 0, 5, 17, 2, 0, 6, 0,
			0, 6, 0, 6, 0, 6, 0, 6, 0, 0, 0, 6, 17, 17, 17, 6, 0, 6, 0,
			0, 6, 0, 5, 0, 7, 11, 13, 0, 2, 0, 5, 17, 2, 0, 5, 0, 6, 0,
			0, 6, 0, 0, 0, 0, 0, 0, 0, 6, 17, 17, 17, 6, 0, 0, 0, 6, 0,
			0, 6, 17, 2, 0, 4, 11, 10, 0, 5, 17, 2, 0, 5, 0, 2, 0, 6, 0,
			0, 6, 17, 6, 0, 6, 0, 6, 17, 17, 17, 5, 0, 0, 0, 6, 0, 6, 0,
			0, 6, 17, 5, 0, 7, 11, 13, 17, 2, 0, 0, 0, 2, 0, 5, 0, 6, 0,
			0, 6, 17, 17, 17, 17, 18, 17, 17, 6, 0, 2, 0, 6, 0, 0, 0, 6, 0,
			0, 7, 11, 11, 11, 11, 11, 11, 11, 15, 11, 15, 11, 15, 11, 11, 11, 13, 0
		};

		returnData.playerSpawn = PlayerSpawnData{ Entity::Vector2Int{ 2, 14 }, 0 };

		returnData.enemySpawns = std::vector<EnemySpawnData>
		{
			EnemySpawnData{ 0, Entity::Vector2Int{ 4, 2 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 10, 2 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 8, 4 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 12, 4 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 3, 5 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 10, 6 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 14, 6 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 12, 8 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 16, 8 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 3, 9 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 3, 12 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 12, 13 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 16, 13 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 10, 15 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 14, 15 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 3, 16 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 8, 17 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 12, 17 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 4, 19 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 10, 19 }, 0 }
		};

		break;

	case 23:

		returnData.gridTiles = std::vector<int>
		{
			0, 0, 0, 0, 0, 6, 0, 6, 0, 0, 0, 6, 0, 6, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 6, 0, 6, 0, 0, 0, 6, 0, 6, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 6, 0, 6, 0, 0, 0, 6, 0, 6, 0, 0, 0, 0, 0,
			0, 4, 11, 11, 11, 14, 0, 7, 11, 11, 11, 13, 0, 8, 11, 11, 11, 10, 0,
			0, 6, 17, 18, 17, 6, 17, 17, 17, 17, 17, 17, 17, 6, 17, 18, 17, 6, 0,
			0, 6, 17, 2, 17, 5, 17, 2, 17, 2, 17, 2, 17, 5, 17, 2, 17, 6, 0,
			11, 13, 17, 5, 17, 0, 17, 5, 17, 6, 17, 5, 17, 0, 17, 5, 17, 7, 11,
			0, 0, 17, 0, 17, 1, 17, 0, 17, 5, 17, 0, 17, 1, 17, 0, 17, 0, 0,
			11, 10, 17, 1, 17, 0, 17, 1, 17, 0, 17, 1, 17, 0, 17, 1, 17, 4, 11,
			0, 6, 17, 0, 17, 2, 17, 0, 17, 2, 17, 0, 17, 2, 17, 0, 17, 6, 0,
			0, 6, 17, 2, 17, 6, 17, 2, 17, 6, 17, 2, 17, 6, 17, 2, 17, 6, 0,
			0, 6, 17, 5, 17, 6, 17, 5, 17, 6, 17, 5, 17, 6, 17, 5, 17, 6, 0,
			0, 6, 17, 0, 17, 5, 17, 0, 17, 5, 17, 17, 17, 5, 17, 0, 17, 6, 0,
			11, 13, 17, 1, 17, 17, 17, 1, 17, 0, 17, 1, 17, 0, 17, 1, 17, 7, 11,
			0, 0, 17, 0, 17, 1, 17, 0, 17, 2, 17, 17, 17, 1, 17, 0, 17, 0, 0,
			11, 10, 17, 2, 17, 17, 17, 2, 17, 6, 17, 2, 17, 0, 17, 2, 17, 4, 11,
			0, 6, 17, 5, 17, 2, 17, 5, 17, 5, 17, 5, 17, 2, 17, 5, 17, 6, 0,
			0, 6, 17, 18, 17, 6, 17, 0, 0, 0, 0, 0, 17, 6, 17, 18, 17, 6, 0,
			0, 7, 11, 11, 11, 14, 0, 4, 11, 11, 11, 10, 0, 8, 11, 11, 11, 13, 0,
			0, 0, 0, 0, 0, 6, 0, 6, 0, 0, 0, 6, 0, 6, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 6, 0, 6, 0, 0, 0, 6, 0, 6, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 6, 0, 6, 0, 0, 0, 6, 0, 6, 0, 0, 0, 0, 0
		};

		returnData.playerSpawn = PlayerSpawnData{ Entity::Vector2Int{ 9, 17 }, 3 };

		returnData.enemySpawns = std::vector<EnemySpawnData>
		{
			EnemySpawnData{ 0, Entity::Vector2Int{ 5, 6 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 13, 6 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 3, 7 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 7, 7 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 11, 7 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 15, 7 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 5, 8 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 9, 8 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 13, 8 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 3, 9 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 7, 9 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 11, 9 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 15, 9 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 3, 12 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 7, 12 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 15, 12 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 3, 12 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 7, 12 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 15, 12 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 9, 13 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 13, 13 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 3, 14 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 7, 14 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 15, 14 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 13, 15 }, 0 }
		};

		break;

	case 24:

		returnData.gridTiles = std::vector<int>
		{
			0, 6, 0, 6, 0, 4, 11, 11, 11, 11, 11, 11, 11, 10, 0, 6, 0, 6, 0,
			0, 6, 0, 6, 0, 6, 17, 17, 17, 17, 17, 17, 17, 6, 0, 6, 0, 6, 0,
			0, 6, 0, 6, 0, 6, 17, 4, 11, 11, 11, 10, 17, 6, 0, 6, 0, 6, 0,
			11, 13, 0, 7, 11, 13, 17, 5, 17, 17, 17, 5, 17, 7, 11, 13, 0, 7, 11,
			0, 0, 0, 0, 18, 0, 17, 0, 17, 1, 17, 0, 17, 0, 18, 0, 0, 0, 0,
			11, 10, 0, 4, 11, 10, 17, 2, 17, 17, 17, 2, 17, 4, 11, 10, 0, 4, 11,
			0, 6, 0, 6, 0, 6, 17, 7, 11, 11, 11, 13, 17, 6, 0, 6, 0, 6, 0,
			0, 6, 0, 6, 0, 6, 17, 17, 17, 17, 17, 17, 17, 6, 0, 6, 0, 6, 0,
			0, 6, 0, 6, 0, 7, 11, 11, 10, 0, 4, 11, 11, 13, 0, 6, 0, 6, 0,
			0, 6, 0, 6, 0, 0, 0, 0, 6, 0, 6, 0, 0, 0, 0, 6, 0, 6, 0,
			0, 6, 0, 6, 0, 4, 10, 0, 6, 0, 6, 0, 4, 10, 0, 6, 0, 6, 0,
			0, 6, 0, 6, 0, 7, 13, 0, 6, 0, 6, 0, 7, 13, 0, 6, 0, 6, 0,
			0, 6, 0, 6, 0, 0, 0, 0, 6, 0, 6, 0, 0, 0, 0, 6, 0, 6, 0,
			0, 6, 0, 6, 0, 4, 11, 11, 13, 0, 7, 11, 11, 10, 0, 6, 0, 6, 0,
			0, 6, 0, 6, 0, 6, 17, 17, 0, 0, 0, 17, 17, 6, 0, 6, 0, 6, 0,
			0, 6, 0, 6, 0, 6, 17, 4, 11, 11, 11, 10, 17, 6, 0, 6, 0, 6, 0,
			11, 13, 0, 7, 11, 13, 17, 5, 17, 17, 17, 5, 17, 7, 11, 13, 0, 7, 11,
			0, 0, 0, 0, 18, 0, 17, 0, 17, 1, 17, 0, 17, 0, 18, 0, 0, 0, 0,
			11, 10, 0, 4, 11, 10, 17, 2, 17, 17, 17, 2, 17, 4, 11, 10, 0, 4, 11,
			0, 6, 0, 6, 0, 6, 17, 7, 11, 11, 11, 13, 17, 6, 0, 6, 0, 6, 0,
			0, 6, 0, 6, 0, 6, 17, 17, 17, 17, 17, 17, 17, 6, 0, 6, 0, 6, 0,
			0, 6, 0, 6, 0, 7, 11, 11, 11, 11, 11, 11, 11, 13, 0, 6, 0, 6, 0
		};

		returnData.playerSpawn = PlayerSpawnData{ Entity::Vector2Int{ 9, 14 }, 3 };

		returnData.enemySpawns = std::vector<EnemySpawnData>
		{
			EnemySpawnData{ 0, Entity::Vector2Int{ 2, 3 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 16, 3 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 1, 4 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 3, 4 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 5, 4 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 7, 4 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 11, 4 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 13, 4 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 15, 4 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 17, 4 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 2, 5 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 16, 5 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 2, 16 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 16, 16 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 1, 17 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 3, 17 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 5, 17 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 7, 17 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 11, 17 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 13, 17 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 15, 17 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 17, 17 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 2, 18 }, 0 },
			EnemySpawnData{ 0, Entity::Vector2Int{ 16, 18 }, 0 }
		};

		break;

	case 25:

		returnData.gridTiles = std::vector<int>
		{
			4, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 10,
			6, 18, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 18, 6,
			6, 17, 4, 10, 17, 4, 10, 17, 4, 11, 10, 17, 4, 10, 17, 4, 10, 17, 6,
			6, 17, 7, 13, 17, 7, 13, 17, 7, 11, 13, 17, 7, 13, 17, 7, 13, 17, 6,
			6, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 6,
			6, 17, 4, 10, 17, 2, 17, 3, 11, 12, 11, 9, 17, 2, 17, 4, 10, 17, 6,
			6, 17, 6, 6, 17, 6, 17, 17, 17, 6, 17, 17, 17, 6, 17, 6, 6, 17, 6,
			6, 17, 7, 13, 17, 7, 11, 9, 17, 5, 17, 3, 11, 13, 17, 7, 13, 17, 6,
			6, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 6,
			6, 17, 4, 11, 11, 10, 17, 4, 11, 11, 11, 10, 17, 4, 11, 11, 10, 17, 6,
			6, 17, 6, 0, 0, 6, 17, 6, 0, 0, 0, 6, 17, 6, 0, 0, 6, 17, 6,
			6, 17, 6, 0, 0, 6, 17, 6, 0, 0, 0, 6, 17, 6, 0, 0, 6, 17, 6,
			6, 17, 7, 11, 11, 13, 17, 7, 11, 11, 11, 13, 17, 7, 11, 11, 13, 17, 6,
			6, 17, 17, 17, 17, 17, 17, 0, 0, 0, 0, 0, 17, 17, 17, 17, 17, 17, 6,
			6, 17, 3, 11, 12, 11, 9, 0, 3, 11, 9, 0, 3, 11, 12, 11, 9, 17, 6,
			6, 17, 17, 17, 6, 17, 17, 17, 17, 17, 17, 17, 17, 17, 6, 17, 17, 17, 6,
			8, 11, 9, 17, 5, 17, 3, 11, 9, 17, 3, 11, 9, 17, 5, 17, 3, 11, 14,
			6, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 6,
			6, 17, 4, 10, 17, 4, 10, 17, 4, 11, 10, 17, 4, 10, 17, 4, 10, 17, 6,
			6, 17, 7, 13, 17, 7, 13, 17, 7, 11, 13, 17, 7, 13, 17, 7, 13, 17, 6,
			6, 18, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 18, 6,
			7, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 13
		};

		returnData.playerSpawn = PlayerSpawnData{ Entity::Vector2Int{ 9, 13 }, 3 };

		returnData.enemySpawns = std::vector<EnemySpawnData>
		{
			EnemySpawnData{ 1, Entity::Vector2Int{ 9, 1 }, 1 },
			EnemySpawnData{ 1, Entity::Vector2Int{ 9, 4 }, 3 },
			EnemySpawnData{ 2, Entity::Vector2Int{ 1, 6 }, 2 },
			EnemySpawnData{ 2, Entity::Vector2Int{ 4, 6 }, 0 },
			EnemySpawnData{ 2, Entity::Vector2Int{ 14, 6 }, 2 },
			EnemySpawnData{ 2, Entity::Vector2Int{ 17, 6 }, 0 },
			EnemySpawnData{ 3, Entity::Vector2Int{ 9, 8 }, 3 },
			EnemySpawnData{ 1, Entity::Vector2Int{ 7, 15 }, 1 },
			EnemySpawnData{ 1, Entity::Vector2Int{ 11, 15 }, 1 },
			EnemySpawnData{ 1, Entity::Vector2Int{ 7, 17 }, 3 },
			EnemySpawnData{ 2, Entity::Vector2Int{ 9, 17 }, 3 },
			EnemySpawnData{ 1, Entity::Vector2Int{ 11, 17 }, 3 },
			EnemySpawnData{ 2, Entity::Vector2Int{ 9, 20 }, 1 },
			EnemySpawnData{ 2, Entity::Vector2Int{ 9, 20 }, 1 }
		};

		break;

	case 26:

		returnData.gridTiles = std::vector<int>
		{
			4, 11, 9, 0, 3, 11, 11, 11, 9, 0, 3, 11, 11, 11, 9, 0, 3, 11, 10,
			6, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 6,
			6, 17, 4, 11, 10, 17, 2, 17, 4, 11, 10, 17, 2, 17, 4, 11, 10, 17, 6,
			6, 17, 7, 11, 13, 17, 6, 17, 7, 11, 13, 17, 6, 17, 7, 11, 13, 17, 6,
			6, 17, 17, 17, 17, 17, 6, 17, 17, 17, 17, 17, 6, 17, 17, 17, 17, 17, 6,
			8, 11, 9, 17, 3, 11, 15, 11, 9, 17, 3, 11, 15, 11, 9, 17, 3, 11, 14,
			5, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 5,
			0, 17, 3, 11, 11, 9, 17, 3, 11, 11, 11, 9, 17, 3, 11, 11, 9, 17, 0,
			2, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 2,
			6, 17, 4, 11, 11, 10, 17, 4, 11, 11, 11, 10, 17, 4, 11, 11, 10, 17, 6,
			6, 17, 6, 0, 0, 6, 17, 6, 0, 0, 0, 6, 17, 6, 0, 0, 6, 17, 6,
			6, 17, 6, 0, 0, 6, 17, 6, 0, 0, 0, 6, 17, 6, 0, 0, 6, 17, 6,
			6, 17, 7, 11, 11, 13, 17, 7, 11, 11, 11, 13, 17, 7, 11, 11, 13, 17, 6,
			5, 17, 17, 17, 17, 17, 17, 0, 0, 0, 0, 0, 17, 17, 17, 17, 17, 17, 5,
			0, 17, 3, 11, 11, 9, 17, 3, 11, 11, 11, 9, 17, 3, 11, 11, 9, 17, 0,
			2, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 2,
			8, 11, 9, 17, 3, 11, 12, 11, 9, 17, 3, 11, 12, 11, 9, 17, 3, 11, 14,
			6, 17, 17, 17, 17, 17, 6, 17, 17, 17, 17, 17, 6, 17, 17, 17, 17, 17, 6,
			6, 17, 4, 11, 10, 17, 6, 17, 4, 11, 10, 17, 6, 17, 4, 11, 10, 17, 6,
			6, 17, 7, 11, 13, 17, 5, 17, 7, 11, 13, 17, 5, 17, 7, 11, 13, 17, 6,
			6, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 6,
			7, 11, 9, 0, 3, 11, 11, 11, 9, 0, 3, 11, 11, 11, 9, 0, 3, 11, 13
		};

		returnData.playerSpawn = PlayerSpawnData{ Entity::Vector2Int{ 9, 13 }, 3 };

		returnData.enemySpawns = std::vector<EnemySpawnData>
		{
			EnemySpawnData{ 1, Entity::Vector2Int{ 3, 1 }, 1 },
			EnemySpawnData{ 1, Entity::Vector2Int{ 9, 1 }, 1 },
			EnemySpawnData{ 1, Entity::Vector2Int{ 15, 1 }, 1 },
			EnemySpawnData{ 1, Entity::Vector2Int{ 3, 4 }, 3 },
			EnemySpawnData{ 1, Entity::Vector2Int{ 9, 4 }, 3 },
			EnemySpawnData{ 1, Entity::Vector2Int{ 15, 4 }, 3 },
			EnemySpawnData{ 3, Entity::Vector2Int{ 9, 6 }, 3 },
			EnemySpawnData{ 4, Entity::Vector2Int{ 9, 8 }, 1 },
			EnemySpawnData{ 2, Entity::Vector2Int{ 3, 17 }, 3 },
			EnemySpawnData{ 2, Entity::Vector2Int{ 9, 17 }, 3 },
			EnemySpawnData{ 2, Entity::Vector2Int{ 15, 17 }, 3 },
			EnemySpawnData{ 2, Entity::Vector2Int{ 3, 20 }, 1 },
			EnemySpawnData{ 2, Entity::Vector2Int{ 9, 20 }, 1 },
			EnemySpawnData{ 2, Entity::Vector2Int{ 15, 20 }, 1 }
		};

		break;

	case 27:

		returnData.gridTiles = std::vector<int>
		{
			0, 0, 0, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 0, 0, 0,
			0, 0, 0, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 0, 0, 0,
			0, 0, 4, 11, 13, 0, 7, 11, 13, 0, 7, 11, 13, 0, 7, 11, 10, 0, 0,
			0, 0, 6, 18, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 18, 6, 0, 0,
			0, 0, 6, 17, 4, 11, 10, 17, 4, 11, 10, 17, 4, 11, 10, 17, 6, 0, 0,
			0, 0, 6, 17, 7, 11, 13, 17, 7, 11, 13, 17, 7, 11, 13, 17, 6, 0, 0,
			0, 0, 6, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 6, 0, 0,
			11, 11, 13, 17, 3, 11, 9, 17, 3, 11, 9, 17, 3, 11, 9, 17, 7, 11, 11,
			0, 0, 0, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 0, 0, 0,
			11, 11, 10, 17, 2, 17, 4, 11, 10, 17, 4, 11, 10, 17, 2, 17, 4, 11, 11,
			0, 0, 6, 17, 6, 17, 6, 0, 6, 17, 6, 0, 6, 17, 6, 17, 6, 0, 0,
			0, 0, 6, 17, 6, 17, 6, 0, 6, 17, 6, 0, 6, 17, 6, 17, 6, 0, 0,
			11, 11, 13, 17, 5, 17, 7, 11, 13, 17, 7, 11, 13, 17, 5, 17, 7, 11, 11,
			0, 0, 0, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 0, 0, 0,
			11, 11, 10, 17, 3, 11, 9, 17, 3, 11, 9, 17, 3, 11, 9, 17, 4, 11, 11,
			0, 0, 6, 17, 17, 17, 17, 17, 0, 0, 0, 17, 17, 17, 17, 17, 6, 0, 0,
			0, 0, 6, 17, 4, 11, 10, 17, 4, 11, 10, 17, 4, 11, 10, 17, 6, 0, 0,
			0, 0, 6, 17, 7, 11, 13, 17, 7, 11, 13, 17, 7, 11, 13, 17, 6, 0, 0,
			0, 0, 6, 18, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 18, 6, 0, 0,
			0, 0, 7, 11, 10, 0, 4, 11, 10, 0, 4, 11, 10, 0, 4, 11, 13, 0, 0,
			0, 0, 0, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 0, 0, 0,
			0, 0, 0, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 0, 0, 0
		};

		returnData.playerSpawn = PlayerSpawnData{ Entity::Vector2Int{ 9, 15 }, 3 };

		returnData.enemySpawns = std::vector<EnemySpawnData>
		{
			EnemySpawnData{ 4, Entity::Vector2Int{ 9, 8 }, 1 },
			EnemySpawnData{ 5, Entity::Vector2Int{ 9, 13 }, 3 }
		};

		break;

	case 28:

		returnData.gridTiles = std::vector<int>
		{
			11, 11, 11, 11, 11, 11, 13, 0, 7, 11, 13, 0, 7, 11, 11, 11, 11, 11, 11,
			0, 0, 0, 0, 0, 17, 17, 17, 17, 17, 17, 17, 17, 17, 0, 0, 0, 0, 0,
			4, 11, 11, 11, 10, 17, 2, 17, 3, 11, 9, 17, 2, 17, 4, 11, 11, 11, 10,
			6, 18, 17, 17, 6, 17, 6, 17, 17, 17, 17, 17, 6, 17, 6, 17, 17, 18, 6,
			6, 17, 2, 17, 5, 17, 8, 11, 9, 18, 3, 11, 14, 17, 5, 17, 2, 17, 6,
			6, 17, 6, 17, 17, 17, 6, 17, 17, 17, 17, 17, 6, 17, 17, 17, 6, 17, 6,
			6, 17, 7, 11, 9, 17, 5, 17, 3, 11, 9, 17, 5, 17, 3, 11, 13, 17, 6,
			6, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 6,
			6, 17, 3, 11, 9, 17, 1, 17, 3, 12, 9, 17, 1, 17, 3, 11, 9, 17, 6,
			6, 17, 17, 17, 17, 17, 17, 17, 17, 6, 17, 17, 17, 17, 17, 17, 17, 17, 6,
			8, 11, 9, 17, 3, 11, 11, 9, 17, 5, 17, 3, 11, 11, 9, 17, 3, 11, 14,
			13, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 7,
			0, 17, 3, 11, 9, 17, 3, 11, 9, 0, 3, 11, 9, 17, 3, 11, 9, 17, 0,
			10, 17, 17, 17, 17, 17, 0, 0, 0, 0, 0, 0, 0, 17, 17, 17, 17, 17, 4,
			8, 11, 9, 17, 3, 11, 12, 11, 9, 0, 3, 11, 12, 11, 9, 0, 3, 11, 14,
			6, 17, 17, 17, 17, 17, 6, 17, 17, 17, 17, 17, 6, 17, 17, 17, 17, 17, 6,
			6, 17, 4, 11, 10, 17, 6, 17, 4, 11, 10, 17, 6, 17, 4, 11, 10, 17, 6,
			6, 17, 7, 11, 13, 17, 5, 17, 7, 11, 13, 17, 5, 17, 7, 11, 13, 17, 6,
			6, 18, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 18, 6,
			8, 11, 11, 10, 17, 3, 9, 17, 3, 11, 9, 17, 3, 9, 17, 4, 11, 11, 14,
			6, 0, 0, 6, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 6, 0, 0, 6,
			7, 11, 11, 15, 11, 11, 10, 0, 4, 11, 10, 0, 4, 11, 11, 15, 11, 11, 13
		};

		returnData.playerSpawn = PlayerSpawnData{ Entity::Vector2Int{ 9, 13 }, 3 };

		returnData.enemySpawns = std::vector<EnemySpawnData>
		{
			EnemySpawnData{ 1, Entity::Vector2Int{ 9, 1 }, 1 },
			EnemySpawnData{ 1, Entity::Vector2Int{ 9, 3 }, 3 },
			EnemySpawnData{ 7, Entity::Vector2Int{ 1, 3 }, 2 },
			EnemySpawnData{ 3, Entity::Vector2Int{ 9, 5 }, 3 },
			EnemySpawnData{ 6, Entity::Vector2Int{ 17, 3 }, 2 },
			EnemySpawnData{ 2, Entity::Vector2Int{ 9, 18 }, 3 },
			EnemySpawnData{ 2, Entity::Vector2Int{ 9, 20 }, 1 }
		};

		break;

	case 29:

		returnData.gridTiles = std::vector<int>
		{
			4, 11, 11, 11, 11, 9, 0, 3, 9, 0, 3, 9, 0, 3, 11, 11, 11, 11, 10,
			6, 18, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 18, 6,
			6, 17, 3, 11, 11, 9, 21, 3, 11, 11, 11, 9, 21, 3, 11, 11, 9, 17, 6,
			6, 17, 20, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 22, 17, 6,
			5, 17, 3, 11, 11, 9, 17, 1, 17, 2, 17, 1, 17, 3, 11, 11, 9, 17, 5,
			0, 17, 17, 17, 17, 17, 17, 17, 17, 6, 17, 17, 17, 17, 17, 17, 17, 17, 0,
			2, 17, 3, 11, 11, 9, 17, 3, 11, 16, 11, 9, 17, 3, 11, 11, 9, 17, 2,
			6, 17, 17, 17, 17, 17, 17, 17, 17, 6, 17, 17, 17, 17, 17, 17, 17, 17, 6,
			5, 17, 3, 11, 11, 9, 17, 2, 17, 5, 17, 2, 17, 3, 11, 11, 9, 17, 5,
			0, 17, 17, 17, 17, 17, 17, 6, 17, 17, 17, 6, 17, 17, 17, 17, 17, 17, 0,
			2, 17, 4, 11, 11, 10, 17, 6, 17, 2, 17, 6, 17, 4, 11, 11, 10, 17, 2,
			5, 17, 7, 11, 11, 13, 17, 6, 17, 5, 17, 6, 17, 7, 11, 11, 13, 17, 5,
			0, 17, 17, 17, 17, 17, 17, 6, 17, 17, 17, 6, 17, 17, 17, 17, 17, 17, 0,
			2, 17, 3, 11, 11, 9, 17, 5, 17, 2, 17, 5, 17, 3, 11, 11, 9, 17, 2,
			6, 17, 17, 17, 17, 17, 17, 17, 17, 6, 17, 17, 17, 17, 17, 17, 17, 17, 6,
			5, 17, 3, 11, 11, 9, 17, 3, 11, 16, 11, 9, 17, 3, 11, 11, 9, 17, 5,
			0, 17, 17, 17, 17, 17, 17, 0, 0, 6, 0, 0, 17, 17, 17, 17, 17, 17, 0,
			2, 17, 3, 11, 11, 9, 17, 1, 0, 5, 0, 1, 17, 3, 11, 11, 9, 17, 2,
			6, 17, 20, 17, 17, 17, 17, 0, 0, 0, 0, 0, 17, 17, 17, 17, 22, 17, 6,
			6, 17, 3, 11, 11, 9, 19, 3, 11, 11, 11, 9, 19, 3, 11, 11, 9, 17, 6,
			6, 18, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 18, 6,
			7, 11, 11, 11, 11, 9, 0, 3, 9, 0, 3, 9, 0, 3, 11, 11, 11, 11, 13
		};

		returnData.playerSpawn = PlayerSpawnData{ Entity::Vector2Int{ 9, 18 }, 3 };

		returnData.enemySpawns = std::vector<EnemySpawnData>
		{
			EnemySpawnData{ 5, Entity::Vector2Int{ 9, 1 }, 1 },
			EnemySpawnData{ 3, Entity::Vector2Int{ 9, 3 }, 3 },
			EnemySpawnData{ 7, Entity::Vector2Int{ 7, 5 }, 3 },
			EnemySpawnData{ 6, Entity::Vector2Int{ 11, 5 }, 1 },
			EnemySpawnData{ 4, Entity::Vector2Int{ 9, 9 }, 1 }
		};

		break;

	default:

		returnData.gridTiles = std::vector<int>
		{
			7, 11, 11, 11, 11, 11, 11, 11, 11, 15, 11, 11, 11, 11, 11, 11, 11, 11, 13,
			6, 17, 17, 17, 17, 17, 17, 17, 17, 6, 17, 17, 17, 17, 17, 17, 17, 17, 6,
			6, 18, 7, 13, 17, 7, 11, 13, 17, 6, 17, 7, 11, 13, 17, 7, 13, 18, 6,
			6, 17, 4, 10, 17, 4, 11, 10, 17, 5, 17, 4, 11, 10, 17, 4, 10, 17, 6,
			6, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 6,
			6, 17, 9, 3, 17, 2, 17, 9, 11, 15, 11, 3, 17, 2, 17, 9, 3, 17, 6,
			6, 17, 17, 17, 17, 6, 17, 17, 17, 6, 17, 17, 17, 6, 17, 17, 17, 17, 6,
			4, 11, 11, 13, 17, 8, 11, 3, 0, 5, 0, 9, 11, 14, 17, 7, 11, 11, 10,
			0, 0, 0, 6, 17, 6, 0, 0, 0, 0, 0, 0, 0, 6, 17, 6, 0, 0, 0,
			11, 11, 11, 10, 17, 5, 0, 7, 3, 0, 9, 13, 0, 5, 17, 4, 11, 11, 11,
			0, 0, 0, 0, 17, 0, 0, 6, 0, 0, 0, 6, 0, 0, 17, 0, 0, 0, 0,
			11, 11, 11, 13, 17, 2, 0, 4, 11, 11, 11, 10, 0, 2, 17, 7, 11, 11, 11,
			0, 0, 0, 6, 17, 6, 0, 0, 0, 0, 0, 0, 0, 6, 17, 6, 0, 0, 0,
			7, 11, 11, 10, 17, 5, 0, 9, 11, 15, 11, 3, 0, 5, 17, 4, 11, 11, 13,
			6, 17, 17, 17, 17, 17, 17, 17, 17, 6, 17, 17, 17, 17, 17, 17, 17, 17, 6,
			6, 17, 9, 13, 17, 9, 11, 3, 17, 5, 17, 9, 11, 3, 17, 7, 3, 17, 6,
			6, 18, 17, 6, 17, 17, 17, 17, 17, 0, 17, 17, 17, 17, 17, 6, 17, 18, 6,
			8, 3, 17, 5, 17, 2, 17, 9, 11, 15, 11, 3, 17, 2, 17, 5, 17, 9, 14,
			6, 17, 17, 17, 17, 6, 17, 17, 17, 6, 17, 17, 17, 6, 17, 17, 17, 17, 6,
			6, 17, 9, 11, 11, 12, 11, 3, 17, 5, 17, 9, 11, 12, 11, 11, 3, 17, 6,
			6, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 6,
			4, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 10
		};

		returnData.playerSpawn = PlayerSpawnData{ Entity::Vector2Int{ 9, 16 }, 3 };

		break;

	}

	return returnData;

}

void Board::SpawnEnemy(EnemySpawnData _spawnData)
{

	switch (_spawnData.typeID)
	{

		// Clockwise Miranda
	case 1:
	case 2: enemies.push_back(new Clock{ this, &grid, _spawnData.typeID == 2 }); break;

		// Miranda
	case 3: enemies.push_back(new Miranda{ this, &grid }); break;

		// Alice
	case 4: enemies.push_back(new Alice{ this, &grid }); break;

		// Carla
	case 5: enemies.push_back(new Carla{ this, &grid }); break;

		// Erika
	case 6: enemies.push_back(new Erika{ this, &grid }); break;

		// Marina
	case 7: enemies.push_back(new Marina{ this, &grid }); break;

		// Lurker
	default: enemies.push_back(new Lurker{ this, &grid }); break;

	}

	(*enemies[enemies.size() - 1]).SetSpawn(_spawnData.spawnPos, _spawnData.spawnDir);

}

void Board::SpawnNextMaze()
{

	if (mazeIDs.size() == 0) { RefillMazeQueue(); }

	MazeSpawnData nextMaze = ReturnMazeSpawnData(mazeIDs[0]);

	grid = Grid(this, nextMaze.gridTiles);
	dotsCollected = 0;
	dotGoal = grid.GetDotCount();

	player.SetSpawn(nextMaze.playerSpawn.spawnPos, nextMaze.playerSpawn.spawnDir);

	RemoveAllEnemies();
	for (int i = 0; i < nextMaze.enemySpawns.size(); i++) { SpawnEnemy(nextMaze.enemySpawns[i]); }

	mazeIDs.erase(mazeIDs.begin());

	clearedRounds++;
	clearedRounds %= 6;

}