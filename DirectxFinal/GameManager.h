#pragma once

#include<d3dx9.h>
class GameManager
{
private:
	GameManager() {};
	~GameManager() {};

	static GameManager *instance;

public:
	static GameManager * get_Instance()
	{
		if (instance == nullptr)
		{
			instance = new GameManager();
		}

		return instance;
	}

	void Delete_Instance();

	void Render();

	void Init();

public:
	int wait_count;
	int wait_music;
	int wait_energy;
	int current_game_stage;

	int unit_enemy_count;
	int unit_count;
	int energy_percent;

	char unit_enemy_str[50];
	char unit_str[50];
	char energy_str[50];

	RECT unit_rect;
	RECT unit_enemy_rect;
	RECT energy_rect;
};
