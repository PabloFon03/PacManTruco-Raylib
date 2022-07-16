#pragma once

#include "SubScreen.h"

namespace Error_Screen
{

	class ErrorScreen : public SubScreen
	{

	public:

		ErrorScreen(Resources* _res, int _errorExitFlag)
		{
			resources = _res;
			errorExitFlag = _errorExitFlag;
		}

		float GetDeltaTime() { return GetRawDeltaTime(); }

		void Update()
		{
			if (quitDelay < 3) { quitDelay += GetDeltaTime(); }
			else if (IsKeyPressed(GetKeyPressed())) { exitFlag = -2; }
		}

		void OnDraw()
		{

			// Draw Error Banner
			DrawBox(8, Vector2{ 64, 24 }, Vector2{ 152, 128 }, Color{ 255, 25, 63, 255 });
			DrawTextCharAtlas("ERROR", Vector2{ 152, 124 }, Color{ 255, 25, 63, 255 }, 1);

			// Draw Exit Code Banner
			DrawBox(8, Vector2{ 192, 48 }, Vector2{ 152, 224 }, Color{ 200, 200, 200, 255 });
			DrawTextCharAtlas("Undefined Exit Flag:", Vector2{ 152, 212 }, Color{ 225, 225, 225, 255 }, 1);
			DrawTextCharAtlas(std::to_string(errorExitFlag), Vector2{ 152, 228 }, Color{ 225, 225, 225, 255 }, 1);

			// Draw Quit Banner
			if (quitDelay >= 3)
			{
				DrawBox(8, Vector2{ 192, 24 }, Vector2{ 152, 320 }, Color{ 200, 200, 200, 255 });
				DrawTextCharAtlas("Press Any Key To Quit", Vector2{ 152, 316 }, Color{ 225, 225, 225, 255 }, 1);
			}

		}

	private:

		int errorExitFlag{ 0 };
		float quitDelay{ 0 };

	};

}