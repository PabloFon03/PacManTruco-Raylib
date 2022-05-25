#include<vector>
#include<raylib.h>
#pragma once
class Maze {

public:

	Maze();

	// Constrcutor
	Maze(std::vector<int> rawTiles);

	// Destructor
	~Maze() noexcept;

	void LoadTextureAtlas();
	void Update();
	void OnDraw();

private:

	std::vector<int> tiles;
	Texture2D tilesAtlas;

};