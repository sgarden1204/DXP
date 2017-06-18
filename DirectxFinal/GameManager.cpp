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
	wait_count = 0;
	current_game_stage = 0;
}
