#include <assert.h>
#include "Game.h";

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

	for (int i = 0; i < textures.size(); i++) { UnloadTexture(textures[i]); }
	CloseWindow();

}

bool Game::GameShouldClose() const { return WindowShouldClose(); }

void Game::Start()
{

	// Clear Texture List
	textures.clear();

	// Load Textures
	std::vector<std::string> textureFiles{ "img/round counter.png", "img/sqweek.png", "img/mouse gal.png", "img/tsundere cheesecake.png", "img/alice.png", "img/orange.png"};

	for (int i = 0; i < textureFiles.size(); i++)
	{

		textures.push_back(LoadTexture(textureFiles[i].c_str()));

		SetTextureFilter(textures[i], TEXTURE_FILTER_POINT);

	}

	StartBoard();

}

void Game::StartBoard()
{

	board = Board(textures);

	board.Start();

}

void Game::Tick()
{
	Update();
	Draw();
}

void Game::Update()
{
	
	switch (board.ExitFlag())
	{

	// Keep Going
	case 0:

		board.Update();

		break;

	}

}

void Game::Draw()
{

	StartDraw();

	board.OnDraw();

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