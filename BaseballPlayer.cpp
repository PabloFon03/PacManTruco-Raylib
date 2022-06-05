#include "Arena.h"

using namespace Baseball_Arena;

Arena::Player::Player()
{
}

Arena::Player::Player(Arena* _arena)
{

	arena = _arena;

	baseAnimAtlas = arena->GetTexture(0);
	ballAnimAtlas = arena->GetTexture(1);

}

void Arena::Player::Update()
{

	float deltaTime = arena->GetDeltaTime();

	if (onGround)
	{

		if (IsKeyPressed(KEY_Z))
		{

			onGround = false;

			v.y = 25;

			baseAnimIndex = 1;

			baseAnimTimer = 0;

		}

	}

	else
	{

		if (baseAnimIndex > 0)
		{

			baseAnimTimer += deltaTime;

			if (baseAnimTimer >= 0.05f)
			{

				baseAnimTimer -= 0.05f;

				baseAnimIndex++;

				if (baseAnimIndex > 7) { baseAnimIndex = 0; }

			}

		}

		else if (IsKeyPressed(KEY_X)) { v.y = -40; }

		ballAnimTimer += deltaTime;

		if (ballAnimTimer >= 0.05f)
		{

			ballAnimTimer -= 0.05f;

			ballAnimIndex++;

			ballAnimIndex = (ballAnimIndex - 1) % 2 + 1;

		}		

		v.y -= 75 * deltaTime;

		Move(deltaTime);

		if (pos.y <= 0)
		{

			pos.y = 0;

			ballAnimIndex = 0;

			onGround = true;

		}

	}

}

void Arena::Player::OnDraw()
{

	// Draw Ball
	DrawTextureRec(ballAnimAtlas, Rectangle{ (float)ballAnimIndex * 16, 0, 16, 16 }, Vector2{ GetDrawPos(pos) }, WHITE);

	// Draw Base
	DrawTextureRec(baseAnimAtlas, Rectangle{ (float)baseAnimIndex * 24, 0, 24, 20 }, GetDrawPos(Vector2{ -0.25f, -0.625f }), WHITE);

}