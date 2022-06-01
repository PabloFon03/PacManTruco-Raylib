#include<string>
#include<vector>
#include<raylib.h>

// Maze Class
class Maze
{

public:

	Maze();

	// Constrcutor
	Maze(std::vector<int> _rawTiles);

	// Destructor
	~Maze() noexcept;

	void LoadTextureAtlas();
	void Update();
	void OnDraw();

	void ClearTile(int _i);
	int GetTileID(int _i);

private:

	std::vector<int> tiles;
	Texture2D tilesAtlas;

	int ColorID{ 5 };

};

// Declared Before Definition
class Board;

// Abstract Class For Player And Enemy
class Entity
{

public:

	class Vector2Int
	{

	public:

		int x;
		int y;

		float magnitude();
		float distanceTo(Vector2Int _target);

	};

	float TrueMod(float _i, float _n)
	{

		if (_n < 0) { _n *= -1; }

		while (_i < 0) { _i += _n; }

		while (_i >= _n) { _i -= _n; }

		return _i;
	}

	Entity();
	virtual ~Entity();

	Board* board;

	Vector2Int coords;
	int dirIndex;
	Vector2Int dir(int _i);

	Vector2 GetRawCoords() { return Vector2{ TrueMod(coords.x + dir(dirIndex).x * stepTimer, 19), TrueMod(coords.y + dir(dirIndex).y * stepTimer, 22) }; }

	void DrawCurrentFrame(Texture2D& _animAtlas);
	virtual void OnDraw() = 0;

	virtual Vector2Int TileSize() = 0;

	int animIndex{ 0 };
	float animDelay{ 0 };

	float stepTimer{ 0 };
	bool hitWall{ false };

	virtual void ChangeDir(Maze* _maze);

	Vector2Int AddDir(Vector2Int _coords, int _dirIndex);
	int CoordsToIndex(Vector2Int _coords);
	bool IsValidDir(Maze* _maze, Vector2Int _coords, int _dirIndex);

};

// Player Class
class Player : public Entity
{

public:

	Player() {}

	Player(Board* _board, Vector2Int _spawnPos, int _dirIndex);

	void Update(Maze* _maze);
	void OnDraw() { DrawCurrentFrame(mainAnimAtlas); }

private:

	Texture2D mainAnimAtlas;

	void ChangeDir(Maze* _maze);

	Vector2Int TileSize() { return Vector2Int{ 24, 24 }; }

};

// Enemy Class
class Enemy : public Entity
{

public:

	Enemy();
	Enemy(Board* _board, int _ID, Vector2Int _spawnPos, int _dirIndex);

	void Update(Maze* _maze, Vector2Int _playerPos);
	void OnDraw() { DrawCurrentFrame(mainAnimAtlas); }

private:

	Texture2D mainAnimAtlas;

	int ID{ -1 };

	Vector2Int GetTarget();

	void ChangeDir(Maze* _maze);

	Vector2Int TileSize() { return Vector2Int{ 18, 32 }; }

};

// Board Class
class Board
{

public:

	Board();

	Board(std::vector<Texture2D>& _textures);

	void Start();
	void Update();
	void OnDraw();

	void ClearTile(int _i);
	void AddScore(int _s);
	void OnPowerCollected();

	float GetDeltaTime() { return GetFrameTime() * (1 + 0.1f * speedMod); }

	Entity::Vector2Int GetPlayerPos() { return player.coords; }
	int GetPlayerDirIndex() { return player.dirIndex; }

	Texture2D GetTexture(int _i) { return textures[_i]; }

private:

	std::vector<Texture2D> textures;

	int score{ 0 };
	int speedScore{ 0 };
	int speedMod{ 0 };

	Maze maze;
	Player player;
	std::vector<Enemy> enemies{ std::vector<Enemy>(0) };

};

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

	// Texture2D* GetTexture(int _i) { return &textures[_i]; }

private:

	const int wMin = 304;
	const int hMin = 400;

	int frameCounter;

	RenderTexture2D target;

	std::vector<Texture2D> textures;

	Board board;

	void Update();
	void Draw();
	void OnDraw();

};