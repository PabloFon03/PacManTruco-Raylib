#pragma once

#include "Resources.h"

// Abstract Class For Screen Contents
class SubScreen
{

public:

	virtual void Update() = 0;
	virtual void OnDraw() = 0;

	float GetRawDeltaTime() { return GetFrameTime() < 0.15f ? GetFrameTime() : 0.15f; }

	virtual int GetTokens() { return 0; }

	int exitFlag{ -1 };

	Resources* resources;

	Texture2D GetCommonTexture(int _i) { return (*resources).GetCommonTexture(_i); }

	std::vector<int> GetCharIndexes(std::string _text)
	{

		std::vector<int> returnVec = std::vector<int>();

		char allValidChars[82]
		{
			' ', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
			'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
			'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
			'.', ',', ':', ';', '-', '"', '#', '$', '%', '&', '@', '?', '!', '(', ')', '[', ']', '<', '>'
		};

		for (int i = 0; i < _text.size(); i++)
		{

			int charIndex = 83;

			for (int j = 0; charIndex == 83 && j < 82; j++) { if (_text[i] == allValidChars[j]) { charIndex = j; } }

			returnVec.push_back(charIndex);

		}

		return returnVec;

	}

	void DrawCharIndexes(std::vector<int> _charIndexes, Vector2 _startPos, Color _tint = WHITE, int _drawType = 0)
	{

		if (_drawType > 0) { _startPos = Vector2{ _startPos.x - 8 * (_charIndexes.size() / (_drawType == 1 ? 2.0f : 1.0f)), _startPos.y }; }

		for (int i = 0; i < _charIndexes.size(); i++) { DrawTextureRec(GetCommonTexture(0), Rectangle{ _charIndexes[i] * 8.0f, 0, 8, 8 }, Vector2{ _startPos.x + 8 * i, _startPos.y }, _tint); }

	}

	void DrawTextCharAtlas(std::string _text, Vector2 _startPos, Color _tint = WHITE, int _drawType = 0) { DrawCharIndexes(GetCharIndexes(_text), _startPos, _tint, _drawType); }

	void DrawBox(int _boxType, Vector2 _size, Vector2 _centerPos, Color _tint = WHITE)
	{

		Vector2 drawPos = Vector2{ _centerPos.x - (_size.x / 2.0f), _centerPos.y - (_size.y / 2.0f) };

		Texture2D boxTex = GetCommonTexture(_boxType + 3);

		for (int y = -1; y <= _size.y; y++)
		{

			for (int x = -1; x <= _size.x; x++)
			{

				// Deafult To Center
				Rectangle cutRec = Rectangle{ 4, 4, 1, 1 };

				// Top Border
				if (y == -1)
				{

					cutRec.y = 0;

					cutRec.height = 4;

				}

				// Bottom Border
				else if (y == _size.y)
				{

					cutRec.y = 5;

					cutRec.height = 4;

				}

				// Left Border
				if (x == -1)
				{

					cutRec.x = 0;

					cutRec.width = 4;

				}

				// Right Border
				else if (x == _size.x)
				{

					cutRec.x = 5;

					cutRec.width = 4;

				}

				DrawTextureRec(boxTex, cutRec, Vector2{ drawPos.x + (x == -1 ? -4 : x), drawPos.y + (y == -1 ? -4 : y) }, _tint);

			}

		}

	}

};