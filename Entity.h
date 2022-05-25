#include<raylib.h>
#pragma once
class Entity
{

public:

	Entity();
	~Entity();

	int x;
	int y;
	int dir;

	void LoadTextureAtlas();
	void Update();
	void Draw();

private:

	int animIndex;
	float animDelay;

	Texture2D animAtlas;

};