#include "Enemy.h"



Enemy::Enemy()
{
	hp = 0;
	atk_damage = 0;
	move_speed = 0;
	type = 0;
	state = 0;

	frame = 0;

	pos_x = 0;
	pos_y = 0;

	active = false;
}


Enemy::~Enemy()
{

}

void Enemy::Enemy_Init()
{
	
}

void Enemy::Unit_Pos_Init(int pos_x_, int pos_y_)
{
}

void Enemy::Unit_Move()
{
	pos_x = pos_x + move_speed;
}

void Enemy::Unit_Init(int pos_x_, int pos_y_, int hp_, int atk_damage_, int move_speed_, int type_)
{
	pos_x = pos_x_;
	pos_y = pos_y_;

	hp = hp_;
	atk_damage = atk_damage_;
	move_speed = move_speed_;
	type = type_;
}
