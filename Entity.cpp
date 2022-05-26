#include "Board.h";

Entity::Entity()
{
}

Entity::~Entity()
{

	UnloadTexture(animAtlas);

}

Entity::Vector2Int Entity::dir(int i)
{

	switch (i % 4)
	{
	case 0: return Vector2Int{ 0, -1 };
	case 1: return Vector2Int{ 1, 0 };
	case 2: return Vector2Int{ 0, 1 };
	case 3: return Vector2Int{ -1, 0 };
	default: return Vector2Int{ 0, 0 };
	}

}

void Entity::LoadTextureAtlas()
{

	animAtlas = LoadTexture("img/wakawaka.png");
	SetTextureFilter(animAtlas, TEXTURE_FILTER_POINT);

}

void Entity::Update(Board* _board, Maze* _maze)
{

	float deltaTime = GetFrameTime();

	stepTimer += 3.5f * deltaTime;

	if (stepTimer >= 1)
	{

		coords = AddDir(coords, dirIndex);

		switch (_maze->GetTileID(CoordsToIndex(coords)))
		{

		case 18:

			_maze->ClearTile(CoordsToIndex(coords));

			_board->AddScore(10);

			break;

		case 19:

			_maze->ClearTile(CoordsToIndex(coords));

			_board->AddScore(50);

			break;

		}

		ChangeDir(_maze);

		stepTimer--;

	}	

	if (hitWall)
	{

		ChangeDir(_maze);

		stepTimer = 0;

	}

	else
	{

		animDelay += deltaTime;

		if (animDelay >= 0.08f)
		{

			animIndex++;

			animIndex %= 4;

			animDelay -= 0.08f;

		}

	}

}

void Entity::OnDraw()
{

	DrawTextureRec(animAtlas, Rectangle{ (float)animIndex * 16, (float)dirIndex * 16, 16, 16 }, Vector2{ (coords.x + dir(dirIndex).x * stepTimer) * 16, (coords.y + dir(dirIndex).y * stepTimer) * 16 + 48 }, WHITE);

}

void Entity::ChangeDir(Maze* _maze)
{

	for (int i = 0; i < 4; i++)
	{

		if (IsKeyDown(std::vector<int> { KEY_UP, KEY_RIGHT, KEY_DOWN, KEY_LEFT }[i]) && IsValidDir(_maze, coords, i))
		{

			dirIndex = i;

			break;

		}

	}

	hitWall = !IsValidDir(_maze, coords, dirIndex);

}

Entity::Vector2Int Entity::AddDir(Vector2Int _coords, int _dirIndex)
{

	Vector2Int returnCoords = Vector2Int{ _coords.x, _coords.y };

	Vector2Int dirVec = dir(_dirIndex);

	returnCoords.x = (returnCoords.x + dirVec.x) % 19;

	returnCoords.y = (returnCoords.y + dirVec.y) % 22;

	return returnCoords;

}

int Entity::CoordsToIndex(Vector2Int coords) { return coords.y * 19 + coords.x; }

bool Entity::IsValidDir(Maze* _maze, Vector2Int _coords, int _dirIndex)
{

	int nextTileID = _maze->GetTileID(CoordsToIndex(AddDir(_coords, _dirIndex)));

	return nextTileID == 0 || nextTileID > 17;

}

float Entity::Vector2Int::magnitude() { return sqrt(powf(x, 2) + powf(y, 2)); }

float Entity::Vector2Int::distanceTo(Vector2Int _target)
{

	Vector2Int diff = Vector2Int{ _target.x - x, _target.y - y };

	return diff.magnitude();

}