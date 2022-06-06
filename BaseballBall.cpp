#include "Baseball.h"

using namespace Baseball_Arena;

Arena::Ball::Ball()
{
}

Arena::Ball::Ball(Arena* _arena, int _ID)
{

	arena = _arena;
	animAtlas = arena->GetTexture(2);

	ID = _ID;

	pos = Vector2{ 12, 1.5f };

	switch (ID)
	{
	case 2:
		v = Vector2{ -7, 8 };
		break;
	case 3:
		v = Vector2{ -7, 5 };
		break;
	default:
		v = Vector2{ -7, 0 };
		break;
	}

}

void Arena::Ball::Update()
{

	float deltaTime = arena->GetDeltaTime();

	switch (ID)
	{

		// Regular Ball
	case 0:

		Move(deltaTime);

		break;

		// Regular Ball
	case 1:

		// Rotating
		if (stepCounter == 1)
		{

			stepTimer += deltaTime;

			float a = 360 * stepTimer / 1.5f;

			if (a < 360)
			{

				// Convert To Radians
				a *= 3.14159265f / 180;

				pos = Vector2{ -2 * (float)sin(a) + 6, 2 * (1 - (float)cos(a)) + 1.5f };

			}

			else
			{

				pos = Vector2{ 6, 1.5f };

				stepCounter = 2;

			}

		}

		// Forward
		else
		{

			Move(deltaTime);

			if (stepCounter == 0 && pos.x < 6)
			{

				pos.x = 6;

				stepCounter = 1;

			}

		}

		break;

		// Floaty Ball
	case 2:

		v.y -= 8 * deltaTime;

		Move(deltaTime);

		break;

		// Bouncy Ball
	case 3:

		v.y -= 15 * deltaTime;

		Move(deltaTime);

		if (pos.y <= 0)
		{

			pos.y = 0;

			v.y = 10;

		}

		break;

		// Fast Ball
	case 4:

		switch (stepCounter)
		{

			// Slow Down
		case 0:

			v.x += 10 * deltaTime;

			if (v.x > 0) { stepCounter = 1; }

			else { Move(deltaTime); }

			break;

			// Wait
		case 1:

			stepTimer += deltaTime;

			if (stepTimer >= 0.75f)
			{

				v.x = -15;

				stepCounter = 2;

			}

			break;

			// Speed Up
		case 2:

			Move(deltaTime);

			break;

		}

		break;

		// Sloped Ball
	case 5:

		Move(deltaTime);

		switch (stepCounter)
		{

			// Wait FOr Descent
		case 0:

			if (pos.x < 4) { stepCounter = 1; }

			break;

			// Descend
		case 1:

			pos.y = (pos.x - 2) / 2 * 1.5f;

			if (pos.x < 2)
			{

				pos.y = 0;

				stepCounter = 2;

			}

			break;

		}

		break;

	}

	Vector2 playerPos = arena->GetPlayerPos();

	// Hit By Player
	if (sqrt(pow(pos.x - playerPos.x, 2) + pow(pos.y - playerPos.y, 2)) < 0.5f) { exitCode = 1; }

	// Out Of Bounds
	if (pos.x < -5) { exitCode = 2; }

	animTimer += deltaTime;

	if (animTimer >= 0.05f)
	{

		animTimer -= 0.05f;

		animIndex++;

		animIndex %= 4;

	}

}

void Arena::Ball::OnDraw()
{

	DrawTextureRec(animAtlas, Rectangle{ (float)animIndex * 8, (float)ID * 8, 8, 8 }, GetDrawPos(pos), WHITE);

}