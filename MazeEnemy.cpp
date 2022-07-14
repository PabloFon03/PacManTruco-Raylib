#include "Maze.h"

using namespace PacMan_Board;

Board::Entity::Vector2Int Board::Enemy::GetTarget(int _i)
{

	Vector2Int playerCoords = board->GetPlayerPos();

	switch (_i)
	{
	case 1: return AddDir(AddDir(playerCoords, board->GetPlayerDirIndex(), false), board->GetPlayerDirIndex(), false);
	default: return playerCoords;
	}

}

void Board::Enemy::UpdateMovement()
{

	float deltaTime = GetDeltaTime();

	stepTimer += 5 * deltaTime;
	if (stepTimer >= 1)
	{

		// Move To Next Tile
		coords = AddDir(coords, dirIndex);

		// Try Turning
		ChangeDir();

		// Reset Step Timer
		stepTimer--;

	}

	// Check For Stunning Projectile
	ProjectileCollisionCheck();

	// Check For Player Collision
	PlayerCollisionCheck();

	if (hitWall)
	{
		stepTimer = 0;
		ChangeDir();
	}

	// Update Animation
	else
	{
		animDelay += deltaTime;
		if (animDelay >= 0.1f)
		{
			animIndex++;
			animIndex %= 2;

			animDelay -= 0.1f;
		}
	}


}

std::vector<Board::Enemy::PossibleDirection> Board::Enemy::GetPossibleDirections(Vector2Int _targetPos, bool _UTurnsAllowed)
{

	// Store All Possible Directions (No U-Turns Allowed)
	std::vector<PossibleDirection> returnVec = std::vector<PossibleDirection>();
	for (int i = -1; i < (_UTurnsAllowed ? 3 : 2); i++)
	{
		int dir = TrueMod(dirIndex + i, 4);
		if (IsValidDir(coords, dir)) { returnVec.push_back(PossibleDirection{ dir, AddDir(coords, dir).distanceTo(_targetPos) }); }
	}

	if (returnVec.size() == 0) { returnVec.push_back(PossibleDirection{ 0, 0 }); }

	// Selection Sort (Sort By Lowest Distance)
	else
	{
		for (int i = 0; i < returnVec.size() - 1; i++)
		{
			int lowestDistIndex = i;
			for (int j = i + 1; j < returnVec.size(); j++) { if (returnVec[j].targetDistance < returnVec[lowestDistIndex].targetDistance) { lowestDistIndex = j; } }
			std::swap(returnVec[i], returnVec[lowestDistIndex]);
		}
	}

	return returnVec;

}

void Board::Enemy::PlayerCollisionCheck()
{

	if (rawDistanceTo(board->GetPlayerRawPos()) < 0.25f)
	{

		switch (board->GetPlayerHitOutcome())
		{

			// 0 - Player Hit
		case 0:
			board->OnPlayerHit();
			break;

			// 1 - Collision Ignored

			// 2 - Stun
		case 2:
			OnStun();
			break;

		}

	}

}

void Board::Enemy::ProjectileCollisionCheck()
{
	std::vector<Vector2> shotProjectiles = board->GetPlayerProjectiles();
	for (int i = 0; i < shotProjectiles.size(); i++) { if (rawDistanceTo(board->GetPlayerRawPos()) < 0.75f) { OnStun(); break; } }
}