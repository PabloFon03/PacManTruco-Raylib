#include "Arena.h"

using namespace Baseball_Arena;

Arena::Arena(std::vector<Texture2D>& _textures)
{

	textures = _textures;

}

void Arena::Start()
{

	player = Player(this);

	balls.clear();

	nextBalls.clear();

	for (int i = 0; i < 100; i++) { nextBalls.push_back(SpawnData{ i % 4, 0.75f }); }

}

void Arena::Update()
{

	float deltaTime = GetDeltaTime();

	if (nextBalls.size() > 0)
	{

		spawnDelay -= deltaTime;

		if (spawnDelay <= 0)
		{

			// Spawn Next Ball
			balls.push_back(Ball{ this, nextBalls[0].GetID() });

			// Add Next Spawn Delay
			spawnDelay += nextBalls[0].GetDelay();

			// Delete From Spawn Queue
			nextBalls.erase(nextBalls.begin());

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
			if (balls[i].GetExitCode() == 1) {}

			// Missed Ball
			else {}

			// Delete Ball
			balls.erase(std::next(balls.begin(), i));

		}

	}

}

void Arena::OnDraw()
{

	// Draw Spawned Balls
	for (int i = 0; i < balls.size(); i++) { balls[i].OnDraw(); }

	// Draw Player
	player.OnDraw();

	// Draw Player Platform
	DrawTexture(textures[5], 32, 278, WHITE);

	// Draw Pitcher
	DrawTextureRec(textures[3], Rectangle{ (float)pitcherAnimIndex * 18, 0, 18, 32 }, Vector2{ 238, 224 }, WHITE);

	// Draw Pitcher Platform
	DrawTexture(textures[4], 224, 256, WHITE);

}