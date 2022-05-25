#include<raylib.h>
#include "Game.h"

int main(void)
{
	const int screenWidth = 304;
	const int screenHeight = 352;

	Game game{ screenWidth, screenHeight, 60, "test" };

	game.LoadTextures();

	while (!game.GameShouldClose())
	{
		game.Tick();
	}

	return 0;
}