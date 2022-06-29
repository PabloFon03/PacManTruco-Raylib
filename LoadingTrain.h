#pragma once

#include "SubScreen.h"
#include "Random.h"

namespace Loading_Screen
{

	// Board Class
	class LoadScreen : public SubScreen
	{

	public:

		LoadScreen(Resources* _res, int _exitFlag)
		{

			resources = _res;

			Random r = Random();

			targetExitFlag = _exitFlag;

			Color tints[8]
			{

				// White
				WHITE,

				// Pink
				Color{ 255, 96, 216, 255 },

				// Crimson
				Color{ 255, 16, 96, 255 },

				// Orange
				Color{ 255, 160, 32, 255 },

				// Yellow
				Color{ 255, 196, 32, 255 },

				// Green
				Color{ 32, 255, 64, 255 },

				// Blue
				Color{ 64, 208, 255, 255 },

				// Purple
				Color{ 200, 32, 255, 255 }

			};
			tint = tints[r.RandomInt(8)];

			// Set Random Wagon Amount
			Train.wagons = r.RandomInt(2, 15);

			// Stop On Random Wagon
			Train.stopWagon = r.RandomInt(Train.wagons - 1) + 1;

			// Set Start Speed
			Train.v = 3;

			// Start Offscreen
			Train.x = -r.RandomInt(5, 12);

			// Set End X-Pos
			Train.endX = Train.wagons + r.RandomInt(10, 20);

			currentState = StartDelay;
			stepTimer = 3;

		}

		float GetDeltaTime() { return GetRawDeltaTime(); }

		void Update()
		{

			switch (currentState)
			{

			case StartDelay:

				stepTimer -= GetDeltaTime();

				if (stepTimer <= 0) { currentState = StoppingTrain; }

				break;

			case StoppingTrain:

				Train.x += Train.v * GetDeltaTime();

				if (Train.x >= Train.stopWagon - 2 && Train.v > 0)
				{

					Train.v -= 2.25f * GetDeltaTime();

					if (Train.v <= 0)
					{

						Train.v = 0;
						Train.x = Train.stopWagon;

						currentState = TrainStopped;
						stepCounter = 0;
						stepTimer = 2;

					}

				}

				break;

			case TrainStopped:

				stepTimer -= GetDeltaTime();

				if (stepTimer <= 0)
				{

					stepCounter++;

					if (stepCounter == 4) { currentState = TrainLeaving; }

					else { stepTimer += stepCounter == 1 ? 0.75f : 2; }

				}

				break;

			case TrainLeaving:

				if (Train.v < 3)
				{

					Train.v += 1.5f * GetDeltaTime();

					if (Train.v >= 3) { Train.v = 3; }

				}

				Train.x += Train.v * GetDeltaTime();

				if (Train.x >= Train.endX)
				{

					currentState = EndDelay;

					stepTimer = 3;

				}

				break;

			case EndDelay:

				stepTimer -= GetDeltaTime();

				if (stepTimer <= 0) { exitFlag = targetExitFlag; }

				break;

			}

		}

		Texture2D GetTexture(int _i) { return (*resources).GetLoadingTrainTexture(_i); }

		std::string GetDestinationText()
		{

			switch (targetExitFlag)
			{

			case 2:
			case 3:
			case 4:
				return "Baseball Arena";

			case 5:
			case 6:
			case 7:
				return "Claw Machine";

			default: return TextFormat("Undefined (Exit Flag: %i)", targetExitFlag);

			}

		}

		void OnDraw()
		{

			if (currentState != StartDelay && currentState != EndDelay)
			{

				// Draw Train Stop
				DrawTextureRec(GetTexture(0), Rectangle{ 0, 0, 80, 64 }, Vector2{ 112, 232 }, tint);

				// Draw Train Wagons
				for (int i = 0; i < Train.wagons; i++) { DrawTextureRec(GetTexture(i == 0 ? 1 : 2), Rectangle{ 0, 0, 64, 40 }, Vector2{ 120 + 64 * (Train.x - i), 256 }, tint); }

			}

			if (currentState == TrainStopped && stepCounter > 0 && stepCounter < 3) { for (int i = 1; i <= stepCounter; i++) { DrawTextCharAtlas(i == 1 ? "Next Stop:" : GetDestinationText(), Vector2{ 152, (float)(i == 1 ? 52 : 64) }, tint, 1); } }

		}

	private:

		int targetExitFlag{ 0 };

		struct
		{

			// Amount Of Wagons
			int wagons;

			// Wagon To Stop On
			int stopWagon;

			// Train Position
			float x;

			// Train Speed
			float v;

			// Fade-Out X-Pos Threshold
			int endX;

		}
		Train;

		Color tint;

		enum States { StartDelay, StoppingTrain, TrainStopped, TrainLeaving, EndDelay };
		States currentState{ StartDelay };
		int stepCounter{ 0 };
		float stepTimer{ 0 };

	};

}