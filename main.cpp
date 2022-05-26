#include<raylib.h>
#include "Game.h"

int main(void)
{
	const int screenWidth = 304;
	const int screenHeight = 400;

	Game game{ screenWidth, screenHeight, 60, "test" };

	game.Start();

	while (!game.GameShouldClose())
	{
		game.Tick();
	}

	return 0;
}