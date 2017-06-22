#pragma once
#include"Unit.h"

class Enemy : public Unit
{
public:
	Enemy();
	~Enemy();

public:

	void Unit_Move();
	void Unit_Init(int pos_x_, int pos_y_, int hp_, int atk_damage_, int move_speed_, int type_);
};

