#pragma once

#include<string>
#include<vector>
#include <raylib.h>

// Class For Common Resources
class Resources
{

public:

	Resources() { }

	// Unload Resources On Destruction
	~Resources() { Unload(); }

	void Load()
	{

		// Load Fragment Shader Files
		LoadShaders(shaders, std::vector<std::string>
		{
			"mazeFlip.fs",
			"uvTest.fs",
			"gray.fs"
		});

		// Load Common Texture Files
		LoadTextures(commonTextures, std::vector<std::string>
		{
			"CharAtlas.png",
			"Tokens.png",
			"HealthIcons.png",
			"Box0.png",
			"Box1.png",
			"Box2.png",
			"Box3.png",
			"Box4.png",
			"Box5.png",
			"Box6.png",
			"Box7.png",
			"Box8.png",
			"Box9.png"
		});

		// Load Loading Screen Texture Files
		LoadTextures(loadingTrainTextures, std::vector<std::string>
		{
			"TrainStop.png",
			"TrainStart.png",
			"TrainWagon.png",
		}, "loading/");

		// Load Maze Texture Files
		LoadTextures(mazeTextures, std::vector<std::string>
		{
			"ItemIcons.png",
			"CharmIcons.png",
			"RoundCounter.png",
			"MazeTiles.png",
			"Mouse.png",
			"Alliga.png",
			"Erika.png",
			"Alice.png",
			"Carla.png"
		}, "maze/");

		// Load Baseball Texture Files
		LoadTextures(baseballTextures, std::vector<std::string>
		{
			"ElectricBase.png",
			"ElectricBall.png",
			"ElectricPlatform.png",
			"Baseballs.png",
			"Carla.png",
			"CarlaPlatform.png",
			"AshleyPlatform.png"
		}, "baseball/");

		// Load Baseball Texture Files
		LoadTextures(clawTextures, std::vector<std::string>
		{
			"Claw.png",
			"Rope.png",
			"RowTiles.png",
			"NoTokens.png"
		}, "claw/");

	}

	void Unload()
	{

		// Unload Shaders
		for (int i = 0; i < shaders.size(); i++) { UnloadShader(shaders[i]); }

		// Unload Common Textures
		for (int i = 0; i < commonTextures.size(); i++) { UnloadTexture(commonTextures[i]); }

		// Unload Loading Train Screen Textures
		for (int i = 0; i < loadingTrainTextures.size(); i++) { UnloadTexture(loadingTrainTextures[i]); }

		// Unload Maze Textures
		for (int i = 0; i < mazeTextures.size(); i++) { UnloadTexture(mazeTextures[i]); }

		// Unload Baseball Textures
		for (int i = 0; i < baseballTextures.size(); i++) { UnloadTexture(baseballTextures[i]); }

		// Unload Claw Textures
		for (int i = 0; i < clawTextures.size(); i++) { UnloadTexture(clawTextures[i]); }

	}

	Shader GetShader(int _i) { return shaders[_i]; }

	Texture2D GetCommonTexture(int _i) { return commonTextures[_i]; }
	Texture2D GetLoadingTrainTexture(int _i) { return loadingTrainTextures[_i]; }
	Texture2D GetMazeTexture(int _i) { return mazeTextures[_i]; }
	Texture2D GetBaseballTexture(int _i) { return baseballTextures[_i]; }
	Texture2D GetClawTexture(int _i) { return clawTextures[_i]; }

private:

	std::vector<Shader> shaders{ std::vector<Shader>() };

	std::vector<Texture2D> commonTextures{ std::vector<Texture2D>() };
	std::vector<Texture2D> loadingTrainTextures{ std::vector<Texture2D>() };
	std::vector<Texture2D> mazeTextures{ std::vector<Texture2D>() };
	std::vector<Texture2D> baseballTextures{ std::vector<Texture2D>() };
	std::vector<Texture2D> clawTextures{ std::vector<Texture2D>() };

	void LoadShaders(std::vector<Shader>& _refVec, std::vector<std::string> _fileNames, std::string _aboveFolder = "") { for (int i = 0; i < _fileNames.size(); i++) { _refVec.push_back(LoadShader(0, ("shaders/" + _aboveFolder + _fileNames[i]).c_str())); } }

	void LoadTextures(std::vector<Texture2D>& _refVec, std::vector<std::string> _fileNames, std::string _aboveFolder = "")
	{

		for (int i = 0; i < _fileNames.size(); i++)
		{

			_refVec.push_back(LoadTexture(("img/" + _aboveFolder + _fileNames[i]).c_str()));

			SetTextureFilter(_refVec[i], TEXTURE_FILTER_POINT);

		}

	}

};