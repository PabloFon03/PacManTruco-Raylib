#include<string>
#include<vector>
#include<raylib.h>

#include "SubScreen.h"

// Game Class
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

	void Start();
	void Tick();

private:

	const int wMin = 304;
	const int hMin = 448;

	RenderTexture2D target;

	void Update();
	void Draw();

	void StartDraw();
	void EndDraw();

	Resources resources;

	SubScreen* content;
	void StartNewScreen(int _ID);

};