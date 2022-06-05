#include <assert.h>

#include "Game.h";
#include "Board.h";
#include "Arena.h";

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

	// Free Content
	delete content;

	for (int i = 0; i < textures.size(); i++) { UnloadTexture(textures[i]); }

	CloseWindow();

}

bool Game::GameShouldClose() const { return WindowShouldClose(); }

void Game::Start()
{

	// Clear Texture List
	textures.clear();

	// Load Textures
	std::vector<std::string> textureFiles{ "img/electric base.png", "img/electric ball.png", "img/baseballs.png", "img/pitcher.png", "img/pitcher platform.png", "img/player platform.png", "img/round counter.png", "img/sqweek.png", "img/mouse gal.png", "img/tsundere cheesecake.png", "img/alice.png", "img/orange.png"};

	for (int i = 0; i < textureFiles.size(); i++)
	{

		textures.push_back(LoadTexture(textureFiles[i].c_str()));

		SetTextureFilter(textures[i], TEXTURE_FILTER_POINT);

	}

	StartNewScreen(2);

}

void Game::StartNewScreen(int _ID)
{

	// Free Old Content
	delete content;

	// Create New Content
	switch (_ID)
	{

	case 1:
		content = new PacMan_Board::Board(textures);
		break;

	case 2:
		content = new Baseball_Arena::Arena(textures);
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