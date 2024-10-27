#include "stdafx.h"
#include "Player.h"

CPlayer::CPlayer(Sint16 x, Sint16 y, char* pFileBitmap, Uint32 time) : CSprite(x, y, 0, 0, time)
{
        AddImage("SpaceshipA.png", "move", 5, 1, 0, 0, 4, 0);
		SetAnimation("move", 10);

	health = 100;
	state = NONE;
	ChangeState(IDLE, DOWN);
}

CPlayer::CPlayer(Sint16 x, Sint16 y, CGraphics* pGraphics, Uint32 time) : CSprite(x, y, 0, 0, time)
{
	
	AddImage("SpaceshipA.png", "move", 5, 1, 0, 0, 4, 0);
	SetAnimation("move", 10);
	health = 100;
	state = NONE;
	ChangeState(IDLE, DOWN);
}

CPlayer::~CPlayer(void)
{
}

void CPlayer::OnAttacked()
{
	health -= 5.0f;
	if (health < 0) health = 0;
}

void CPlayer::OnUpdate(Uint32 time, Uint32 deltaTime)
{
	CSprite::OnUpdate(time, deltaTime);
	if (Isdemo == false)
	{
		if (GetPosition().m_x < 0) SetPosition(1280, GetPosition().m_y);
		if (GetPosition().m_x > 1280) SetPosition(0, GetPosition().m_y);
		if (GetPosition().m_y < 0) SetPosition(GetPosition().m_x, 768);
		if (GetPosition().m_y > 768) SetPosition(GetPosition().m_x, 0);
	}
	if (Isdemo == true)
	{
		// State-dependent actions
		switch (state)
		{
		

			
			
		case PATROL:	// take a random turn at a random frame, on average once every 60 frames


			if (rand() % 60 == 0)
				SetDirection((float)(rand() % 360));
			SetRotation(GetDirection());
			
			if (IsOverDrive == false)
			{
				SetSpeed(PATROL_SPEED);
			}
			if (IsOverDrive == true)
			{
				SetSpeed(OVERDRIVE_SPEED);
			}
			break;
		case CHASE:

			SetDirection(enemyPosition - GetPosition());

			break;
		case ATTACK:

			SetDirection(enemyPosition - GetPosition());
			SetRotation(GetDirection());
			break;
		

			SetDirection(GetPosition() - enemyPosition);
			break;
		
		}

		//if spider goes out of the screen, it will reappear on the other side
		if (GetPosition().m_x < 0) SetPosition(1100, GetPosition().m_y);
		if (GetPosition().m_x > 1280) SetPosition(10, GetPosition().m_y);
		if (GetPosition().m_y < 0) SetPosition(GetPosition().m_x, 700);
		if (GetPosition().m_y > 768) SetPosition(GetPosition().m_x, 10);


		

		// Transitions
		float enemyDistance = Distance(enemyPosition, GetPosition());
		
		switch (state)
		{
		case IDLE:
			if (health > 90) ChangeDemoState(PATROL);
			if (enemyDistance < 250) ChangeDemoState(CHASE);
			
			
			break;
		case PATROL:
			if (IsOverDrive == false)
			{
				if (enemyDistance < 250) ChangeDemoState(CHASE);
			}
			if (IsOverDrive == true)
			{
				if (enemyDistance < 1000) ChangeDemoState(CHASE);
			}


			break;
		case CHASE:
			
			if (enemyDistance < 100) ChangeDemoState(ATTACK);
			if (enemyDistance > 250) ChangeDemoState(PATROL);
			
			break;

		case ATTACK:
			
			if (enemyDistance > 120) ChangeDemoState(PATROL);
			
			
			break;
		
		
		}

		
	}
}

void CPlayer::ChangeState(STATE newState, DIR newDir)
{
	if (Isdemo == false)
	{
		if (newState == state && newDir == dir)
			return;		// no state change
		if (newState == state && newDir == NO_DIR)
			return;		// also no state change

		state = newState;
		if (newDir != NO_DIR)
			dir = newDir;

		switch (state)
		{
		case WALK:
			switch (dir)
			{





			case UP: if (GetSpeed() != 300) SetSpeed(GetSpeed() + 10); SetDirection(GetRotation());  break;
			case DOWN:if (GetSpeed() != -300)  SetSpeed(GetSpeed() - 10); SetDirection(GetRotation());  break;
			case LEFT:  Rotate(-5); break;
			case RIGHT: Rotate(5);   break;
			}
			break;

		case ATTACK:

			switch (dir)
			{
			case UP: break;
			case DOWN: break;
			case LEFT: break;
			case RIGHT: break;
			}
			break;

		case IDLE:

			switch (dir)
			{
			case UP: break;
			case DOWN: break;
			case LEFT:  break;
			case RIGHT:  break;
			}
			break;
		}
	}
	
}

void CPlayer::ChangeDemoState(STATE newState)
{
	if (Isdemo == true)
	{
		if (newState == state)
			return; // No state change

		state = newState;

		switch (state)
		{
		case IDLE:
			SetVelocity(0, 0);
			break;

		case PATROL:
			SetDirection((float)(rand() % 360));
			if(IsOverDrive == false)
			{ 
			SetSpeed(PATROL_SPEED);
			}
			if (IsOverDrive == true)
			{
				SetSpeed(OVERDRIVE_SPEED);
			}
			break;

		case CHASE:
			SetDirection(enemyPosition - GetPosition());
			SetRotation(GetDirection());
			if (IsOverDrive == false)
			{
				SetSpeed(CHASE_SPEED);
			}
			if (IsOverDrive == true)
			{
				SetSpeed(OVERDRIVE_SPEED);
			}
			break;

		case ATTACK:
			SetSpeed(0);
			break;
			
		

		
		}
	}
}


void CPlayer::Input(DIR newDir, bool bAttack)
{
	STATE newState;
	if (bAttack)
		newState = ATTACK;
	else if (newDir != NO_DIR)
		newState = WALK;
	else
		newState = IDLE;

	ChangeState(newState, newDir);
}
