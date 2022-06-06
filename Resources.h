#pragma once

#include<string>
#include<vector>

#include "raylib.h"

// Class For Common Resources
class Resources
{

public:

	Resources() { }

	// Unload Resources On Destruction
	~Resources() { Unload(); }

	void Load()
	{

		// Load Maze Texture Files
		LoadTextures(mazeTextures, std::vector<std::string>
		{
			"round counter.png",
				"sqweek.png",
				"mouse gal.png",
				"tsundere cheesecake.png",
				"alice.png",
				"orange.png"
		});

		// Load Baseball Texture Files
		LoadTextures(baseballTextures, std::vector<std::string>
		{
			"electric base.png",
				"electric ball.png",
				"baseballs.png",
				"pitcher.png",
				"pitcher platform.png",
				"player platform.png"
		});

	}

	void Unload()
	{

		// Unload Maze Textures
		for (int i = 0; i < mazeTextures.size(); i++) { UnloadTexture(mazeTextures[i]); }

		// Unload Baseball Textures
		for (int i = 0; i < baseballTextures.size(); i++) { UnloadTexture(baseballTextures[i]); }

	}

	Texture2D GetMazeTexture(int _i) { return mazeTextures[_i]; }

	Texture2D GetBaseballTexture(int _i) { return baseballTextures[_i]; }

private:

	std::vector<Texture2D> mazeTextures{ std::vector<Texture2D>() };
	std::vector<Texture2D> baseballTextures{ std::vector<Texture2D>() };

	void LoadTextures(std::vector<Texture2D>& _refVec, std::vector<std::string> _fileNames)
	{

		for (int i = 0; i < _fileNames.size(); i++)
		{

			_refVec.push_back(LoadTexture(("img/" + _fileNames[i]).c_str()));

			SetTextureFilter(_refVec[i], TEXTURE_FILTER_POINT);

		}

	}

};