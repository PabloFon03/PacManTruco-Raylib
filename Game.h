#include <string>
#include <raylib.h>
#include "Board.h";

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
	const int hMin = 400;

	int frameCounter;

	RenderTexture2D target;

	Board board;

	void Update();
	void Draw();
	void OnDraw();

};