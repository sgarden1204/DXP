#pragma once
class Unit
{
public:
	Unit();
	~Unit();

public:

	int hp;
	int atk_damage;
	int move_speed;
	int type;
	int state;
	
	int pos_x;
	int pos_y;

	int frame;

	bool active;

	virtual void Unit_Pos_Init(int pos_x_,int pos_y_) = 0;
	virtual void Unit_Move() = 0;
	virtual void Unit_Init(int pos_x_,int pos_y_,int hp_,int atk_damage_,int move_speed_,int type_) = 0;
};

