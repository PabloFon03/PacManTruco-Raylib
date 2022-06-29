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
			Vector2 TargetTileSize() { return Vector2{ 16, 16 }; }
			Vector2 NoTargetsTileSize() { return Vector2{ 136, 136 }; }

			void Drop();
			void Restart();

		private:

			Texture2D ropeTile;
			Texture2D animAtlas;
			Texture2D targetTiles;
			Texture2D noTargets;

			enum States { Playing, Grabbing, Retrieving };
			States currentState{ Playing };

			int grabbedTargetID{ -1 };

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

			int GetID() { return ID; }

		private:

			int ID{ -1 };

			float speed;

			Texture2D animAtlas;
			int animIndex{ 0 };
			float animTimer{ 0 };

			int exitCode{ 0 };

		};

		Board(Resources* _res, int _difficulty);

		void Update();
		void OnDraw();

		void TargetGrabbed(int _i);
		void TargetRetreived();

		std::vector<Vector2> GetTargetsPos()
		{
			
			std::vector<Vector2> returnVec = std::vector<Vector2>();

			for (int i = 0; i < targets.size(); i++) { returnVec.push_back(targets[i].pos); }

			return returnVec;
		
		}

		int GetTargetID(int _i) { return targets[_i].GetID(); }

		float GetDeltaTime() { return GetRawDeltaTime(); }

		Texture2D GetTexture(int _i) { return (*resources).GetClawTexture(_i); }

		int GetTokens() { return (int)ceilf(score / 10.0f) + (score >= bonusThreshold ? bonusTokens : 0); }

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

		enum States { Starting, Playing, Grabbing, Retrieving, Results };
		States currentState{ Starting };
		int stepCounter{ 0 };
		float stepTimer{ 0 };

		int bonusThreshold{ 500 };
		int bonusTokens{ 50 };

	};

}