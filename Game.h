#include "Board.h"

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

	void StartBoard();

	void Tick();
	void Update();
	void Draw();

	void StartDraw();
	void EndDraw();

private:

	const int wMin = 304;
	const int hMin = 448;

	RenderTexture2D target;

	std::vector<Texture2D> textures;

	enum Screens { MainMenu, StageSelect, Maze, CamilaShop, HappyShop };

	Screens currentScreen{ Maze };

	Board board;

};