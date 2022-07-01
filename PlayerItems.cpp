#include "Maze.h"

using namespace PacMan_Board;

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

	private:

		int price{ 0 };

		bool isElectric{ false };
		int manaCost{ 0 };
		float dischargeRate{ 0.0f };

	};

	PlayerItem GetItem(int _i)
	{

		PlayerItem Items[8]
		{

			// None
			PlayerItem{ 0, 0 },

			// Sword
			PlayerItem{ 0, 0 },

			// None
			PlayerItem{ 0, 0 },

			// Sword
			PlayerItem{ 0, 0 },

			// None
			PlayerItem{ 0, 0 },

			// Sword
			PlayerItem{ 0, 0 },

			// Freeze
			PlayerItem{ 0, 0.0f },

			// Boost
			PlayerItem{ 0, 0.0f }

		};

		return Items[_i];

	}

	int GetCharmPrice(int _i)
	{

		int CharmPrices[8]
		{

			// None
			0,

			// Score x3
			0,

			// Lifes x3
			0,

			// Crystals x3
			0,

			// Score x4
			0,

			// Lifes x5
			0,

			// Crystals x5
			0,

			// Locket
			0,

		};

		return CharmPrices[_i];

	}

}