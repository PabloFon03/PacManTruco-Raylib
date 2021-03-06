#pragma once

#include<string>
#include<vector>
#include<raylib.h>

#include "PlayerData.h"
#include "SubScreen.h"

// Game Class
class Game
{

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

	void Start();
	void Tick();

private:

	bool isClosed{ false };

	const int wMin = 304;
	const int hMin = 448;

	RenderTexture2D target;

	RenderTexture2D buffer1;
	RenderTexture2D buffer2;

	void Update();
	void Draw();

	void StartDraw();
	void EndDraw();

	Resources resources;

	PlayerData player;

	SubScreen* content;
	void StartNewScreen(int _ID);

};