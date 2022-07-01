#pragma once

#include "SubScreen.h"
#include "Random.h"

namespace MainMenu_Screen
{

	class MainMenu : public SubScreen
	{

	public:

		MainMenu(Resources* _res, int _tokenAmount)
		{

			resources = _res;

			TokenAnim.animAtlas = resources->GetCommonTexture(1);

			Random r = Random();

			TokenAnim.colorID = r.RandomInt(5);

			tokenAmount = _tokenAmount;

		}

		float GetDeltaTime() { return GetRawDeltaTime(); }

		float Lerp(float _a, float _b, float _n) { return (_b - _a) * _n + _a; }

		void Update()
		{

			BGTimer += 1.5f * GetDeltaTime();
			if (BGTimer >= 2) { BGTimer -= 2; }

			TokenAnim.animDelay -= GetDeltaTime();
			if (TokenAnim.animDelay <= 0)
			{

				TokenAnim.animIndex++;
				TokenAnim.animIndex %= 4;

				TokenAnim.animDelay += TokenAnim.animIndex == 0 ? 1 : 0.1f;

			}

			switch (currentState)
			{

			case GameChoice:
			case DifficultyChoice:

				int* buttonIndex;
				buttonIndex = currentState == GameChoice ? &gameButtonIndex : &difficultyButtonIndex;

				if (IsKeyPressed(KEY_DOWN))
				{

					(*buttonIndex)++;

					if ((*buttonIndex) > 2) { (*buttonIndex) = 0; }

				}

				if (IsKeyPressed(KEY_UP))
				{

					(*buttonIndex)--;

					if ((*buttonIndex) < 0) { (*buttonIndex) = 2; }

				}

				if (IsKeyPressed(KEY_Z)) { currentState = currentState == GameChoice ? GameFadeOut : DifficultyQuit; }

				if (IsKeyPressed(KEY_X)) { currentState = currentState == GameChoice ? GameQuit : DifficultyFadeOut; }

				break;

			default:

				stepTimer += 2.5f * GetDeltaTime();

				if (stepTimer >= (currentState == DifficultyQuit ? 3 : 1))
				{

					stepTimer--;

					switch (currentState)
					{

					case GameQuit: exitFlag = -2; break;

					case FirstFadeIn: case GameFadeIn: currentState = GameChoice; break;

					case GameFadeOut: currentState = DifficultyFadeIn; break;

					case DifficultyFadeIn: currentState = DifficultyChoice; break;

					case DifficultyFadeOut: currentState = GameFadeIn; break;

					case DifficultyQuit: exitFlag = 3 * gameButtonIndex + difficultyButtonIndex + (gameButtonIndex == 0 ? 1 : 4) + 40; break;

					}

				}

				break;

			}

		}

		Texture2D GetTexture(int _i) { return (*resources).GetLoadingTrainTexture(_i); }

		void OnDraw()
		{

			if (exitFlag == -1)
			{

				for (int i = -2; i < 56; i++)
				{

					unsigned char v = i % 2 == 0 ? 10 : 25;

					DrawRectangle(0, 8 * (i + BGTimer), 304, 8, Color{ v, v, v, 255 });

				}

				for (int i = 0; i < 3; i++)
				{

					Vector2 pivot = Vector2{ 152, 224 + 64 * (float)(i - 1) };

					bool buttonsActive = currentState == GameChoice || currentState == DifficultyChoice;

					if (!buttonsActive)
					{

						// GameQuit, GameFadeIn, GameChoice, GameFadeOut, DifficultyFadeIn, DifficultyChoice, DifficultyFadeOut, DifficultyQuit

						bool fadingIn = currentState == FirstFadeIn || currentState == GameFadeIn || currentState == DifficultyFadeIn;
						bool rightSide = currentState == FirstFadeIn || currentState == DifficultyFadeIn || currentState == DifficultyFadeOut;

						pivot.x = Lerp(-128, 152, (fadingIn ? (rightSide ? 2 - stepTimer : stepTimer) : (rightSide ? 1 + stepTimer : (1 - stepTimer))));

					}

					int selectedButtonIndex = currentState == GameChoice ? gameButtonIndex : currentState == DifficultyChoice ? difficultyButtonIndex : -1;

					Color difficultyColors[3]{ Color{ 0, 255, 64, 255 }, Color{ 255, 192, 0, 255 }, Color{ 255, 0, 64, 255 } };
					Color c = i == selectedButtonIndex ? currentState == DifficultyChoice ? difficultyColors[i] : WHITE : Color{ 100, 100, 100, 255 };

					DrawBox(i == selectedButtonIndex ? 6 : 8, Vector2{ 96, 24 }, pivot, c);

					if (buttonsActive)
					{

						std::string buttonLabels[3]{ "Main Game", "Baseball", "Claw" };
						std::string difficultyLabels[3]{ "Easy", "Normal", "Hard" };

						DrawTextCharAtlas(currentState == GameChoice ? buttonLabels[i] : difficultyLabels[i], Vector2{ pivot.x, pivot.y - 4 }, c, 1);

					}

				}

				// Top And Bottom Borders
				for (int i = 0; i < 2; i++) { DrawBox(8, Vector2{ 304, 80 }, Vector2{ 152, (float)(40 + 368 * i) }, Color{ 128, 128, 128, 255 }); }

				// Draw Controls
				DrawTextCharAtlas("[Arrow Keys] - Move", Vector2{ 152, 24 }, WHITE, 1);
				DrawTextCharAtlas("[Z] - Accept", Vector2{ 152, 36 }, WHITE, 1);
				DrawTextCharAtlas("[X] - Back", Vector2{ 152, 48 }, WHITE, 1);

				// Draw Tokens Amount
				DrawTextureRec(TokenAnim.animAtlas, Rectangle{ 16 * (float)TokenAnim.animIndex, 16 * (float)TokenAnim.colorID, 16, 16 }, Vector2{ 144, 394 }, WHITE);
				DrawTextCharAtlas(TextFormat("%i", tokenAmount), Vector2{ 152, 414 }, WHITE, 1);

			}

		}

	private:

		int tokenAmount{ 0 };

		float BGTimer{ 0 };

		struct
		{

			Texture2D animAtlas;

			int colorID{ 0 };

			int animIndex{ 0 };

			float animDelay{ 1 };

		}
		TokenAnim;

		int gameButtonIndex{ 0 };
		int difficultyButtonIndex{ 0 };

		enum States { FirstFadeIn, GameQuit, GameFadeIn, GameChoice, GameFadeOut, DifficultyFadeIn, DifficultyChoice, DifficultyFadeOut, DifficultyQuit };
		States currentState{ FirstFadeIn };
		int stepCounter{ 0 };
		float stepTimer{ 0 };

	};

}