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


};

