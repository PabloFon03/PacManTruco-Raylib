#include<raylib.h>
#pragma once
class Entity
{

public:

	Entity();

	int x;

	int y;

	int dir;

	void Update();

	void Draw();

private:

	int frameCounter;

};