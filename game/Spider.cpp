#include "stdafx.h"
#include "Spider.h"

CSpider::CSpider(Sint16 x, Sint16 y, char* pFileBitmap, Uint32 time) : CSprite(x, y, 0, 0, time)
{
	AddImage("Enemy1.png", "fly", 1, 4, 0, 0, 4, 0);
	SetAnimation("fly", 10);

	health = 100;
	state = NONE;
	ChangeState(IDLE);
}

CSpider::CSpider(Sint16 x, Sint16 y, CGraphics* pGraphics, Uint32 time) : CSprite(x, y, 0, 0, time)
{
	
	AddImage("Enemy1.png", "fly", 1, 4, 0, 0, 4, 0);
	SetAnimation("fly", 10);

	health = 100;
	state = NONE;
	ChangeState(IDLE);
}

CSpider::~CSpider(void)
{
}

void CSpider::OnAttacked()
{
	health -= 15.0f;
	if (health < 0) health = 0;
	isAttacked = true;
}

void CSpider::OnUpdate(Uint32 time, Uint32 deltaTime)
{
	// State-dependent actions
	switch (state)
	{
	case IDLE:
		
		health += 0.1f;
		break;
	case PATROL:	// take a random turn at a random frame, on average once every 60 frames
		
		
		if (rand() % 60 == 0)
			SetDirection((float)(rand() % 360));
		break;
	case CHASE:
		
		SetDirection(enemyPosition - GetPosition());
		
		break;
	case ATTACK:
		
		
		break;
	case FLEE:
		
		SetDirection(GetPosition() - enemyPosition);
		break;
	case DIE:
		break;
	}

	const int screenWidth = 1280;
	const int screenHeight = 768;
	const int buffer = 1; // Small buffer to avoid immediate repositioning

	if (GetPosition().m_x < -buffer) SetPosition(screenWidth + buffer, GetPosition().m_y);
	if (GetPosition().m_x > screenWidth + buffer) SetPosition(-buffer, GetPosition().m_y);
	if (GetPosition().m_y < -buffer) SetPosition(GetPosition().m_x, screenHeight + buffer);
	if (GetPosition().m_y > screenHeight + buffer) SetPosition(GetPosition().m_x, -buffer);

		



	// if in motion, rotate so that to follow the direction of the motion; otherwise reset rotation
	if (GetSpeed() > 0)
		SetRotation(GetDirection() - 90);
	else if (state != ATTACK)
		SetRotation(0);

	// Transitions
	float enemyDistance = Distance(enemyPosition, GetPosition()); 
	if (isAttacked)
	{
		ChangeState(CHASE);
		isAttacked = false; // Reset the flag
	}
	switch (state)
	{
	case IDLE:
		if(health > 90) ChangeState(PATROL);
		if (enemyDistance < 200 && health > 40) ChangeState(CHASE);
		if (enemyDistance < 50) ChangeState(ATTACK);
		if (health < 1) ChangeState(DIE);
		break;
	case PATROL:
		if (enemyDistance < 200) ChangeState(CHASE);
		if (health < 20) ChangeState(IDLE);

		break;
	case CHASE:
		if (enemyDistance > 250 && isAttacked == true) ChangeState(IDLE);
		if (enemyDistance < 100) ChangeState(ATTACK);
		if (health < 30) ChangeState(FLEE);
		break;

	case ATTACK:
		if (enemyDistance > 120) ChangeState(CHASE);
		if (health < 30) ChangeState(FLEE);
		if (health < 1) ChangeState(DIE);
		break;
	case FLEE:
		if (health < 15) ChangeState(IDLE);
		if (enemyDistance > 250) ChangeState(IDLE);
		break;
	case DIE:
		break;
	}

	CSprite::OnUpdate(time, deltaTime);
}

void CSpider::ChangeState(STATE newState)
{
	if (newState == state)
		return;		// No state change

	state = newState;

	switch (state)
	{
	case IDLE:
		SetSpeed(100);
		
		break;
	case PATROL:
		
		SetDirection((float)(rand() % 360));
		SetSpeed(PATROL_SPEED);
		break;
	case CHASE:
		
		SetDirection(enemyPosition - GetPosition());
		SetSpeed(CHASE_SPEED);
		break;
	case ATTACK:
		
		
		

		
		SetVelocity(0, 0);
		break;
	case FLEE:
		
		SetDirection(GetPosition() - enemyPosition);
		SetSpeed(100);
		break;
	case DIE:
		
		SetVelocity(0, 0);
		break;
	
	}
}
