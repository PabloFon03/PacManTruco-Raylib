#include<vector>
#include<raylib.h>
#include "Entity.h";
#pragma once
class Maze {

public:

	Maze();

	// Constrcutor
	Maze(std::vector<int> rawTiles);

	// Destructor
	~Maze() noexcept;

	void OnDraw();

private:

	std::vector<int> tiles;
	Texture2D tilesAtlas;

	Entity player;

};