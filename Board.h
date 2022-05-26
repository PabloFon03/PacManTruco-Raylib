#include<string>
#include<vector>
#include<raylib.h>


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

};

// Declared Before Definition
class Board;

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

	Entity();
	~Entity();

	int TrueMod(int _i, int _n) { return (_i % _n + _n) % _n; }

	Vector2Int coords;
	int dirIndex;
	Vector2Int dir(int _i);

	virtual void LoadTextureAtlas();
	virtual void Update(Board* _board, Maze* _maze);
	void OnDraw();

	int animIndex;
	float animDelay;

	float stepTimer;
	bool hitWall;

	virtual void ChangeDir(Maze* _maze);

	Vector2Int AddDir(Vector2Int _coords, int _dirIndex);
	int CoordsToIndex(Vector2Int _coords);
	bool IsValidDir(Maze* _maze, Vector2Int _coords, int _dirIndex);

	Texture2D animAtlas;

};

class Player : public Entity
{

public:

	void LoadTextureAtlas();
	void Update(Board* _board, Maze* _maze);

private:

	void ChangeDir(Maze* _maze);

};

class Enemy : public Entity
{

public:

	void LoadTextureAtlas();
	void Update(Board* _board, Maze* _maze, Vector2Int _playerPos);

private:

	void ChangeDir(Maze* _maze, Vector2Int _playerPos);

};

class Board
{

public:

	Board();

	void Start();
	void Update();
	void OnDraw();

	void ClearTile(int _i);
	void AddScore(int _s);
	void OnPowerCollected();

private:

	int score;

	Maze maze;
	Player player;

	Enemy enemy;

};