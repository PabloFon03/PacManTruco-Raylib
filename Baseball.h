#pragma once

#include<string>
#include<vector>
#include<raylib.h>

#include "SubScreen.h"

namespace Baseball_Arena
{

	// Board Class
	class Arena : public SubScreen
	{

	public:

		// Abstract Class For Player And Balls
		class Entity
		{

		public:

			Arena* arena;

			Vector2 pos{ Vector2{ 0, 0 } };
			Vector2 v{ Vector2{ 0, 0 } };

			Vector2 zeroPoint = Vector2{ 48, 256 };

			virtual Vector2 TileSize() = 0;
			Vector2 GetDrawPos(Vector2 _pos) { return Vector2{ _pos.x * 16 - TileSize().x / 2 + zeroPoint.x, zeroPoint.y - _pos.y * 16 - TileSize().y / 2 }; }

			void Move(float _Dt) { pos = Vector2{ pos.x + v.x * _Dt, pos.y + v.y * _Dt }; }

			virtual void Update() = 0;
			virtual void OnDraw() = 0;

		};

		// Player Class
		class Player : public Entity
		{

		public:

			Player();
			Player(Arena* _arena);

			void Update();
			void OnDraw();

			bool isOnGround() { return onGround; }

			Vector2 TileSize() { return Vector2{ 16, 16 }; }

		private:

			Texture2D baseAnimAtlas;
			int baseAnimIndex{ 0 };
			float baseAnimTimer{ 0 };

			Texture2D ballAnimAtlas;
			int ballAnimIndex{ 0 };
			float ballAnimTimer{ 0 };

			bool onGround{ true };

		};

		// Baseball Ball Class
		class Ball : public Entity
		{

		public:

			Ball();
			Ball(Arena* _arena, int _ID);

			void Update();
			void OnDraw();

			Vector2 TileSize() { return Vector2{ 8, 8 }; }

			int GetExitCode() { return exitCode; }

		private:

			int ID{ -1 };

			int stepCounter{ 0 };
			float stepTimer{ 0 };

			Texture2D animAtlas;
			int animIndex{ 0 };
			float animTimer{ 0 };

			int exitCode{ 0 };

		};

		// Ball Spawn Data Class
		class SpawnData
		{

		public:

			SpawnData() {};

			SpawnData(int _ID, float _delay) { ballID = _ID; delay = _delay; }

			int GetID() { return ballID; }

			float GetDelay() { return delay; }

		private:

			int ballID{ -1 };

			float delay{ 0 };

		};

		Arena(Resources* _res, int _difficulty);

		void Start();
		void Update();
		void OnDraw();

		void OnBallHit()
		{

			// Register Hit
			hits++;

			// Add Score
			score += 50 + 25 * speedMod;

		}

		Vector2 GetPlayerPos() { return player.pos; }

		float GetDeltaTime() { return GetRawDeltaTime() * (1 + 0.1f * speedMod); }

		Texture2D GetTexture(int _i) { return (*resources).GetBaseballTexture(_i); }

		int GetTokens() { return (int)ceilf(score / 150.0f) + (totalMisses == 0 ? GetBonus() : 0); }
		int GetBonus() { return 25 * (int)powf(2, difficulty); }

	private:

		int difficulty{ 0 };
		int clearedRounds{ 0 };

		int score{ 0 };
		int speedMod{ 0 };

		int hits{ 0 };
		int misses{ 0 };
		int totalMisses{ 0 };

		Player player;
		std::vector<Ball> balls{ std::vector<Ball>(0) };

		std::vector<std::vector<SpawnData>> roundSpawns{ std::vector<std::vector<SpawnData>>() };
		float spawnDelay{ 0 };

		int pitcherAnimIndex{ 0 };
		float pitcherAnimTimer{ 0 };

		enum States { Starting, Playing, GameOver, Results };
		States currentState{ Playing };
		int stepCounter{ 0 };
		float stepTimer{ 0 };

	};

}