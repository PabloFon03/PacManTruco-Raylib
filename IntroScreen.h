#pragma once

#pragma once

#include "SubScreen.h"

namespace Intro_Screen
{

	class IntroScreen : public SubScreen
	{

	public:

		IntroScreen(Resources* _res) { resources = _res; }

		float GetDeltaTime() { return GetRawDeltaTime(); }

		void Update()
		{

			if (IsKeyPressed(KEY_Z)) { exitFlag = 0; }

			switch (currentState)
			{

			case BlankScreen:
			{
				stepTimer += GetDeltaTime();
				if (stepTimer >= 2)
				{
					if (stepCounter == 0) { currentState = GridShow; }
					else { exitFlag = 0; }
					stepTimer -= 2;
				}
				break;
			}

			case GridShow:
			{
				stepTimer += GetDeltaTime();
				if (stepTimer >= 0.75f)
				{
					currentState = GridFadeIn;
					stepTimer -= 0.75f;
				}
				break;
			}

			case GridFadeIn:
			case GridFadeOut:
			{
				stepTimer += GetDeltaTime();
				if (stepTimer >= 0.02f)
				{
					stepCounter++;
					if (stepCounter >= 50)
					{
						stepCounter = currentState == GridFadeIn ? 0 : 1;
						currentState = currentState == GridFadeIn ? GridFadeOut : BlankScreen;
					}
					stepTimer -= 0.02f;
				}
				break;
			}

			}

		}

		void OnDraw()
		{

			switch (currentState)
			{

			case GridShow:
			{
				for (int y = 0; y < 28; y++) { for (int x = 0; x < 19; x++) { DrawGridTile(x, y); } }
				break;
			}

			case GridFadeIn:
			case GridFadeOut:
			{
				for (int y = 0; y < 28; y++) { for (int x = 0; x < 19; x++) { if ((x + y < stepCounter) != (currentState == GridFadeOut)) { DrawGridTile(x, y); } } }
				break;
			}

			}

		}

	private:

		void DrawGridTile(int x, int y)
		{
			DrawRectangle(16 * x, 16 * y, 16, 16, WHITE);
			DrawRectangle(16 * x + 1, 16 * y + 1, 14, 14, BLACK);
		}

		enum States { BlankScreen, GridShow, GridFadeIn, GridFadeOut };
		States currentState{ BlankScreen };
		int stepCounter{ 0 };
		float stepTimer{ 0 };

	};

}