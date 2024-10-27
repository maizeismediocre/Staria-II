#pragma once

class CPlayer : public CSprite
{
public:
	enum STATE { NONE, IDLE, WALK, ATTACK, PATROL, CHASE};
	enum DIR { NO_DIR, UP, DOWN, LEFT, RIGHT };
	bool Isdemo;
	bool IsOverDrive; 
	int shotcooldown = 0;
	CSoundPlayer shotSound;
	
	float PATROL_SPEED = 300;
	float CHASE_SPEED = 300;
	float OVERDRIVE_SPEED = 600;
private:
	STATE state;
	DIR dir;
	float health;
	CVector enemyPosition;

public:
	CPlayer(Sint16 x, Sint16 y, char* pFileBitmap, Uint32 time);
	CPlayer(Sint16 x, Sint16 y, CGraphics* pGraphics, Uint32 time);
	~CPlayer(void);

	void OnAttacked();
	void OnUpdate(Uint32 time, Uint32 deltaTime);

	void ChangeState(STATE newState, DIR newDir);
	void ChangeDemoState(STATE newsState);
	STATE GetState() { return state; }
	DIR GetDir() { return dir; }
	

	float GetHealth() { return health; }
	void SetHealth(float h) { health = h; }
	void SetEnemyPosition(CVector v) { enemyPosition = v; }
	void Input(DIR newDir, bool bAttack);
};

