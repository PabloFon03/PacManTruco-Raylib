#include "Arena.h"

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

	}

	Vector2 playerPos = arena->GetPlayerPos();

	// Hit By Player
	if (sqrt(pow(pos.x - playerPos.x, 2) + pow(pos.y - playerPos.y, 2)) < 0.5f) { exitCode = 1; }

	// Out Of Bounds
	else if (pos.x < -5) { exitCode = 2; }

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