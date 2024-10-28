#include "stdafx.h"
#include "MyGame.h"

#pragma warning(disable:4244)

CMyGame::CMyGame(void) :
	m_player(640, 384, "SpaceshipA.png", 0),
	m_background("tile_5.png"),
	Targetingtimer(0),
	enemycooldown(0),
	insideenemy(false),
	insideplayer(false),
	isControls(false),
	IsTargeting(false),
	score(0)
{
	
}

CMyGame::~CMyGame(void)
{
	// TODO: add destruction code here
}

/////////////////////////////////////////////////////
// Per-Frame Callback Funtions (must be implemented!)

void CMyGame::OnUpdate()
{
	if (!IsGameMode())  return;

	Uint32 t = GetTime();

	// Update AI Agents
	if (m_player.Isdemo == false)
	{ 
	if (IsKeyDown(SDLK_w) || IsKeyDown(SDLK_UP)) m_player.Input(CPlayer::UP, false);
	if(IsKeyDown(SDLK_s) || IsKeyDown(SDLK_DOWN)) m_player.Input(CPlayer::DOWN, false);
    
	// rotate left and right
	if (IsKeyDown(SDLK_a) || IsKeyDown(SDLK_LEFT)) m_player.Input(CPlayer::LEFT, false);
	if (IsKeyDown(SDLK_d) || IsKeyDown(SDLK_RIGHT)) m_player.Input(CPlayer::RIGHT, false);
	m_player.Input(CPlayer::NO_DIR, false);
	}
	float health = 0;
	
	
	
	

	//game over if the player health is less than or equal to 0
	if (m_player.GetHealth() <= 0)
	{
		
		GameOver();
	}
	
	
	//functions
	Demobehavior();
	Fixcrash();
	Spiderbehavior();
	healthdropbehavior();
	// update sprites
	m_player.Update(t);
	for (CSpider* pSpider : m_spiders)
		pSpider->Update(t);
	// update player shots here 
	Shotbehavior();
	for (CSprite* pShot : shotList)
		pShot->Update(t);
	
	// update health drops here
	for (CSprite* healthdrop : healthdrops)
		healthdrop->Update(t);
	
	//update explosions here
	for (CSprite* explosion : explosions)
		explosion->Update(t);
	// update enemy shots here
	for (CSprite* pShot : EnemyShotList)
		pShot->Update(t);
	//cooldowns
	for (CSpider* pSpider : m_spiders)
	{
		
		
			if (pSpider->shotcooldown > 0) pSpider->shotcooldown--;
		
	}
	// player shot cooldown
	if (m_player.shotcooldown > 0) m_player.shotcooldown--;
	if (enemycooldown > 0) enemycooldown--;
	if (Targetingtimer > 0) Targetingtimer--;
	// deletelist if they are deleted 
	shotList.delete_if(deleted);
	EnemyShotList.delete_if(deleted);
	explosions.delete_if(deleted);
	m_spiders.delete_if(deleted);
	
}

void DrawHealth(CGraphics* g, CVector pos, float w, float h, float health)
{
	if (health <= 0) return;
	g->DrawRect(CRectangle(pos.m_x - 1, pos.m_y - 1, w + 1, h + 1), CColor::Black());
	g->FillRect(CRectangle(pos.m_x, pos.m_y - 1, ceil(w * health / 100.f), h), health > 20 ? CColor::DarkGreen() : CColor::Red());
}

void CMyGame::OnDraw(CGraphics* g)
{
	// Draw background
	for (int i = 0; i < 5; i++)
		for (int j = 0; j < 3; j++)
			g->Blit(CVector(256.f*i, 256.f *j), m_background);

	m_player.Draw(g);
	DrawHealth(g, m_player.GetPosition() + CVector(-32, 32), 20, 4, m_player.GetHealth());
	for (CSpider* pSpider : m_spiders)
	{
		pSpider->Draw(g);
		DrawHealth(g, pSpider->GetPosition() + CVector(-32, 32), 20, 4, pSpider->GetHealth());
	}
	//draw player shots here
	for (CSprite* pShot : shotList)
		pShot->Draw(g);
	//draw enemy shots here
	for (CSprite* pShot : EnemyShotList)
		pShot->Draw(g);
	//draw health drops here
	for (CSprite* healthdrop : healthdrops)
		healthdrop->Draw(g);
	//draw explosions here
	for (CSprite* explosion : explosions)
		explosion->Draw(g);
	// draw score 
	if (!IsMenuMode())
	{
		*g << font(24) << color(CColor::Red()) << bottom << left << "Score: " << score << endl;
		// draw playe speed		
		*g << font(24) << color(CColor::Red()) << bottom << right << "Speed: " << m_player.GetSpeed() << endl;
		if (IsGameOver())
			if (m_player.GetHealth() <= 0)
				*g << font(48) << color(CColor::DarkRed()) << vcenter << center << "GAME OVER" << " YOUR SCORE IS: " << score << endl;
	}
	// if in menu mode draw the start screen
	if (IsMenuMode())
	{
		if (isControls == false)
		{
			ShowMouse();
			Startscreen.Draw(g);
			Playbutton.SetPosition(640, 384);
			Playbutton.Draw(g);
			Quitbutton.SetPosition(640, 384 - 200);
			Quitbutton.Draw(g);
			Controlsbutton.SetPosition(640, 384 - 100);
			Controlsbutton.Draw(g);
			Demo.SetPosition(640, 384 - 300);
			Demo.Draw(g);
			if (m_player.Isdemo == true)
			{
				Explaination.SetPosition(1000, 200);
				Explaination.Draw(g);
				OverDrive.SetPosition(200, 84);
				OverDrive.Draw(g);
			}
		}
		// if controls is true draw controls
		if (isControls == true)
		{
			Controls.Draw(g);
			Exitbutton.SetPosition(640, 384 - 300);
			Exitbutton.Draw(g);

		}
		

	}
	// pause menu
	if (IsPaused() && !IsMenuMode())
	{
		ShowMouse();
		*g << font(48) << color(CColor::Red()) << xy(730,700) << "PAUSED" << endl;
		Quitbutton.SetPosition(640, 300);
		Quitbutton.Draw(g);
		MenuButton.SetPosition(640, 600);
		MenuButton.Draw(g);
	}
	if (!IsPaused() && !IsMenuMode() && !IsGameOver())
	{
		HideMouse();
	}
	// if its game over show mouse and show the menu and quit button
	if (IsGameOver())
	{
		ShowMouse();
		Quitbutton.SetPosition(640, 300);
		Quitbutton.Draw(g);
		MenuButton.SetPosition(640, 600);
		MenuButton.Draw(g);
	}
	if (!IsMenuMode() && m_player.Isdemo == true)
	{
		*g << font(24) << color(CColor::Red()) << top << left << "DEMO MODE" << endl;
	}
}

/////////////////////////////////////////////////////
// Game Life Cycle

// one time initialisation
void CMyGame::OnInitialize()
{	
	m_player.Isdemo = false;
	//menu images 
	Explaination.LoadImage("Explaination.bmp");
	Explaination.SetImage("Explaination.bmp");
	
	Startscreen.LoadImage("startscreen.bmp");
	Startscreen.SetImage("startscreen.bmp");
	Controls.LoadImage("Controls.bmp");
	Controls.SetImage("Controls.bmp");
	Playbutton.LoadImage("Play.bmp");
	Playbutton.SetImage("Play.bmp");
	Quitbutton.LoadImage("Quit.bmp");
	Quitbutton.SetImage("Quit.bmp");
	Controlsbutton.LoadImage("Controlsbutton.bmp");
	Controlsbutton.SetImage("Controlsbutton.bmp");
	Exitbutton.LoadImage("Exit.bmp");
	Exitbutton.SetImage("Exit.bmp");
	MenuButton.LoadImage("Menu.bmp");
	MenuButton.SetImage("Menu.bmp");
	Demo.LoadImage("DemoOff.bmp");
	Demo.LoadImage("DemoOn.bmp");
	if (m_player.Isdemo == false)
	{
		Demo.SetImage("DemoOff.bmp");
	}
	if (m_player.Isdemo == true)
	{
		Demo.SetImage("DemoOn.bmp");
	}
	OverDrive.LoadImage("OverdriveOff.bmp");
	OverDrive.LoadImage("OverdriveOn.bmp");
	if (m_player.IsOverDrive == false)
	{
		OverDrive.SetImage("OverdriveOff.bmp");
	}
	if (m_player.IsOverDrive == true)
	{
		OverDrive.SetImage("OverdriveOn.bmp");
	}
}

// called when a new game is requested (e.g. when F2 pressed)
// use this function to prepare a menu or a welcome screen
void CMyGame::OnDisplayMenu()
{
	GameMusic.Stop();
	Startscreen.SetPosition(640, 384);
	Controls.SetPosition(640, 384);
	MenuMusic.Play("menumusic.wav",-1);
	isControls = false;

	
}

// called when a new game is started
// as a second phase after a menu or a welcome screen
void CMyGame::OnStartGame()
{
	
	
	MenuMusic.Stop();
	GameMusic.Play("gamemusic.wav",-1);
	insideenemy = false;
	score = 0;
	m_spiders.delete_all();
	shotList.delete_all();
	EnemyShotList.delete_all();
	healthdrops.delete_all();
	explosions.delete_all();

	m_player.SetEnemyPosition(CVector(2500, 2500));
	m_player.SetDirection(0);
	m_player.SetRotation(0);
	m_player.SetSpeed(0);
	m_player.SetPosition(640, 384);
	
	m_player.SetHealth(100);
	m_player.ChangeState(CPlayer::IDLE, CPlayer::DOWN);
	
	if (m_player.Isdemo == true)
	{
		m_player.ChangeDemoState(CPlayer::IDLE);
	}
}

// called when a new level started - first call for nLevel = 1
void CMyGame::OnStartLevel(Sint16 nLevel)
{
	
}

// called when the game is over
void CMyGame::OnGameOver()
{
	GameOverMusic.Play("gameover.wav");
}

// one time termination code
void CMyGame::OnTerminate()
{
}

/////////////////////////////////////////////////////
// Keyboard Event Handlers

void CMyGame::OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode)
{
	if (sym == SDLK_F4 && (mod & (KMOD_LALT | KMOD_RALT)))
		StopGame();
	if (sym == SDLK_ESCAPE)
		PauseGame();
	if (sym == SDLK_ESCAPE && !IsPaused() && !IsMenuMode())
	{
		PauseSound.Play("pause.wav");

	}
	if (sym == SDLK_ESCAPE && IsPaused() && !IsMenuMode())
	{
		PauseSound.Play("resume.wav");
	}

	if (sym == SDLK_F2)
		NewGame();
	
}

void CMyGame::OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode)
{
}

/////////////////////////////////////////////////////
// Mouse Events Handlers

void CMyGame::OnMouseMove(Uint16 x,Uint16 y,Sint16 relx,Sint16 rely,bool bLeft,bool bRight,bool bMiddle)
{
}

void CMyGame::OnLButtonDown(Uint16 x,Uint16 y)
{
	//menu behaviours 
	if (IsMenuMode())
	{
		if (isControls == false)
		{
			if (Playbutton.HitTest(x, y))
			{
				MenuSelect.Play("MenuSelect.wav");
				StartGame();
				if (IsPaused())
				{
					PauseGame();
				}
			}
			if (Quitbutton.HitTest(x, y))
			{
				MenuSelect.Play("MenuSelect.wav");
				StopGame();
			}
			if (Controlsbutton.HitTest(x, y))
			{
				MenuSelect.Play("MenuSelect.wav");
				isControls = true;
			}
			if (Demo.HitTest(x, y))
			{
				MenuSelect.Play("MenuSelect.wav");
				if (m_player.Isdemo == false)
				{
					m_player.Isdemo = true;
					Demo.SetImage("DemoOn.bmp");
				
				}
				else
				{
					m_player.IsOverDrive = false;
					m_player.Isdemo = false;
					Demo.SetImage("DemoOff.bmp");
					OverDrive.SetImage("OverdriveOff.bmp");
				}
			}
			if (m_player.Isdemo == true)
			{
				if (OverDrive.HitTest(x, y))
				{
					MenuSelect.Play("MenuSelect.wav");
					if (m_player.IsOverDrive == false)
					{
						m_player.IsOverDrive = true;
						OverDrive.SetImage("OverdriveOn.bmp");
						
					}
					else
					{
						m_player.IsOverDrive = false;
						OverDrive.SetImage("OverdriveOff.bmp");
						
					}
				}
			}
			
		}
		if (isControls == true)
		{
			if (Exitbutton.HitTest(x, y))
			{
				MenuSelect.Play("MenuSelect.wav");
				isControls = false;
			}
		}
	}
	if (IsPaused() && !IsMenuMode() || IsGameOver())
	{
		if (Quitbutton.HitTest(x, y))
		{
			MenuSelect.Play("MenuSelect.wav");
			StopGame();
		}
		if (MenuButton.HitTest(x, y))
		{
			MenuSelect.Play("MenuSelect.wav");
			NewGame();
		}
	}
}

void CMyGame::OnLButtonUp(Uint16 x,Uint16 y)
{
}

void CMyGame::OnRButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnRButtonUp(Uint16 x,Uint16 y)
{
}

void CMyGame::OnMButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnMButtonUp(Uint16 x,Uint16 y)
{
}

void CMyGame::Spiderbehavior()
{
	//set direction of the spiders to the player
	for (CSpider* pSpider : m_spiders)
	{


		pSpider->SetEnemyPosition(m_player.GetPosition());


	}
	//if the state of enemies = attack then shoot at the player
	for (CSpider* pSpider : m_spiders)
	{
		if (pSpider->GetState() == CSpider::ATTACK && insideplayer == false)
		{
			// add enemy cool down here
			if (pSpider->shotcooldown == 0)
			{

				CSprite* EShot = new CSprite(pSpider->GetX(), pSpider->GetY(), 0, 0, GetTime());
				EShot->AddImage("shot2.png", "shot2", 10, 1, 0, 0, 9, 0);
				EShot->SetAnimation("shot2", 12);
				EShot->SetDirection(m_player.GetPosition() - pSpider->GetPosition());
				EShot->SetRotation(EShot->GetDirection());
				EShot->SetSpeed(500);

				EnemyShotList.push_back(EShot);
				pSpider->shotSound.Play("EnemyShot.wav");
				pSpider->shotcooldown = 120;
			}
		}
	}

	//if the state of enemies = CHASE then shoot at the player (for some reason if you put this into an or statement it breaks every thing)
	for (CSpider* pSpider : m_spiders)
	{
		if (pSpider->GetState() == CSpider::CHASE && insideplayer == false)
		{
			// add enemy cool down here
			if (pSpider->shotcooldown == 0)
			{

				CSprite* EShot = new CSprite(pSpider->GetX(), pSpider->GetY(), 0, 0, GetTime());
				EShot->AddImage("shot2.png", "shot2", 10, 1, 0, 0, 9, 0);
				EShot->SetAnimation("shot2", 12);
				EShot->SetDirection(m_player.GetPosition() - pSpider->GetPosition());
				EShot->SetRotation(EShot->GetDirection());
				EShot->SetSpeed(500);

				EnemyShotList.push_back(EShot);
				pSpider->shotSound.Play("EnemyShot.wav");
				pSpider->shotcooldown = 120;
			}
		}
	}
	// while the spider is in the attack state make their rotation follow the player 
	for (CSpider* pSpider : m_spiders)
	{
		if (pSpider->GetState() == CSpider::ATTACK)
		{
			pSpider->SetDirection(m_player.GetPosition() - pSpider->GetPosition());
			pSpider->SetRotation(pSpider->GetDirection() - 90);
		}
	}

	//for the actual game the enemies spawn less to make it easier 

	if (m_player.IsOverDrive == false)
	{

		if (m_spiders.size() < 5 && enemycooldown == 0)
		{
			CVector playerPos = m_player.GetPosition();
			CVector spawnPos;
			float distance;

			do
			{
				spawnPos = CVector(rand() % 600 + 200, rand() % 300 + 200);
				distance = Distance(playerPos, spawnPos);
			} while (distance < 300.0f);

			m_spiders.push_back(new CSpider(spawnPos.m_x, spawnPos.m_y, "enemy1.png", 0));
			enemycooldown = 160; // around 4 seconds
		}
	}

		if (m_player.IsOverDrive == true)
		{
			if (m_spiders.size() < 10 && enemycooldown == 0)
			{
				CVector playerPos = m_player.GetPosition();
				CVector spawnPos;
				float distance;

				do
				{
					spawnPos = CVector(rand() % 600 + 200, rand() % 300 + 200);
					distance = Distance(playerPos, spawnPos);
				} while (distance < 300.0f);

				m_spiders.push_back(new CSpider(spawnPos.m_x, spawnPos.m_y, "enemy1.png", 0));
				enemycooldown = 80; //around 2 seconds
			}
		}

	
	
	
	//when a spider dies add 10 points to the score		
	for (CSpider* pSpider : m_spiders)
	{
		if (pSpider->GetHealth() <= 0)
		{
			
			// create explosion animation where the spider died
			CSprite* pExplosion = new CSprite(pSpider->GetX(), pSpider->GetY(), 0, 0, GetTime());
			pExplosion->AddImage("Explosion.png", "explode", 12, 1, 0, 0, 11, 0);
			// play explosion animation
			pExplosion->SetAnimation("explode", 12);
			// set explosion animation to die in 500ms
			pExplosion->Die(500); // remove in 500ms
			// add explosion animation sprite to explosions sprite lists
			explosions.push_back(pExplosion);
			// play explosion sound
			explosionsound.Play("explosion.wav");
		
			//create health 1 health drop where the spider died
			CSprite* healthdrop = new CSprite(pSpider->GetX(), pSpider->GetY(), 0, 0, GetTime());
			healthdrop->AddImage("health.png", "healthdrop", 5, 1, 0, 0, 4, 0);
			healthdrop->SetAnimation("healthdrop", 10);
			// add the health drop to the list
			healthdrops.push_back(healthdrop);
			pSpider->Delete();
			
			score += 10;
			
			break;
			
			

		}
	}
	// if spider is inside player reduce players heatlh by 1
	for (CSpider* pSpider : m_spiders)
	{
		if (insideplayer == true)
		{
			m_player.SetHealth(m_player.GetHealth() - 1);
			break;
		}
	}
	
}

void CMyGame::Shotbehavior()
{
	// if the space key is pressed shoot a shot
	if (IsKeyDown(SDLK_SPACE))
	{
		if (m_player.shotcooldown == 0 && insideenemy == false && m_player.Isdemo == false)
		{
			{

				CSprite* newShot = new CSprite(m_player.GetX(), m_player.GetY(), 0, 0, GetTime());
				// set the motion of the new shot sprite
				newShot->AddImage("shot.png", "shot1", 10, 1, 0, 0, 9, 0);
				newShot->SetAnimation("shot1", 12);
				newShot->SetDirection(m_player.GetRotation());
				newShot->SetRotation(m_player.GetRotation());
				newShot->SetSpeed(500);
				// add the shot sprite to the list
				shotList.push_back(newShot);
				// play shot sound
				m_player.shotSound.Play("shot.wav");

				m_player.shotcooldown = 15; // just under half a second
			}
		}
	}
	for (CSprite* pShot : shotList)
	{
		if (pShot->GetX() < 0 || pShot->GetX() > 1280 || pShot->GetY() < 0 || pShot->GetY() > 768)
		{
			pShot->Delete();
			break;
		}
	}
	// remove shots that are off screen
	for (CSprite* EShot : EnemyShotList)
	{
		if (EShot->GetX() < 0 || EShot->GetX() > 1280 || EShot->GetY() < 0 || EShot->GetY() > 768)
		{
			EShot->Delete();
			break;
		}
	}
	// check for collisions between shots and spiders 
	for (CSprite* pShot : shotList)
	{
		for (CSpider* pSpider : m_spiders)
		{
			if (pShot->HitTest(pSpider))
			{
				pSpider->OnAttacked();
				
				pShot->Delete();
				break;
			}
		}
	}
	// check for collisions between shots and player
	for (CSprite* EShot : EnemyShotList)
	{
		if (EShot->HitTest(&m_player))
		{
			m_player.OnAttacked();
			hitsound.Play("playerhit.wav");
			EShot->Delete();
			break;
		}
	}
	
}



void CMyGame::Fixcrash()
{
	//if the player is inside an enemy set the inside enemy to true if they aren't set it to false
	for (CSpider* pSpider : m_spiders)
	{
		if (m_player.HitTest(pSpider))
		{
			insideenemy = true;
			shotList.delete_all();
			EnemyShotList.delete_all();
		    insideplayer = true;
			break;
		}
		else
		{
			insideenemy = false;
			insideplayer = false;
		}
	}
	
}

void CMyGame::healthdropbehavior()
{
	//if the player collides with a health drop increase the players health by 30 and delete the health drop
	for (CSprite* healthdrop : healthdrops)
	{
		if (m_player.HitTest(healthdrop) && m_player.GetHealth() <= 100)
		{
			
			m_player.SetHealth(m_player.GetHealth() + 30);
			healthdrop->Delete();
			Healthdropsound.Play("Healthpickup.wav");
			break;
		}
	}
	healthdrops.delete_if(deleted);
}

void CMyGame::Demobehavior()
{
	// set the players enemy position to the spiders position

	for (CSpider* pSpider : m_spiders)
	{
		
		
			float distance = Distance(m_player.GetPosition(), pSpider->GetPosition());
			if (distance < 250.0f && !IsTargeting)
			{
				m_player.SetEnemyPosition(pSpider->GetPosition());
				Targetingtimer = 80;
				IsTargeting = true;


			}
			// way of fudging the set enemy position so it effectively resets the position allowing the player to change to the patrol state without having to make a whole new class of spiders
			if (pSpider->GetHealth() <= 0 || Targetingtimer == 0)
			{
				IsTargeting = false;
			}
			if (IsTargeting == false)
			{
				m_player.SetEnemyPosition(CVector(2500, 2500));
				
			}
		
		
	}
	// if the players case is attack or chase 
	
		if (m_player.GetState() == CPlayer::ATTACK && insideenemy == false)
		{
			// add player cool down here
			if (m_player.shotcooldown == 0)
			{
				{

					CSprite* newShot = new CSprite(m_player.GetX(), m_player.GetY(), 0, 0, GetTime());
					// set the motion of the new shot sprite
					newShot->AddImage("shot.png", "shot1", 10, 1, 0, 0, 9, 0);
					newShot->SetAnimation("shot1", 12);
					newShot->SetDirection(m_player.GetRotation());
					newShot->SetRotation(m_player.GetRotation());
					//if overdrive is on set the speed of the shot to 600
					if (m_player.IsOverDrive == true)
					{
						newShot->SetSpeed(1000);
					}
					//if overdrive is off set the speed of the shot to 500
					if (m_player.IsOverDrive == false)
					{
						newShot->SetSpeed(500);
					}
					
					// add the shot sprite to the list
					shotList.push_back(newShot);
					// play shot sound
					m_player.shotSound.Play("shot.wav");
					// if overdrive is off set the shot cooldown to 30
					if (m_player.IsOverDrive == false)
					{
						m_player.shotcooldown = 15; // just under half a second
					}
					// if overdrive is on set the shot cooldown to 15
					if (m_player.IsOverDrive == true)
					{

						m_player.shotcooldown = 8; // just under half a second
					}

				}
			}
		}
	



	for (CSpider* pSpider : m_spiders)
	{
		if (m_player.GetState() == CPlayer::ATTACK)
		{
			float distance = Distance(m_player.GetPosition(), pSpider->GetPosition());
			if (distance <= 250.0f)
			{
				m_player.SetDirection(pSpider->GetPosition() - m_player.GetPosition());
				m_player.SetRotation(m_player.GetDirection());
			}
		}
	}
	
	// if there are health packs and the case is patrol then move towards the health pack
	for (CSprite* healthdrop : healthdrops)
	{
		if (m_player.GetState() == CPlayer::PATROL && m_player.GetHealth() <= 100)
		{
			m_player.SetDirection(healthdrop->GetPosition() - m_player.GetPosition());
			m_player.SetRotation(m_player.GetDirection());
		}
	}
	// if the player is inside an enemy reduce the players health by 20 and teleport them outside of the enemy
	if (m_player.Isdemo == true)
	{
		for (CSpider* pSpider : m_spiders)
		{
			if (insideenemy == true)
			{
				m_player.SetHealth(m_player.GetHealth() - 20);
				m_player.SetPosition(640, 384);
				hitsound.Play("playerhit.wav");
				insideenemy = false;
				break;
			}
		}
	}
	
}
