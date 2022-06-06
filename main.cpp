#include<raylib.h>

#include "Game.h"

int main(void)
{

	Game game{ 304, 448, 60, "test" };

	game.Start();

	while (!game.GameShouldClose()) { game.Tick(); }

	return 0;

}