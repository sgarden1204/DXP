#include "GameManager.h"

void GameManager::Delete_Instance()
{
	if (instance != nullptr)
	{
		delete instance;
		instance = nullptr;
	}
}

void GameManager::Render()
{

}

void GameManager::Init()
{
	ending_count = 0;
	victory_count = 0;

	active_unit_count = 0;
	active_enemy_unit_count = 0;

	victory = false;
	start = false;

	ready_frame = 0;

	stage_ready = true;
	stage_start = true;
	stage_end = false;

	cool_down = 10;

	wait_music = 0;
	wait_count = 0;
	wait_energy = 0;
	current_game_stage = 1;


	unit_count = 0;
	unit_enemy_count = 0;
	energy_percent = 0;

	SetRect(&unit_rect, 40, 20, 200, 200);
	SetRect(&unit_enemy_rect, 340, 20, 200, 200);
	SetRect(&energy_rect, 600, 20, 200, 200);
}
