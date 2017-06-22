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

	int ending_count;
	int victory_count;

	int active_unit_count;
	int active_enemy_unit_count;

	bool victory;
	bool start;

	int ready_frame;

	bool stage_ready;
	bool stage_start;
	bool stage_end;

	int  cool_down;

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

