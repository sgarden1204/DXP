#pragma once
#include"Unit.h"

class Enemy : public Unit
{
public:
	Enemy();
	~Enemy();

public:
	void Enemy_Init();
	void Knock_Back();

	void Unit_Pos_Init(int pos_x_, int pos_y_);
	void Unit_Move();
	void Unit_Init(int pos_x_, int pos_y_, int hp_, int atk_damage_, int move_speed_, int type_);
};

