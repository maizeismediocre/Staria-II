#pragma once

#include "Player.h"
#include "Spider.h"

class CMyGame : public CGame
{
	// Define sprites and other instance variables here
	CPlayer m_player;
	CSpiderList m_spiders;

	CSprite Startscreen;
	CSprite Playbutton;
	CSprite Quitbutton;
	CSprite Controlsbutton;
	CSprite Exitbutton;
	CSprite Controls;
	CSprite Demo;
	CSprite MenuButton;
	CSprite Explaination;
	CSprite OverDrive;
	CSpriteList shotList; 
	CSpriteList EnemyShotList;
	CSpriteList healthdrops;
	CSpriteList explosions; // list of explosion animations
	CGraphics m_background;
	//game variables
	int score;
	int enemycooldown;
	int Targetingtimer;
	bool insideenemy;
	bool insideplayer;
	
	// menu booleans
	bool isControls;
	//sound players
	CSoundPlayer hitsound; // plays when the player is hit
	CSoundPlayer Healthdropsound; // plays when the health drop is picked up
	CSoundPlayer explosionsound; // plays when the explosion is played
	CSoundPlayer MenuMusic; // plays the menu music
	CSoundPlayer GameMusic; // plays the game music	
	CSoundPlayer GameOverMusic; // plays the game over music
	CSoundPlayer MenuSelect; // plays the menu select sound
	CSoundPlayer PauseSound; // plays the pause sound
public:
	CMyGame(void);
	~CMyGame(void);

	// Per-Frame Callback Funtions (must be implemented!)
	virtual void OnUpdate();
	virtual void OnDraw(CGraphics* g);

	// Game Life Cycle
	virtual void OnInitialize();
	virtual void OnDisplayMenu();
	virtual void OnStartGame();
	virtual void OnStartLevel(Sint16 nLevel);
	virtual void OnGameOver();
	virtual void OnTerminate();

	// Keyboard Event Handlers
	virtual void OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode);
	virtual void OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode);

	// Mouse Events Handlers
	virtual void OnMouseMove(Uint16 x,Uint16 y,Sint16 relx,Sint16 rely,bool bLeft,bool bRight,bool bMiddle);
	virtual void OnLButtonDown(Uint16 x,Uint16 y);
	virtual void OnLButtonUp(Uint16 x,Uint16 y);
	virtual void OnRButtonDown(Uint16 x,Uint16 y);
	virtual void OnRButtonUp(Uint16 x,Uint16 y);
	virtual void OnMButtonDown(Uint16 x,Uint16 y);
	virtual void OnMButtonUp(Uint16 x,Uint16 y); 
	virtual void Spiderbehavior();
	virtual void Shotbehavior();
	virtual void Fixcrash();
	virtual void healthdropbehavior();
	virtual void Demobehavior();
};
