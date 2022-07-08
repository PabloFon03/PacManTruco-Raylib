#pragma once

#include<string>
#include<vector>
#include<raylib.h>

#include "SubScreen.h"
#include "Random.h"

namespace PacMan_Board
{

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

			int GetPrice() { return price; }

			bool IsElectric() { return isElectric; }
			int GetManaCost() { return manaCost; }
			int GetDischargeRate() { return dischargeRate; }

		private:

			int price{ 0 };

			bool isElectric{ false };
			int manaCost{ 0 };
			float dischargeRate{ 0.0f };

		};

		inline PlayerItem GetItem(int _i)
		{

			PlayerItem Items[8]
			{

				// None
				PlayerItem{ 0, 0 },

				// Sword
				PlayerItem{ 1, 2 },

				// Jump
				PlayerItem{ 3, 0 },

				// Dash
				PlayerItem{ 5, 0 },

				// Magic
				PlayerItem{ 8, 0 },

				// Projectile
				PlayerItem{ 10, 0 },

				// Freeze
				PlayerItem{ 12, 0.2f },

				// Boost
				PlayerItem{ 15, 0.5f }

			};

			return Items[_i];

		}

		inline int GetCharmPrice(int _i)
		{

			int CharmPrices[8]
			{

				// None
				0,

				// Score x2
				3,

				// Lifes x3
				3,

				// Crystals x3
				5,

				// Score x4
				8,

				// Lifes x5
				10,

				// Crystals x5
				15,

				// Locket
				25

			};

			return CharmPrices[_i];

		}

	}

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

			Vector2Int spawnCoords;
			int spawnDirIndex;
			void SetSpawn(Vector2Int _coords, int _dirIndex)
			{

				spawnCoords = _coords;
				spawnDirIndex = _dirIndex;

				Spawn();

			}
			void Spawn()
			{

				stepTimer = 0;

				coords = spawnCoords;
				dirIndex = spawnDirIndex;

			}

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

			void DrawCurrentFrame(Texture2D& _animAtlas, int _frameIndex, Vector2 _tileSize, Vector2 _posOffset = Vector2{ 0, 0 });
			virtual void OnDraw() = 0;

			int animIndex{ 0 };
			float animDelay{ 0 };

			float stepTimer{ 0 };
			bool hitWall{ false };

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

			Player(Board* _board, Grid* _grid)
			{

				board = _board;
				grid = _grid;

				mainAnimAtlas.atlas = board->GetTexture(4);
				mainAnimAtlas.tileSize = Vector2{ 24, 24 };

				swordAnimAtlas.atlas = board->GetTexture(5);
				swordAnimAtlas.tileSize = Vector2{ 16, 16 };

				dashAnimAtlas.atlas = board->GetTexture(6);
				dashAnimAtlas.tileSize = Vector2{ 16, 16 };

				magicAnimAtlas.atlas = board->GetTexture(7);
				magicAnimAtlas.tileSize = Vector2{ 18, 52 };

			}

			void Update();
			void OnDraw();

			int ReturnEnemyCollisionOutcome() { return currentState == Dash || currentState == ElectricDash ? 2 : currentState == Magic ? 1 : 0; }

		private:

			struct AnimAtlas
			{
				Texture2D atlas;
				Vector2 tileSize;
			};

			AnimAtlas mainAnimAtlas;
			AnimAtlas swordAnimAtlas;
			AnimAtlas dashAnimAtlas;
			AnimAtlas magicAnimAtlas;

			AnimAtlas GetCurrentAnimAtlas()
			{

				switch (currentState)
				{

				case Dash:
				case ElectricDash:
					return dashAnimAtlas;

				case Magic: return magicAnimAtlas;

				default: return mainAnimAtlas;

				}

			}

			void ChangeDir();
			void BoostTileCheck();

			enum ItemStates { None, Sword, Dash, Magic, Projectile, Trade, Freeze, ElectricDash };
			ItemStates currentState{ None };
			float stateTimer;

			ItemStates items[3]{ Sword, Dash, Magic };

			int energy{ 0 };
			float electricEnergy{ 0.0f };

		};

		// Enemy Class
		class Enemy : public Entity
		{

		public:

			Enemy() {}
			Enemy(Board* _board, Grid* _grid, int _ID)
			{

				board = _board;

				grid = _grid;

				ID = _ID;

				mainAnimAtlas = board->GetTexture(ID + 3);

			}

			void Update();
			void OnDraw() { DrawCurrentFrame(mainAnimAtlas, animIndex, TileSize()); }

			void OnStun() {}

		private:

			Texture2D mainAnimAtlas;

			int ID{ -1 };

			Vector2Int GetTarget();

			void ChangeDir();

			Vector2 TileSize() { return Vector2{ 18, 52 }; }

		};

		Board(Resources* _res, int _difficulty);

		void Update();
		void OnDraw();

		void ClearTile(int _i);
		void AddScore(int _s);
		void DotCollected();
		void OnPlayerHit();

		float GetDeltaTime() { return GetRawDeltaTime() * (1 + 0.1f * speedMod); }

		Entity::Vector2Int GetPlayerPos() { return player.coords; }
		Vector2 GetPlayerRawPos() { return player.GetRawCoords(); }
		int GetPlayerDirIndex() { return player.dirIndex; }
		int GetPlayerHitOutcome() { return player.ReturnEnemyCollisionOutcome(); }

		Texture2D GetTexture(int _i) { return (*resources).GetMazeTexture(_i); }

	private:

		struct PlayerSpawnData
		{

			Entity::Vector2Int spawnPos;

			int spawnDir;

		};

		struct EnemySpawnData
		{

			int typeID;

			Entity::Vector2Int spawnPos;

			int spawnDir;

		};

		struct MazeSpawnData
		{

			std::vector<int> gridTiles;

			PlayerSpawnData playerSpawn;

			std::vector<EnemySpawnData> enemySpawns{ std::vector<EnemySpawnData>() };

		};

		std::vector<int> mazeIDs{ std::vector<int>() };

		void RefillMazeQueue(bool _random = true);

		MazeSpawnData ReturnMazeSpawnData(int _i);

		void SpawnNextMaze();

		void SpawnEnemy(EnemySpawnData _spawnData);

		int difficulty{ 0 };
		int clearedRounds{ 0 };

		float timeLeft{ 300 };

		int score{ 0 };
		int speedScore{ 0 };
		int speedMod{ 0 };

		int dotsCollected{ 0 };
		int dotGoal{ 0 };

		int lifesLeft{ 99 };
		int crystalsLeft{ 99 };

		Grid grid;
		Player player;
		std::vector<Enemy> enemies{ std::vector<Enemy>() };

		enum States { Starting, Playing, Paused, FlipOut, FlipIn, Defeated, TimeUp, Results };
		States currentState{ Starting };
		int stepCounter{ 0 };
		float stepTimer{ 0 };

		int shaderFlipValLocation{ -1 };

	};

}