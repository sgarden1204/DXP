#pragma once
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
	int current_game_stage;
};

