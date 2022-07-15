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
			float GetDischargeRate() { return dischargeRate; }

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

				// Dash
				PlayerItem{ 3, 0 },

				// Magic
				PlayerItem{ 5, 0 },

				// Projectile
				PlayerItem{ 8, 0 },

				// Trade
				PlayerItem{ 10, 0.125f },

				// Freeze
				PlayerItem{ 12, 0.4f },

				// Boost
				PlayerItem{ 15, 0.25f }

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

			void ClearTile(int _i) { tiles[_i] = 0; }
			int GetTileID(int _i) { return tiles[_i]; }

			int GetDotCount();

		private:

			std::vector<int> tiles{ std::vector<int>() };
			Texture2D tilesAtlas;

			int ColorID{ 0 };

		};

		// Player Projectile Attack Class
		class Projectile
		{

		public:

			Projectile(Board* _board, Vector2 _pos, Vector2 _dir, int _dirIndex)
			{

				board = _board;

				pos = _pos;
				dir = _dir;

				animAtlas = board->GetTexture(9);
				dirIndex = _dirIndex;

			}

			void Update()
			{

				// Update Position
				float deltaTime = board->GetDeltaTime();
				pos = Vector2{ pos.x + dir.x * deltaTime * v, pos.y + dir.y * deltaTime * v };

				// Check If Out Of Bounds
				if (pos.x < -1 || pos.x > 19 || pos.y < -1 || pos.y > 22) { outOfBounds = true; }

				// Update Animation
				animDelay += deltaTime;
				if (animDelay >= 0.1f)
				{

					animIndex++;
					animIndex %= 3;

					animDelay -= 0.1f;

				}

			}

			void OnDraw() { DrawTextureRec(animAtlas, Rectangle{ (float)(16 * animIndex), (float)(16 * dirIndex), 16, 16 }, Vector2{ 16 * pos.x, 16 * pos.y + 48 }, WHITE); }

			bool IsOutOfBounds() { return outOfBounds; }

			Vector2 GetPos() { return pos; }

		private:

			Board* board;

			float v{ 15 };
			Vector2 pos;
			Vector2 dir;
			bool outOfBounds{ false };

			Texture2D animAtlas;
			int dirIndex;
			int animIndex{ 0 };
			float animDelay{ 0 };

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
			virtual void OnRespawn() = 0;
			void Respawn()
			{
				Spawn();
				OnRespawn();
			}

			Vector2Int coords;
			int dirIndex;
			Vector2Int DirVec(int _i);

			float rawDistanceTo(Vector2 _target)
			{
				Vector2 rawCoords = GetRawCoords();
				Vector2 diff = Vector2{ _target.x - rawCoords.x, _target.y - rawCoords.y };
				return sqrt(powf(diff.x, 2) + powf(diff.y, 2));
			}

			Vector2 GetRawCoords() { return Vector2{ TrueMod(coords.x + DirVec(dirIndex).x * stepTimer, 19), TrueMod(coords.y + DirVec(dirIndex).y * stepTimer, 22) }; }

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

			Player(Board* _board, Grid* _grid, int _item1, int _item2, int _item3)
			{

				board = _board;
				grid = _grid;

				mainAnimAtlas.atlas = board->GetTexture(5);
				mainAnimAtlas.tileSize = Vector2{ 24, 24 };

				swordAnimAtlas.atlas = board->GetTexture(6);
				swordAnimAtlas.tileSize = Vector2{ 16, 16 };

				dashAnimAtlas.atlas = board->GetTexture(7);
				dashAnimAtlas.tileSize = Vector2{ 16, 16 };

				magicAnimAtlas.atlas = board->GetTexture(8);
				magicAnimAtlas.tileSize = Vector2{ 18, 52 };

				items[0] = (ItemStates)_item1;
				items[1] = (ItemStates)_item2;
				items[2] = (ItemStates)_item3;

			}

			void Spawn()
			{
				Entity::Spawn();

				currentState = None;
				stateTimer = 0;
			}

			void OnRespawn() {}

			void Update();
			void OnDraw();

			void UpdateItems();
			void UpdateMovement();

			int ReturnEnemyCollisionOutcome() { return Dashing() ? 2 : currentState == Magic || TimeFrozen() ? 1 : 0; }

			int GetItemIndex(int _i) { return (int)items[_i]; }
			bool UsingItem() { return currentState != None; }
			bool SwingingSword() { return currentState == Sword; }
			bool Dashing() { return currentState == Dash || currentState == ElectricDash; }
			bool TimeFrozen() { return currentState == Freeze; }

			Vector2 GetSwordCoords() { return Vector2{ GetRawCoords().x + DirVec(dirIndex).x * 0.75f, GetRawCoords().y + DirVec(dirIndex).y * 0.75f }; }

			int GetEnergy() { return energy; }
			float GetElectricCharge() { return electricEnergy; }

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
				case ElectricDash: return dashAnimAtlas;

				case Magic: return stateTimer > 1 || (int)ceilf(stateTimer * 8) % 2 == 0 ? magicAnimAtlas : mainAnimAtlas;

				default: return mainAnimAtlas;

				}

			}

			void ChangeDir();
			void BoostTileCheck();

			enum ItemStates { None, Sword, Dash, Magic, Projectile, Trade, Freeze, ElectricDash };
			ItemStates currentState{ None };
			float stateTimer{ 0 };

			ItemStates items[3]{ None, None, None };

			int energy{ 0 };
			float electricEnergy{ 0 };

		};

		// Abstract Class For Enemies
		class Enemy : public Entity
		{

		public:

			virtual void Update() = 0;
			virtual void OnDraw() = 0;

			virtual void ChangeDir() = 0;
			virtual void OnStepFinished() = 0;

			virtual void OnStun() {}

			bool GetRemoveFlag() { return removeFlag; }

			bool removeFlag{ false };
			void SetRemoveFlag() { removeFlag = true; }

			float GetDeltaTime() { return board->GetEnemyDeltaTime(); }

			struct PossibleDirection
			{
				int dirIndex;
				float targetDistance;
			};
			Vector2Int GetTarget(int _i);
			std::vector<PossibleDirection> GetPossibleDirections(Vector2Int _targetPos, bool _UTurnsAllowed = false);

			void UpdateMovement();

			void PlayerCollisionCheck();
			void ProjectileCollisionCheck();

		};

		// Lurker Enemy Class
		class Lurker : public Enemy
		{

		public:

			Lurker(Board* _board, Grid* _grid)
			{

				board = _board;
				grid = _grid;

				wakeAnimAtlas = board->GetTexture(10);
				followAnimAtlas = board->GetTexture(11);
				sleepAnimAtlas = board->GetTexture(12);

			}

			void Update()
			{

				float deltaTime = GetDeltaTime();

				switch (currentState)
				{

				case Asleep:

					if (rawDistanceTo(board->GetPlayerRawPos()) < 1.25f) { currentState = WakingUp; }

					break;

				case WakingUp:
				case FallingAsleep:

					stateTimer += deltaTime;

					float delay;
					delay = stateCounter == 4 && currentState == WakingUp ? 0.4f : 0.05f;

					if (stateTimer >= delay)
					{

						stateCounter++;
						if (stateCounter == 5) { if (currentState == WakingUp) { ChangeDir(); currentState = Lurking; } else { SetRemoveFlag(); } }

						stateTimer -= delay;

					}

					break;

				case Lurking:

					UpdateMovement();

					break;

				}

			}

			void OnDraw()
			{
				switch (currentState)
				{
				case Asleep: DrawCurrentFrame(wakeAnimAtlas, 0, TileSize()); break;
				case WakingUp: DrawCurrentFrame(wakeAnimAtlas, stateCounter, TileSize()); break;
				case Lurking: DrawCurrentFrame(followAnimAtlas, 0, TileSize()); break;
				case FallingAsleep: DrawCurrentFrame(sleepAnimAtlas, stateCounter, TileSize()); break;
				}
			}

			void OnStun()
			{
				currentState = FallingAsleep;
				stateCounter = 0;
				stateTimer = 0;
			}

		private:

			void ChangeDir()
			{
				dirIndex = GetPossibleDirections(GetTarget(0), currentState == WakingUp)[0].dirIndex;
				Entity::ChangeDir();
			}

			void OnStepFinished() {}

			void OnRespawn() { OnStun(); }

			enum States { Asleep, WakingUp, Lurking, FallingAsleep };
			States currentState{ Asleep };
			float stateTimer{ 0 };
			int stateCounter{ 0 };

			Texture2D wakeAnimAtlas;
			Texture2D followAnimAtlas;
			Texture2D sleepAnimAtlas;

			Vector2 TileSize() { return Vector2{ 14, 10 }; }

		};

		// Clockwise Miranda Enemy Class
		class Clock : public Enemy
		{

		public:

			Clock(Board* _board, Grid* _grid, bool _counterClock)
			{

				board = _board;
				grid = _grid;

				counterClock = _counterClock;

				mainAnimAtlas = board->GetTexture(counterClock ? 15 : 13);
				stunAnimAtlas = board->GetTexture(counterClock ? 16 : 14);

			}

			void Update()
			{

				float deltaTime = GetDeltaTime();

				switch (currentState)
				{

				case Walking:

					UpdateMovement();

					break;

				case Stunned:

					// Update Animation
					animDelay += deltaTime;
					if (animDelay >= 0.075f)
					{
						animIndex++;
						animIndex %= 4;
						animDelay -= 0.075f;
					}

					// Update State Timer
					stateTimer += deltaTime;
					if (stateTimer > 0.75f)
					{
						currentState = Walking;
						animIndex = 0;
						stateTimer -= 0.75f;
					}

					break;

				}

			}

			void OnDraw() { DrawCurrentFrame(currentState == Stunned ? stunAnimAtlas : mainAnimAtlas, animIndex, TileSize()); }

			void OnStun()
			{
				currentState = Stunned;
				animIndex = 0;
				stateTimer = 0;
			}

		private:

			// Rotate Clockwise
			void ChangeDir()
			{
				int targetDir = (int)TrueMod(dirIndex + (counterClock ? -1 : 1), 4);
				if (IsValidDir(coords, targetDir)) { dirIndex = targetDir; }
				Entity::ChangeDir();
			}

			void OnStepFinished() {}

			void OnRespawn() {}

			bool counterClock{ false };

			enum States { Walking, Stunned };
			States currentState{ Walking };
			float stateTimer{ 0 };

			Texture2D mainAnimAtlas;
			Texture2D stunAnimAtlas;

			Vector2 TileSize() { return Vector2{ 20, 52 }; }

		};

		Board(Resources* _res, int _difficulty, int _item1, int _item2, int _item3, int _charm);
		~Board();

		void Update();
		void OnDraw();

		void ClearTile(int _i);
		void AddScore(int _s);
		void DotCollected();
		void ShootProjectile(Vector2 _pos, Vector2 _dir, int _dirIndex);
		void OnPlayerHit();

		float GetDeltaTime() { return GetRawDeltaTime() * (1 + 0.1f * speedMod); }
		float GetEnemyDeltaTime() { return player.TimeFrozen() ? 0 : GetDeltaTime(); }

		Entity::Vector2Int GetPlayerPos() { return player.coords; }
		Vector2 GetPlayerRawPos() { return player.GetRawCoords(); }
		int GetPlayerDirIndex() { return player.dirIndex; }
		int GetPlayerHitOutcome() { return player.ReturnEnemyCollisionOutcome(); }
		std::vector<Vector2> GetPlayerProjectiles()
		{
			std::vector<Vector2> returnVec = std::vector<Vector2>();
			for (int i = 0; i < playerProjectiles.size(); i++) { returnVec.push_back(playerProjectiles[i].GetPos()); }
			if (player.SwingingSword()) { returnVec.push_back(player.GetSwordCoords()); }
			if (player.Dashing()) { returnVec.push_back(player.GetRawCoords()); }
			return returnVec;
		}

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
		void RemoveEnemy(int _i)
		{
			delete enemies[_i];
			enemies.erase(std::next(enemies.begin(), _i));
		}
		void RemoveAllEnemies() { for (int i = enemies.size() - 1; i >= 0; i--) { RemoveEnemy(i); } }

		int difficulty{ 0 };
		int clearedRounds{ -1 };

		float timeLeft{ 180 };

		int score{ 0 };
		int speedScore{ 0 };
		int speedMod{ 0 };

		int dotsCollected{ 0 };
		int dotGoal{ 0 };

		int lifesLeft{ 99 };
		int crystalsLeft{ 99 };

		Grid grid;
		Player player;
		std::vector<Enemy*> enemies{ std::vector<Enemy*>() };
		std::vector<Projectile> playerProjectiles{ std::vector<Projectile>() };

		enum States { Starting, Playing, Paused, FlipOut, FlipIn, Defeated, GameOver, TimeUp, Results };
		States currentState{ Starting };
		int stepCounter{ 0 };
		float stepTimer{ 0 };

		Texture2D itemIcons;
		Texture2D charmIcons;
		Texture2D uiIcons;
		Texture2D roundCounter;

		int shaderFlipValLocation{ -1 };

		std::string GetTimerDisplayValue()
		{

			int intTimerVal;

			// X.XX
			if (timeLeft < 10)
			{
				intTimerVal = (int)ceil(timeLeft * 100);
				return TextFormat("%i.%i%i", (intTimerVal / 100) % 10, (intTimerVal / 10) % 10, intTimerVal % 10);
			}

			// XX.X
			else if (timeLeft < 60)
			{
				intTimerVal = (int)ceil(timeLeft * 10);
				return TextFormat("%i%i.%i", (intTimerVal / 100) % 6, (intTimerVal / 10) % 10, intTimerVal % 10);
			}

			// X:XX
			else
			{
				intTimerVal = (int)ceil(timeLeft);
				return TextFormat("%i:%i%i", (intTimerVal / 60) % 10, (intTimerVal / 10) % 6, intTimerVal % 10);
			}

		}

	};

}