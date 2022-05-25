#include <assert.h>
#include "Game.h";
#include "Entity.h";

Game::Game(int w, int h, int fps, std::string t)
{
	assert(!IsWindowReady());

	SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
	InitWindow(w, h, t.c_str());
	SetWindowMinSize(wMin, hMin);
	SetTargetFPS(fps);

	target = LoadRenderTexture(wMin, hMin);
	SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);

	// 19x22
	std::vector<int> mazeTiles
	{
		7, 11, 11, 11, 11, 11, 11, 11, 11, 15, 11, 11, 11, 11, 11, 11, 11, 11, 13,
		6, 18, 18, 18, 18, 18, 18, 18, 18, 6, 18, 18, 18, 18, 18, 18, 18, 18, 6,
		6, 19, 7, 13, 18, 7, 11, 13, 18, 6, 18, 7, 11, 13, 18, 7, 13, 19, 6,
		6, 18, 4, 10, 18, 4, 11, 10, 18, 5, 18, 4, 11, 10, 18, 4, 10, 18, 6,
		6, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 6,
		6, 18, 9, 3, 18, 2, 18, 9, 11, 15, 11, 3, 18, 2, 18, 9, 3, 18, 6,
		6, 18, 18, 18, 18, 6, 18, 18, 18, 6, 18, 18, 18, 6, 18, 18, 18, 18, 6,
		4, 11, 11, 13, 18, 8, 11, 3, 0, 5, 0, 9, 11, 14, 18, 7, 11, 11, 10,
		0, 0, 0, 6, 18, 6, 0, 0, 0, 0, 0, 0, 0, 6, 18, 6, 0, 0, 0,
		11, 11, 11, 10, 18, 5, 0, 7, 3, 17, 9, 13, 0, 5, 18, 4, 11, 11, 11,
		0, 0, 0, 0, 18, 0, 0, 6, 0, 0, 0, 6, 0, 0, 18, 0, 0, 0, 0,
		11, 11, 11, 13, 18, 2, 0, 4, 11, 11, 11, 10, 0, 2, 18, 7, 11, 11, 11,
		0, 0, 0, 6, 18, 6, 0, 0, 0, 0, 0, 0, 0, 6, 18, 6, 0, 0, 0,
		7, 11, 11, 10, 18, 5, 0, 9, 11, 15, 11, 3, 0, 5, 18, 4, 11, 11, 13,
		6, 18, 18, 18, 18, 18, 18, 18, 18, 6, 18, 18, 18, 18, 18, 18, 18, 18, 6,
		6, 18, 9, 13, 18, 9, 11, 3, 18, 5, 18, 9, 11, 3, 18, 7, 3, 18, 6,
		6, 19, 18, 6, 18, 18, 18, 18, 18, 0, 18, 18, 18, 18, 18, 6, 18, 19, 6,
		8, 3, 18, 5, 18, 2, 18, 9, 11, 15, 11, 3, 18, 2, 18, 5, 18, 9, 14,
		6, 18, 18, 18, 18, 6, 18, 18, 18, 6, 18, 18, 18, 6, 18, 18, 18, 18, 6,
		6, 18, 9, 11, 11, 12, 11, 3, 18, 5, 18, 9, 11, 12, 11, 11, 3, 18, 6,
		6, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 6,
		4, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 10
	};
	currentMaze = Maze(mazeTiles);

	player.x = 9;
	player.y = 16;
	player.dir = 3;

}

Game::~Game() noexcept
{
	assert(GetWindowHandle());
	CloseWindow();
}

bool Game::GameShouldClose() const { return WindowShouldClose(); }

void Game::LoadTextures()
{

	currentMaze.LoadTextureAtlas();

	player.LoadTextureAtlas();

}

void Game::Tick()
{
	frameCounter++;
	Update();
	Draw();
}

void Game::Update()
{
	currentMaze.Update();
	player.Update();
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

	currentMaze.OnDraw();

	player.Draw();

}