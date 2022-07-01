#pragma once

#include "SubScreen.h"
#include "Random.h"

#include "MazeItems.h"

namespace ItemShop_Screen
{

	class ItemShop : public SubScreen
	{

	public:

		ItemShop(Resources* _res, int _boardDifficulty, int _tokenAmount)
		{

			resources = _res;

			ItemIcons = resources->GetMazeTexture(0);
			CharmIcons = resources->GetMazeTexture(1);

			TokenAnim.animAtlas = resources->GetCommonTexture(1);
			Random r = Random();
			TokenAnim.colorID = r.RandomInt(5);

			difficulty = _boardDifficulty;

			tokenAmount = _tokenAmount;

		}

		int GetTokens() { return GetRemainingTokens() - tokenAmount; }

		float GetDeltaTime() { return GetRawDeltaTime(); }

		float Lerp(float _a, float _b, float _n) { return (_b - _a) * _n + _a; }

		void Update()
		{

			TokenAnim.animDelay -= GetDeltaTime();
			if (TokenAnim.animDelay <= 0)
			{

				TokenAnim.animIndex++;
				TokenAnim.animIndex %= 4;

				TokenAnim.animDelay += TokenAnim.animIndex == 0 ? 1 : 0.1f;

			}

			// Scroll Left
			if (IsKeyPressed(KEY_LEFT))
			{

				cursorIndexes[currentSection]--;
				if (cursorIndexes[currentSection] < 0) { cursorIndexes[currentSection] = 7; }

			}

			// Scroll Right
			if (IsKeyPressed(KEY_RIGHT))
			{

				cursorIndexes[currentSection]++;
				if (cursorIndexes[currentSection] > 7) { cursorIndexes[currentSection] = 0; }

			}

			// Move Up
			if (IsKeyPressed(KEY_UP))
			{

				currentSection--;
				if (currentSection < 0) { currentSection = 3; }

			}

			// Move Down
			if (IsKeyPressed(KEY_DOWN))
			{

				currentSection++;
				if (currentSection > 3) { currentSection = 0; }

			}

			// Purchase Item
			if (IsKeyPressed(KEY_Z))
			{

				int itemIndex = cursorIndexes[currentSection];

				bool isValidPurchase = true;

				// Funds Check
				isValidPurchase = GetRemainingTokens() + GetItemPrice(currentSection, itemIndexes[currentSection]) >= GetItemPrice(currentSection, itemIndex);

				// One-Time Purchase Check
				for (int i = 0; isValidPurchase && i < 4; i++) { if (currentSection < 3 == i < 3) { isValidPurchase = itemIndexes[i] != itemIndex; } }

				if (isValidPurchase || itemIndex == 0) { itemIndexes[currentSection] = itemIndex; }

			}

			// Back To Menu
			if (IsKeyPressed(KEY_X)) { exitFlag = 40; }

			// Start Game
			if (IsKeyPressed(KEY_SPACE)) { exitFlag = 44 + difficulty; }

		}

		Texture2D GetTexture(int _i) { return (*resources).GetLoadingTrainTexture(_i); }

		void OnDraw()
		{

			// Draw Tokens Amount
			DrawTextureRec(TokenAnim.animAtlas, Rectangle{ 16 * (float)TokenAnim.animIndex, 16 * (float)TokenAnim.colorID, 16, 16 }, Vector2{ 16, 24 }, WHITE);
			DrawTextCharAtlas(TextFormat("%i", GetRemainingTokens()), Vector2{ 36, 28 });

			// Draw Items
			for (int i = 0; i < 4; i++)
			{

				// Draw Icon
				DrawTextureRec(i == 3 ? CharmIcons : ItemIcons, Rectangle{ (float)(16 * itemIndexes[i]), i == 3 ? 0 : (float)(16 * i), 16, 16 }, Vector2{ (float)(200 + 24 * i), 24 }, WHITE);

				// Draw Price
				DrawTextCharAtlas(TextFormat("%i", GetItemPrice(i, itemIndexes[i])), Vector2{ (float)(208 + 24 * i), 44 }, WHITE, 1);

			}

			// Draw Sections
			for (int i = 0; i < 4; i++)
			{

				Vector2 pivot = Vector2{ 152, (float)(114 + 96 * i) };

				DrawBox(4, Vector2{ 304, 92 }, pivot);

				Color BGColors[4]{ Color{ 127, 0, 33, 255 }, Color{ 0, 127, 31, 255 }, Color{ 0, 95, 127, 255 }, Color{ 95, 0, 127, 255 } };

				DrawRectangle(0, 68 + 96 * i, 304, 92, i == currentSection ? BGColors[i] : BLACK);

			}

			// Draw Current Section Items
			for (int i = 0; i < 8; i++)
			{

				float offset = 192 - 64 * cursorIndexes[currentSection];
				if (offset > 0) { offset = 0; }

				Color tint = i == cursorIndexes[currentSection] ? WHITE : GRAY;

				// Draw Card
				DrawBox(9, Vector2{ 40, 56 }, Vector2{ 56 + 64 * i + offset, (float)(114 + 96 * currentSection) }, tint);

				// Draw Icon
				DrawTextureRec(currentSection == 3 ? CharmIcons : ItemIcons, Rectangle{ (float)(16 * i), (float)(16 * currentSection), 16, 16 }, Vector2{ 48 + 64 * i + offset, (float)(98 + 96 * currentSection) }, tint);

				// Draw Price
				DrawTextCharAtlas(TextFormat("%i", GetItemPrice(currentSection, i)), Vector2{ 56 + 64 * i + offset, (float)(126 + 96 * currentSection) }, tint, 1);

			}

		}

	private:

		int GetItemPrice(int _section, int _index) { return _section == 3 ? PacMan_Board::Items::GetCharmPrice(_index) : PacMan_Board::Items::GetItem(_index).GetPrice(); }

		int GetRemainingTokens()
		{

			int remainingTokens = tokenAmount;

			for (int i = 0; i < 4; i++) { remainingTokens -= GetItemPrice(i, itemIndexes[i]); }

			return remainingTokens;

		}

		int tokenAmount{ 0 };

		int itemIndexes[4]{ 0, 0, 0, 0 };

		int currentSection{ 0 };
		int cursorIndexes[4]{ 0, 0, 0, 0 };

		Texture2D ItemIcons;
		Texture2D CharmIcons;

		struct
		{

			Texture2D animAtlas;

			int colorID{ 0 };

			int animIndex{ 0 };

			float animDelay{ 1 };

		}
		TokenAnim;

		int difficulty{ 0 };

		enum States { FirstFadeIn, GameQuit, GameFadeIn, GameChoice, GameFadeOut, DifficultyFadeIn, DifficultyChoice, DifficultyFadeOut, DifficultyQuit };
		States currentState{ FirstFadeIn };
		int stepCounter{ 0 };
		float stepTimer{ 0 };

	};

}