#pragma once

#include<string>
#include<vector>
#include<raylib.h>

#include "SubScreen.h"

namespace PacMan_Board
{
	/*
	namespace Items
	{

		class PlayerItem
		{

		public:

			PlayerItem(int _price, int _mana)
			{

				price = _price;

				isElectric = false;
				manaCost = _mana;

			}

			PlayerItem(int _price, float _discharge)
			{

				price = _price;

				isElectric = true;
				dischargeRate = _discharge;

			}

		private:

			int price{ 0 };

			bool isElectric{ false };
			int manaCost{ 0 };
			float dischargeRate{ 0.0f };

		};

		PlayerItem Items[8]
		{

			// None
			PlayerItem{ 0, 0 },

			// Sword
			PlayerItem{ 0, 0 },

			// None
			PlayerItem{ 0, 0 },

			// Sword
			PlayerItem{ 0, 0 },

			// None
			PlayerItem{ 0, 0 },

			// Sword
			PlayerItem{ 0, 0 },

			// Freeze
			PlayerItem{ 0, 0.0f },

			// Boost
			PlayerItem{ 0, 0.0f }

		};

		int CharmPrices[8]
		{

			// None
			0,

			// Score x3
			0,

			// Lifes x3
			0,

			// Crystals x3
			0,

			// Score x4
			0,

			// Lifes x5
			0,

			// Crystals x5
			0,

			// Locket
			0,

		};

	}
	*/
	// Board Class
	class Board : public SubScreen
	{

	public:

		// Grid Class
		class Grid
		{

		public:

			Grid();

			// Constrcutor
			Grid(Board* _board, std::vector<int> _rawTiles);

			// Destructor
			~Grid();

			void Update();
			void OnDraw(int _ColorID);

			void ClearTile(int _i);
			int GetTileID(int _i);

			int GetDotCount();

		private:

			Board* board;

			std::vector<int> tiles;
			Texture2D tilesAtlas;

			int ColorID{ 1 };

		};

		// Abstract Class For Player And Enemy
		class Entity
		{

		public:

			class Vector2Int
			{

			public:

				int x;
				int y;

				float magnitude() { return sqrt(powf(x, 2) + powf(y, 2)); }

				float distanceTo(Vector2Int _target)
				{

					Vector2Int diff = Vector2Int{ _target.x - x, _target.y - y };
					return diff.magnitude();

				}

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
			Grid* grid;

			Vector2Int coords;
			int dirIndex;
			Vector2Int dir(int _i);

			float rawDistanceTo(Vector2 _target)
			{

				Vector2 rawCoords = GetRawCoords();
				Vector2 diff = Vector2{ _target.x - rawCoords.x, _target.y - rawCoords.y };
				return sqrt(powf(diff.x, 2) + powf(diff.y, 2));

			}

			Vector2 GetRawCoords() { return Vector2{ TrueMod(coords.x + dir(dirIndex).x * stepTimer, 19), TrueMod(coords.y + dir(dirIndex).y * stepTimer, 22) }; }

			void DrawCurrentFrame(Texture2D& _animAtlas);
			virtual void OnDraw() = 0;

			virtual Vector2Int TileSize() = 0;

			int animIndex{ 0 };
			float animDelay{ 0 };

			float stepTimer{ 0 };
			bool hitWall{ false };

			float stateTimer;
			int stateIndex;

			virtual void ChangeDir();

			Vector2Int AddDir(Vector2Int _coords, int _dirIndex, bool _modWrap = true);
			int CoordsToIndex(Vector2Int _coords);
			bool IsValidDir(Vector2Int _coords, int _dirIndex);

		};

		// Player Class
		class Player : public Entity
		{

		public:

			Player() {}

			Player(Board* _board, Grid* _grid, Vector2Int _spawnPos, int _dirIndex)
			{

				board = _board;

				grid = _grid;

				coords = _spawnPos;

				dirIndex = _dirIndex;

				mainAnimAtlas = board->GetTexture(2);

			}

			void Update();
			void OnDraw() { DrawCurrentFrame(mainAnimAtlas); }

		private:

			Texture2D mainAnimAtlas;

			void ChangeDir();

			Vector2Int TileSize() { return Vector2Int{ 24, 24 }; }

		};

		// Enemy Class
		class Enemy : public Entity
		{

		public:

			Enemy() {}
			Enemy(Board* _board, Grid* _grid, int _ID, Vector2Int _spawnPos, int _dirIndex)
			{

				board = _board;

				grid = _grid;

				ID = _ID;

				coords = _spawnPos;

				dirIndex = _dirIndex;

				mainAnimAtlas = board->GetTexture(ID + 3);

			}

			void Update();
			void OnDraw() { DrawCurrentFrame(mainAnimAtlas); }

		private:

			Texture2D mainAnimAtlas;

			int ID{ -1 };

			Vector2Int GetTarget();

			void ChangeDir();

			Vector2Int TileSize() { return Vector2Int{ 18, 52 }; }

		};

		Board(Resources* _res);

		void Update();
		void OnDraw();

		void ClearTile(int _i);
		void AddScore(int _s);
		void DotCollected();
		void OnPowerCollected();
		void OnPlayerHit();

		void SpawnEnemy(int _ID, Entity::Vector2Int _spawnCoords, int _spawnDir) { enemies.push_back(Enemy{ this, &grid, _ID, _spawnCoords, _spawnDir }); }

		float GetDeltaTime() { return GetRawDeltaTime() * (1 + 0.1f * speedMod); }

		Entity::Vector2Int GetPlayerPos() { return player.coords; }
		Vector2 GetPlayerRawPos() { return player.GetRawCoords(); }
		int GetPlayerDirIndex() { return player.dirIndex; }

		Texture2D GetTexture(int _i) { return (*resources).GetMazeTexture(_i); }

		int ExitFlag() { return 0; }

	private:

		int clearedRounds{ 0 };

		int score{ 0 };
		int speedScore{ 0 };
		int speedMod{ 0 };

		int dotsCollected;
		int dotGoal;

		Grid grid;
		Player player;
		std::vector<Enemy> enemies{ std::vector<Enemy>(0) };

		enum States { Starting, Playing, Paused, Defeat, Cleared, TimeUp, Results };
		States currentState{ Starting };
		int stepCounter{ 0 };
		float stepTimer{ 0 };

	};

}