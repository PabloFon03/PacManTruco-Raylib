#pragma once

// Player Save Data Class
class PlayerData
{

public:

	PlayerData() {}

	int GetTokens() { return tokens; }
	void AddTokens(int _amount)
	{
		tokens += _amount;
		if (tokens < 0) { tokens = 0; }
	}

	void ResetPlayerItems() { for (int i = 0; i < 3; i++) { playerItems[i] = 0; } }
	void SetPlayerItem(int _slot, int _item) { playerItems[_slot] = _item; }
	int GetPlayerItem(int _slot) { return playerItems[_slot]; }

private:

	int tokens{ 50 };
	int playerItems[4]{ 0, 0, 0, 0 };

};