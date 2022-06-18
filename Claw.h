#pragma once

#pragma once

#include<string>
#include<vector>
#include<raylib.h>

#include "SubScreen.h"

namespace Claw_Board
{

	// Board Class
	class Board : public SubScreen
	{

	public:

		// Abstract Class For Player And Targets
		class Entity
		{

		public:

			Board* board;

			Vector2 pos = Vector2{ 0, 0 };
			Vector2 zeroPoint = Vector2{ 152, 440 };

			virtual Vector2 TileSize() = 0;
			Vector2 GetDrawPos(Vector2 _pos) { return Vector2{ _pos.x * 16 - TileSize().x / 2 + zeroPoint.x, zeroPoint.y - _pos.y * 16 - TileSize().y / 2 }; }

			virtual void Update() = 0;
			virtual void OnDraw() = 0;

		};

		// Player Class
		class Player : public Entity
		{

		public:

			Player();
			Player(Board* _arena);

			void Update();
			void OnDraw();

			Vector2 TileSize() { return Vector2{ 48, 32 }; }

		private:

			Texture2D ropeTile;
			Texture2D animAtlas;

		};

		// Baseball Ball Class
		class Target : public Entity
		{

		public:

			Target();
			Target(Board* _arena, int _ID, Vector2 _startPos, float _speed);

			void Update();
			void OnDraw();

			Vector2 TileSize() { return Vector2{ 16, 16 }; }

		private:

			int ID{ -1 };

			float speed;

			Texture2D animAtlas;
			int animIndex{ 0 };
			float animTimer{ 0 };

			int exitCode{ 0 };

		};

		Board(Resources* _res, int _difficulty);

		void Start();
		void Update();
		void OnDraw();

		void TokenGrabbed(int _ID);

		Vector2 GetPlayerPos() { return player.pos; }

		float GetDeltaTime() { return GetRawDeltaTime(); }

		Texture2D GetTexture(int _i) { return (*resources).GetClawTexture(_i); }

	private:

		int difficulty{ 0 };
		int roundCounter{ 0 };
		int score{ 0 };

		struct Row
		{

			// Color ID
			int ID;

			// Y-Coord
			int y;

		};

		std::vector<Row> rows{ std::vector<Row>() };

		Player player;
		std::vector<Target> targets{ std::vector<Target>() };

		enum States { Starting, Playing, Retrieving, Results };
		States currentState{ Starting };
		int stepCounter{ 0 };
		float stepTimer{ 0 };

	};

}