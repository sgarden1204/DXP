#pragma once
#include "Unit.h"
class Friend : public Unit
{
public:
	Friend();
	~Friend();

public:
	
	void Unit_Pos_Init(int pos_x_, int pos_y_);
	void Unit_Move();
	void Unit_Init(int pos_x_, int pos_y_, int hp_, int atk_damage_, int move_speed_,int type_);
};

