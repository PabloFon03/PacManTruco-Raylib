#include <assert.h>

#include "Game.h";
#include "Maze.h";
#include "Baseball.h";
#include "Claw.h";

Game::Game(int w, int h, int fps, std::string t)
{

	assert(!IsWindowReady());

	SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
	InitWindow(w, h, t.c_str());
	SetWindowMinSize(wMin, hMin);
	SetTargetFPS(fps);

	target = LoadRenderTexture(wMin, hMin);
	SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);

}

Game::~Game() noexcept
{

	assert(GetWindowHandle());

	// Free Window Content
	delete content;

	CloseWindow();

}

bool Game::GameShouldClose() const { return WindowShouldClose(); }

void Game::Start()
{

	resources = Resources();

	// Load Resources
	resources.Load();

	StartNewScreen(5);

}

void Game::StartNewScreen(int _ID)
{

	// Free Old Content
	delete content;

	// Create New Content
	switch (_ID)
	{

	case 1:
		content = new PacMan_Board::Board(&resources);
		break;

	// Baseball Minigame
	case 2:
	case 3:
	case 4:
		content = new Baseball_Arena::Arena(&resources, _ID - 2);
		break;

	// Claw Minigame
	case 5:
	case 6:
	case 7:
		content = new Claw_Board::Board(&resources, _ID - 5);
		break;

	default:
		break;

	}

	// Initcialize New Content
	(*content).Start();

}

void Game::Tick()
{

	Update();

	Draw();

}

void Game::Update()
{
	
	switch ((*content).exitFlag)
	{

	// Keep Going
	default:

		(*content).Update();

		break;

	}

}

void Game::Draw()
{

	StartDraw();

	(*content).OnDraw();

	EndDraw();

}

void Game::StartDraw()
{

	BeginTextureMode(target);

	ClearBackground(BLACK);

}

void Game::EndDraw()
{

	EndTextureMode();

	BeginDrawing();

	float wScale = (float)GetScreenWidth() / wMin;
	float hScale = (float)GetScreenHeight() / hMin;

	float scale = wScale < hScale ? wScale : hScale;

	DrawTexturePro(target.texture,
		Rectangle{ 0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height },
		Rectangle{
		(GetScreenWidth() - ((float)wMin * scale)) * 0.5f,
		(GetScreenHeight() - ((float)hMin * scale)) * 0.5f,
		(float)wMin * scale,
		(float)hMin * scale },
		Vector2{ 0, 0 }, 0.0f, WHITE);

	EndDrawing();

}