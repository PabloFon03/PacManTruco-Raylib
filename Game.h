#pragma once
#include <string>
#include <raylib.h>
#include "Maze.h";
#include "Entity.h";

class Game {

public:

	// Constrcutor
	Game(int w, int h, int fps, std::string t);

	// Destructor
	~Game() noexcept;

	// Copy Constructor
	Game(const Game& other) = delete;

	// Copy Assignment
	Game& operator = (const Game& other) = delete;

	bool GameShouldClose() const;

	void LoadTextures();
	void Tick();

private:

	const int wMin = 304;
	const int hMin = 352;

	int frameCounter;

	RenderTexture2D target;

	Maze currentMaze;

	Entity player;

	void Update();
	void Draw();
	void OnDraw();

};