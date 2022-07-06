#include <assert.h>

#include "Game.h";
#include "MainMenu.h";
#include "LoadingTrain.h";
#include "ItemShop.h";
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

bool Game::GameShouldClose() const { return WindowShouldClose() || isClosed; }

void Game::Start()
{

	resources = Resources();

	// Load Resources
	resources.Load();

	// Start Intro Screen
	StartNewScreen(6);

}

void Game::StartNewScreen(int _ID)
{

	// Free Old Content Memory
	delete content;

	// Create New Content
	switch (_ID)
	{

	// Main Menu
	case -1:
	case 0:
		content = new MainMenu_Screen::MainMenu(&resources, player.GetTokens());
		break;

		// Maze
	case 1:
	case 2:
	case 3:
		content = new ItemShop_Screen::ItemShop(&resources, _ID - 1, player.GetTokens() + 25);
		break;

	// Maze
	case 4:
	case 5:
	case 6:
		content = new PacMan_Board::Board(&resources, _ID - 4);
		break;

	// Baseball Minigame
	case 7:
	case 8:
	case 9:
		content = new Baseball_Arena::Arena(&resources, _ID - 7);
		break;

	// Claw Minigame
	case 10:
	case 11:
	case 12:
		content = new Claw_Board::Board(&resources, _ID - 10);
		break;

	default:

		// Loading Screens
		if (_ID >= 40 && _ID <= 52) { content = new Loading_Screen::LoadScreen(&resources, _ID - 40); }

		break;

	}

}

void Game::Tick()
{

	// Update Logic
	Update();

	// Draw On Screen
	Draw();

}

void Game::Update()
{
	
	int exitFlag = (*content).exitFlag;

	switch (exitFlag)
	{

	// Keep Going
	case -1:

		(*content).Update();

		break;

	// Close App
	case -2:

		isClosed = true;

		break;

	// Load New Screen
	default:

		player.AddTokens((*content).GetTokens());

		StartNewScreen(exitFlag);

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