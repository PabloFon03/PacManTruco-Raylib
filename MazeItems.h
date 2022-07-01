#pragma once

#include "Maze.h"

namespace PacMan_Board
{

	namespace Items
	{

		PlayerItem GetItem(int _i)
		{

			PlayerItem Items[8]
			{

				// None
				PlayerItem{ 0, 0 },

				// Sword
				PlayerItem{ 1, 2 },

				// Jump
				PlayerItem{ 3, 0 },

				// Dash
				PlayerItem{ 5, 0 },

				// Magic
				PlayerItem{ 8, 0 },

				// Projectile
				PlayerItem{ 10, 0 },

				// Freeze
				PlayerItem{ 12, 0.2f },

				// Boost
				PlayerItem{ 15, 0.5f }

			};

			return Items[_i];

		}

		int GetCharmPrice(int _i)
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
				25,

			};

			return CharmPrices[_i];

		}

	}

}