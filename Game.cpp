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
	CloseWindow();
}

bool Game::GameShouldClose() const { return WindowShouldClose(); }


void Game::Start()
{

	board.Start();

}

void Game::Tick()
{
	frameCounter++;
	Update();
	Draw();
}

void Game::Update()
{
	board.Update();
}

void Game::Draw()
{

	BeginTextureMode(target);

	ClearBackground(Color{ 5, 5, 5, 255 });

	OnDraw();

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

void Game::OnDraw()
{

	board.OnDraw();

}