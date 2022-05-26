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

private:

	class Vector2Int
	{

	public:

		int x;
		int y;

		float magnitude();
		float distanceTo(Vector2Int _target);

	};

public:

	Entity();
	~Entity();

	Vector2Int coords;
	int dirIndex;
	Vector2Int dir(int _i);

	void LoadTextureAtlas();
	void Update(Board* _board, Maze* _maze);
	void OnDraw();

private:

	int animIndex;
	float animDelay;

	float stepTimer;
	bool hitWall;

	void ChangeDir(Maze* _maze);

	Vector2Int AddDir(Vector2Int _coords, int _dirIndex);
	int CoordsToIndex(Vector2Int _coords);
	bool IsValidDir(Maze* _maze, Vector2Int _coords, int _dirIndex);

	Texture2D animAtlas;

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

private:

	int score;

	Maze maze;
	Entity player;

};