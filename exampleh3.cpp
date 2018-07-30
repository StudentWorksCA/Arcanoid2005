/*--------------------------------------------------------------------------
//Arkanoid 2004
	by MAYOR systems											
--------------------------------------------------------------------------*/

//------ Include Files ------//
#define INITGUID
#pragma comment(lib, "dxguid.lib") 

#include "stdafx.h"
#include <stdio.h>  
#include <conio.h>
#include <ddraw.h> 
#include <dinput.h>  
#include <mmsystem.h>
#include <dsound.h>
#include "class.h"
#include "enemy.h"       
#include "ball.h"  
#include "phisics.h"
#include "landscape.h"
#include "window.h" 
#include "resource.h"
#include "player.h"
#include "upgrade.h"
#include "Wave.h" 

//------ Window Class Information ------//

static char szClass[] = "Arkanoid";
static char szCaption[] = "Arkanoid by MAYOR systems";

#define KEYDOWN(buf,key) (buf[key] && 0x80)


// should not be duplicate defininions!
// 1-for screens
// 2-for schemas
#define LOGO           1001 //	1
#define GAME           2001 //	1 
#define MAIN           3001 //	1
#define OPTIONS        4001 //	1
#define ONEPLAYER      5001 //	1
#define TWOPLAYER      6001 //	1
#define CONTROLS       7001
#define SCORES         8001
						 
#define SPASE_SCHEMA   1002 //	2
#define NATURE_SCHEMA  2002 //	2
#define WATER_SCHEMA   3002 //	2

#define EASY           1003 //  3
#define MEDIUM         2003 //  3
#define HARD           3003 //  3

#define CAMPAIGN       1004 //  4
#define USERBUILT      2004 //  4
#define RANDOM         3004 //  4


#define EMPTY				0 //+
#define WEAK_BRICK			1 //+
#define MEDIUM_BRICK		2 //+red
#define CR1_MEDIUM_BRICK    3 //+red
#define STRONG_BRICK		4 //+
#define CR1_STRONG_BRICK    5 //+
#define	CR2_STRONG_BRICK    6 //+
#define SPECIAL_BRICK		7 //
#define UNDESTRUCTABLE		8 //

#define SPEEDUP             1
#define SPEEDDOWN           2
#define SIZEUP              3
#define SIZEDOWN            4
#define POWERBALL           5
#define MULTIBALL           6

#define MAX_BALLS 20

#define NUMSOUNDS 1

//------ Global Interface Pointers ------//

LPDIRECTDRAW4 lpDD=NULL;
LPDIRECTDRAWSURFACE4 lpDDSPrimary=NULL;
LPDIRECTDRAWSURFACE4 lpDDSBack=NULL;
LPDIRECTDRAWSURFACE4 supp=NULL;
LPDIRECTINPUT lpDI=NULL;
LPDIRECTINPUTDEVICE pKeyboard=NULL;
LPDIRECTINPUTDEVICE pMouse=NULL;


//------ DirectSound Object and Buffers ------//

LPDIRECTSOUND       lpDS;
LPDIRECTSOUNDBUFFER lpDSBSounds[NUMSOUNDS];

//------ Surfaces for Image Storage ------//

animObject paddle, cur, bg,bg_blue,bg_green, brick, logo, moon, font, print_cursor, repair, 
powerball_upgrade, tree, cloud, water;
animObject a1, a2;
enemy ufo;
landscape field;
player *player1, *player2;
Button theScreen;
int gameOver=0;
int quitYN=0;

//main menu::::
animObject bg_mm, decor_paddle, decor_bricks, decor_logo;
Button one_pl_btn, two_pl_btn, scores_btn, quit_btn, opt_btn;
Ball decor_ball;
int decor_daddle_delta=2;
int tree_delta=0;
double water_delta=0;
double tree_deltaY[10];
//::::main menu

//one player menu::::
animObject header_one_pl, capt1_one_pl, capt2_one_pl, capt3_one_pl,name_field_one_pl;
Button easy_btn, medium_btn, hard_btn, campaign_btn, userbuilt_btn, random_btn, back_btn, play_btn;
//::::one player menu

//one player menu::::
animObject header_two_pl, name_field_two_pl;
//::::one player menu

//options::
animObject header_options, capt1_options;
Button schema_space_btn, schema_ocean_btn,schema_nature_btn, 
		controls_btn, options_back_btn;

//::options

//controls:::
animObject capt1_controls,controls_devider;
Button controls_1player_btn,controls_2player_btn,
		LEFT_change_btn, RIGHT_change_btn, FIRE_change_btn,
		controls_back_btn, control_usemouse_btn;
int cur_change_player=1;
int cur_change_contr=0;
//::controls

//scores::
animObject scores_header,scores_table;
Button scores_clear_btn,scores_back_btn;

//::scoers

//powerups:
upgrade ballSpeedUp,ballSpeedDown, paddleSizeUp, paddleSizeDown, ballIsPowerball, ballMultiBall;
vector <upgrade *> m_upgrades;

int rX, rY, rD; //resulution, that is set set in dialog, then passed into Init(...)
//-------Other-------//
DWORD                   dwFrames = 0;       // The frame rate (frames/second).
int mx, my;
BOOL buttonDown[2];
DWORD m_dwLastTick;
int pause;

int curWindow;
int curSchema;
int cur_name_field=1;
int curDifficulty=EASY;
int curLevConc=CAMPAIGN;
bool isTwoPlayerGame;
int curLevel=1;
int demoShot=0;

////////////////////
int numLevels=20;
///////////////////


int emptyNameFlag=0;
animObject emptyName, lvlComplObj;


object jupBG1, europa;
int mvJup, curJup;

int outc;

int showtext(double num);
int showtext(char in);
int drawBackground(object bg);
int dispalyLogo();
int dispalyMainGameScreen();
int dispalyStartOnePlayer();
int dispalyStartTwoPlayer();
int displayMainMenu();
int dispalyOptions();
int displayControls();
int displayScores();
int drawText(char * text, double x, double y);
int getTextWidth(char * text);
BOOL loadAllgraphics(int schema);
int drawSpaceBg();
int drawNatureBg();
int drawOceanBg();
BOOL load_sounds();
void recordScore();

struct record
{
public:
	char name[50];
	int score;
};
record scores[5];


 

//------ Error Return String ------//

const char *ErrStr=NULL;	

//------ Error Messages ------//

const char Err_Reg_Class[]			= "Error Registering Window Class";
const char Err_Create_Win[]			= "Error Creating Window";
const char Err_DirectDrawCreate[]	= "DirectDrawCreate FAILED";
const char Err_Query[]				= "QueryInterface FAILED";
const char Err_Coop[]				= "SetCooperativeLevel FAILED";
const char Err_CreateSurf[]			= "CreateSurface FAILED";
const char Err_LoadBMP[]			= "Error Loading Image";
const char Err_DispMode[]			= "Error Setting Display Mode, this mode is not supported by your video card";
const char Err_LoadImage[]			= "Error Loading Image, please re-install game";
const char Err_LoadLevel[]			= "Some level file(s) are corrupted or missing, please re-install game";
const char Err_CreateBuff[]			= "CreateBuff FAILED";
const char Err_LoadWAV[]			= "LoadWAV FAILED";
const char Err_DirectSoundCreate[]	= "DirectSoundCreate FAILED";
//------ Function to Load a Bitmap into a DirectDraw Surface ------//



///------ Clean-up Function to Release Objects ------///

#define SafeRelease(x) if (x) { x->Release(); x=NULL;}

void Cleanup()
{


	//SafeRelease(lpDDSPrimary);
	SafeRelease(lpDDSBack);

	SafeRelease(lpDD);

	if (ErrStr) 
	{
		MessageBox(NULL, ErrStr, szCaption, MB_OK);
		ErrStr=NULL;
	}
}

//------ Function to Draw a Slide ------//

int draw_slide()
{

	if( pause==1){
		return 0;
	}
	
	//drawBackground(bg);

	if(curWindow==LOGO)
	{
		drawBackground(bg);
		if(logo.hasTimePassed(3000)){
			curWindow=MAIN;
		}else{
			dispalyLogo();
			//return 0;
		}
	} 
	else if(curWindow==GAME)
	{
		if(curSchema==SPASE_SCHEMA)
			drawSpaceBg();
		else if(curSchema==NATURE_SCHEMA)
			drawNatureBg();
		else if(curSchema==WATER_SCHEMA)
			drawOceanBg();
		else
			drawSpaceBg();
		if(gameOver==1)
		{
			drawText("game over", object::resX/2-getTextWidth("game over")/2, object::resY/2);
			drawText("press y to quit", object::resX/2-getTextWidth("press y to quit")/2,object::resY/2+30);  

		}
		else
			dispalyMainGameScreen();
	}
	else if(curWindow==MAIN)
	{
		displayMainMenu();
	}
	else if(curWindow==OPTIONS)
	{
		dispalyOptions();
	}
	else if(curWindow==ONEPLAYER)
	{
		dispalyStartOnePlayer();
	}
	else if(curWindow==TWOPLAYER)
	{
		dispalyStartTwoPlayer();
	}
	else if(curWindow==CONTROLS)
	{
		displayControls();
	}
	else if(curWindow==SCORES)
	{
		displayScores();
	}

	// flip to the primary surface
	lpDDSPrimary->Flip(0,DDFLIP_WAIT); 

	return 1;
	

}

int dispalyMainGameScreen()
{

	DWORD  m_dwCurrTick = timeGetTime();
    DWORD  temp         = m_dwCurrTick - m_dwLastTick;
	double howLong      = double(temp)/1000;
	m_dwLastTick        = m_dwCurrTick;
	bool levelComplete=true;

	int  i, j;
	RECT r;
	r.left = 0;r.top = 0;r.right = long(object.resX) ;r.bottom = long(object.resY);
	
	if(quitYN)
	{
		drawText("press y to quit", object::resX/2-getTextWidth("press y to quit")/2,object::resY/2);  
		return 0;
	}
	//draw correct bricks in correct positions && collision detection
	for(i=0;i<field.getHmY();i++)
	{
		for(j=0;j<field.getHmX() ;j++)
		{
			if(field.getTypeAt(i,j)==0)
				continue;

			//set levelComplete flag to false, if something left
			//since  ==0 check is done above, we are sure that it is not EMPTY
			if(field.getTypeAt(i,j) != UNDESTRUCTABLE)
				levelComplete=false;
			
			//get coords using getXAt() and getYAt() of landscape class
			brick.setCoord(field.getXAt(j,brick), field.getYAt(i, brick));

			//COLLISION DETECTION!!!
			//player 1:
			for(int h=0; h<player1->getHmBalls() ; h++)
			{
				if(isCollide(player1->getBall(h), &brick))
				{
					if(!player1->getBall(h)->isPowerBall() || 
						field.getTypeAt(i,j) == UNDESTRUCTABLE)
						updateBrickCollision(player1->getBall(h), &brick);

					DWORD dwStatus;
					lpDSBSounds[0]->GetStatus(&dwStatus);
					if ((dwStatus & DSBSTATUS_PLAYING)) 
					{
						//lpDSBSounds[0]->Stop();
						lpDSBSounds[0]->SetCurrentPosition(0);
					}
					else
					lpDSBSounds[0]->Play(0, 0, 0);
					
					switch(field.getTypeAt(i,j))
					{
					case WEAK_BRICK:
					case CR1_MEDIUM_BRICK:
					case CR2_STRONG_BRICK:
						field.setTypeAt(0,i,j); 
						player1->increaseScoreBy(10) ; 
						break;
					case MEDIUM_BRICK:
						field.setTypeAt(CR1_MEDIUM_BRICK,i,j);
						player1->increaseScoreBy(20); 
						break;
					case STRONG_BRICK:
						field.setTypeAt(CR1_STRONG_BRICK,i,j);
						player1->increaseScoreBy(50); 
						break;
					case CR1_STRONG_BRICK:
						field.setTypeAt(CR2_STRONG_BRICK,i,j);
						player1->increaseScoreBy(20);
						break;
					case SPECIAL_BRICK:
						{
							field.setTypeAt(0,i,j);
						
							int n = getRandom(1,7);// 7 is max powerup (MULTIBALL)
							upgrade *tempU; 
							switch(n)
							{
							case SPEEDUP:   
								tempU= new upgrade(ballSpeedUp); 
								break;
							case SPEEDDOWN: 
								tempU= new upgrade(ballSpeedDown);
								break;
							case SIZEUP:    
								tempU= new upgrade(paddleSizeUp);
								break;
							case SIZEDOWN:  
								tempU= new upgrade(paddleSizeDown);
								break;
							case POWERBALL:  
								tempU= new upgrade(ballIsPowerball);
								break;
							case MULTIBALL:
								tempU= new upgrade(ballMultiBall);
								break;
							default: //just in case:
								tempU= new upgrade(paddleSizeDown);
								break;
							}
							tempU->setCoord(field.getXAt(j,brick), field.getYAt(i, brick)); 
							m_upgrades.push_back(tempU);
								
						}
						player1->increaseScoreBy(30);
						break;
					}
				}
					//if(updateBrickCollision(player1->getBall(h), &ufo))
					//	turnVector(&(enemy)ufo,30);
			}
			//player2:
			if(isTwoPlayerGame)
			for(int l=0; l<player2->getHmBalls(); l++)
			{ 
				
				if(isCollide(player2->getBall(l), &brick))
				{
					if(!player2->getBall(l)->isPowerBall() || 
						field.getTypeAt(i,j) == UNDESTRUCTABLE)
						updateBrickCollision(player2->getBall(l), &brick);

					DWORD dwStatus;
					lpDSBSounds[0]->GetStatus(&dwStatus);
					if ((dwStatus & DSBSTATUS_PLAYING)) 
					{
						//lpDSBSounds[0]->Stop();
						lpDSBSounds[0]->SetCurrentPosition(0);
					}
					else
					lpDSBSounds[0]->Play(0, 0, 0);


					switch(field.getTypeAt(i,j))
					{
					case WEAK_BRICK:
					case CR1_MEDIUM_BRICK:
					case CR2_STRONG_BRICK:
						field.setTypeAt(0,i,j); 
						player2->increaseScoreBy(10); 
						break;
					case MEDIUM_BRICK:
						field.setTypeAt(CR1_MEDIUM_BRICK,i,j);
						player2->increaseScoreBy(20); 
						break;
					case STRONG_BRICK:
						field.setTypeAt(CR1_STRONG_BRICK,i,j);
						player2->increaseScoreBy(50); 
						break;
					case CR1_STRONG_BRICK:
						field.setTypeAt(CR2_STRONG_BRICK,i,j);
						player2->increaseScoreBy(20); 
						break;
					case SPECIAL_BRICK:
						{
							field.setTypeAt(0,i,j);

							int n = getRandom(1,7);// 7 is max powerup (MULTIBALL)
							upgrade *tempU; 
							switch(n)
							{
							case SPEEDUP:   
								tempU= new upgrade(ballSpeedUp); 
								//tempU->setType(1); 
								break;
							case SPEEDDOWN: 
								tempU= new upgrade(ballSpeedDown);
								//tempU->setType(2); 
								break;
							case SIZEUP:    
								tempU= new upgrade(paddleSizeUp);
								//tempU->setType(3); 
								break;
							case SIZEDOWN:  
								tempU= new upgrade(paddleSizeDown);
								//tempU->setType(4); 
								break;
							case POWERBALL:  
								tempU= new upgrade(ballIsPowerball);
								//tempU->setType(5); 
								break;
							case MULTIBALL:
								tempU= new upgrade(ballMultiBall);
								break;
							default://just in case:
								tempU= new upgrade(paddleSizeDown);
								//tempU->setType(4); 
								break;
							}
							tempU->setCoord(field.getXAt(j,brick), field.getYAt(i, brick)); 
							m_upgrades.push_back(tempU);
							
						}
						player2->increaseScoreBy(30); 
						break;
					}
				}
			}

			//draw bricks considering their types
			if(field.getTypeAt(i,j)==WEAK_BRICK) //regular brick
			{			
				lpDDSBack->Blt(&brick.getCRect(), brick.getObjSurf(WEAK_BRICK), 
					&brick.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
			}
			else if(field.getTypeAt(i,j)==MEDIUM_BRICK) //medium brick
			{			
				lpDDSBack->Blt(&brick.getCRect(), brick.getObjSurf(MEDIUM_BRICK), 
					&brick.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
			}
			else if(field.getTypeAt(i,j)==CR1_MEDIUM_BRICK) //special brick
			{			
				lpDDSBack->Blt(&brick.getCRect(), brick.getObjSurf(CR1_MEDIUM_BRICK), 
					&brick.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
			}
			else if(field.getTypeAt(i,j)==STRONG_BRICK) //special brick
			{			
				lpDDSBack->Blt(&brick.getCRect(), brick.getObjSurf(STRONG_BRICK), 
					&brick.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
			}
			else if(field.getTypeAt(i,j)==CR1_STRONG_BRICK) //special brick
			{			
				lpDDSBack->Blt(&brick.getCRect(), brick.getObjSurf(CR1_STRONG_BRICK), 
					&brick.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
			}
			else if(field.getTypeAt(i,j)==CR2_STRONG_BRICK) //special brick
			{			
				lpDDSBack->Blt(&brick.getCRect(), brick.getObjSurf(CR2_STRONG_BRICK), 
					&brick.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
			}
			else if(field.getTypeAt(i,j)==SPECIAL_BRICK) //special brick
			{			
				lpDDSBack->Blt(&brick.getCRect(), brick.getObjSurf(SPECIAL_BRICK), 
					&brick.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
			}
			else if(field.getTypeAt(i,j)==UNDESTRUCTABLE) //special brick
			{			
				lpDDSBack->Blt(&brick.getCRect(), brick.getObjSurf(UNDESTRUCTABLE), 
					&brick.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
			}
		}
	}//end drawing bricks and collision detection
	

	if(levelComplete && lvlComplObj.getType()!=-1)
	{
		lvlComplObj.setLastTick(timeGetTime()); 
		lvlComplObj.setType(-1); 
	}
	//player1 paddle
	if(!player1->isGameOver())
	{
		if(!player1->isFrozen())
		{
			if(player1->isUsingMouse())
				player1->getPaddle()->setCoord(mx, object::resY - 20 );
			else 
				player1->getPaddle()->setCoord(player1->getPaddle()->getCurX(), object::resY - 20 );
		}
		updatePositionOf(player1->getPaddle());
		lpDDSBack->Blt(&player1->getPaddle()->getCRect(), player1->getPaddle()->getObjSurf(0), 
			&player1->getPaddle()->getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
	}

	//palyer 2 paddle
	if(isTwoPlayerGame && !player2->isGameOver())
	{
		if(!player2->isFrozen())
		{
			if(player2->isUsingMouse())
				player2->getPaddle()->setCoord(mx, object::resY - 20 -player2->getPaddle()->getConstSizeY());
			else
				player2->getPaddle()->setCoord(player2->getPaddle()->getCurX(), object::resY - 20 -player2->getPaddle()->getConstSizeY());
		}
		updatePositionOf(player2->getPaddle());
		lpDDSBack->Blt(&player2->getPaddle()->getCRect(), player2->getPaddle()->getObjSurf(0), 
			&player2->getPaddle()->getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
	}
	//add balls if screen is ckicked
	theScreen.setConstSizeX(object::resX); 
	theScreen.setConstSizeY(object::resY);
	if(theScreen.isClicked(buttonDown[0], mx, my))
	{
		if(player1->isUsingMouse())
		{
			if(player1->getBallsLeft()>0)
			{
				player1->addBall(); 
				player1->getBall(player1->getHmBalls()-1)->setUnitXY(cosDeg(90),-sinDeg(90));
				player1->getBall(player1->getHmBalls()-1)->setCoord(player1->getPaddle()->getCurX()+player1->getPaddle()->getConstSizeX()/2,player1->getPaddle()->getCurY()-player1->getBall(0)->getConstSizeY()-1);
				player1->getBall(player1->getHmBalls()-1)->setDelay(0.01);
				player1->getBall(player1->getHmBalls()-1)->setSpeed(4);
				if(player1->getBallsLeft()-1>=0)
					player1->setBallsLeft(player1->getBallsLeft()-1);
			}

		}
		else
		{
			if(player2->getBallsLeft()>0)
			{
				player2->addBall(); 
				player2->getBall(player2->getHmBalls()-1)->setUnitXY(cosDeg(90),-sinDeg(90));
				player2->getBall(player2->getHmBalls()-1)->setCoord(player2->getPaddle()->getCurX()+player2->getPaddle()->getConstSizeX()/2,player2->getPaddle()->getCurY()-player2->getBall(0)->getConstSizeY()-1);
				player2->getBall(player2->getHmBalls()-1)->setDelay(0.01);
				player2->getBall(player2->getHmBalls()-1)->setSpeed(4);
				if(player2->getBallsLeft()-1>=0)
					player2->setBallsLeft(player2->getBallsLeft()-1);
			}
		}
	}

	//player1 balls:
	for(i=0;i<player1->getHmBalls() ;i++)
	{
		if(player1->getBall(i)->hasTimePassed(100))
			player1->getBall(i)->goNextImage();
			
		player1->getBall(i)->setDelay(howLong); 
		if(  !updateVectorOf(player1->getBall(i), player1->getPaddle(),true) ||
			(isTwoPlayerGame && !player2->isGameOver() &&  !updateVectorOf(player1->getBall(i), player2->getPaddle(),true) )
		  )
		{
			player1->removeBall(i);

			if(player1->getHmBalls()==0 && player1->getBallsLeft()==0)
				player1->setGameOver(1);
			
			continue; //<- not to go to the rest of for loop, since this element is deleted
		}
		player1->getBall(i)->setCoord(player1->getBall(i)->getCurX()+ player1->getBall(i)->getStepX(), player1->getBall(i)->getCurY()+ player1->getBall(i)->getStepY());
		lpDDSBack->Blt(&player1->getBall(i)->getCRect(), player1->getBall(i)->getObjSurf(player1->getBall(i)->getCurImage()), &player1->getBall(i)->getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
	}
	//player2 balls:
	if(isTwoPlayerGame)
	for(i=0;i<player2->getHmBalls() ;i++)
	{
		if(player2->getBall(i)->hasTimePassed(100))
			player2->getBall(i)->goNextImage();
			
		player2->getBall(i)->setDelay(howLong); 
		if((!player1->isGameOver() && !updateVectorOf(player2->getBall(i), player1->getPaddle(), true)) ||
			!updateVectorOf(player2->getBall(i), player2->getPaddle(),true))
		{
			player2->removeBall(i);

			if(player2->getHmBalls()==0 && player2->getBallsLeft()==0)
				player2->setGameOver(1);
			 
			continue; //<- not to go to the rest of for loop, since this element is deleted
		}
		player2->getBall(i)->setCoord(player2->getBall(i)->getCurX()+ player2->getBall(i)->getStepX(), player2->getBall(i)->getCurY()+ player2->getBall(i)->getStepY());
		lpDDSBack->Blt(&player2->getBall(i)->getCRect(), player2->getBall(i)->getObjSurf(player2->getBall(i)->getCurImage()), &player2->getBall(i)->getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
	}
	//::end draw balls
	
	//////UFO:::::::::
	if(ufo.hasTimePassed(200))
		ufo.goNextImage();
		
	if(ufo.IsRandomSuccess(200))
	turnVector(&ufo, rand()*100+1);
	ufo.setDelay(howLong); 
	updateVectorOf(&ufo, player1->getPaddle(), false);
	if(isTwoPlayerGame)
	updateVectorOf(&ufo, player2->getPaddle(), false);
	ufo.setCoord(ufo.getCurX()+ ufo.getStepX(), ufo.getCurY()+ ufo.getStepY());
	lpDDSBack->Blt(&ufo.getCRect(), ufo.getObjSurf(ufo.getCurImage()), &ufo.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
	//::::::UFO


	//BULLET::::::::::::
	int randSuc = 0;
	switch(curDifficulty)
	{
		case EASY: randSuc = 900; break;
		case MEDIUM: randSuc = 600; break;
		case HARD: randSuc = 300; break;
	}
	//set which padle to shoot and shoot it!
	if(!ufo.isShooting() && (ufo.IsRandomSuccess(randSuc)||demoShot==1))
	{
		demoShot=0;
		//which paddle based on frozeness and respect :-)
		if(isTwoPlayerGame && !player2->isGameOver())
		{
			if(!player1->isFrozen() && !player2->isFrozen())
			{
				if(rand()%10>4 && !player1->isGameOver()) 
					ufo.setPaddleToShoot(1);
				else
					ufo.setPaddleToShoot(2);
			}
			else if(!player1->isFrozen() && !player1->isGameOver()  && player2->isFrozen())
				ufo.setPaddleToShoot(1);
			else if(player1->isFrozen() && !player2->isFrozen() && !player2->isGameOver() )
				ufo.setPaddleToShoot(2);
		}
		else ufo.setPaddleToShoot(1);

		ufo.shoot();
		
		//set vector of bullet
		if(ufo.getPaddleToShoot()==1)
			ufo.letBulletFolowPaddle(player1->getPaddle()) ;
		else if(ufo.getPaddleToShoot()==2)
			ufo.letBulletFolowPaddle(player2->getPaddle()) ;
	}
	
	//end shot if bullet out of screen
	if(ufo.getBullet()->getCurX()<=0 || ufo.getBullet()->getCurX()>=object::resX ||
		ufo.getBullet()->getCurY()<=0 || ufo.getBullet()->getCurY()>=object::resY)
	{
		ufo.endShot(); 
		//drawText("not shooting", 300, 250);
	}

	//display repair sign on paddle 1
	if(player1->isFrozen() )
	{
		repair.setCoord(player1->getPaddle()->getCurX()+player1->getPaddle()->getConstSizeX()/2-repair.getConstSizeX()/2,
				player1->getPaddle()->getCurY()+player1->getPaddle()->getConstSizeY()/2-repair.getConstSizeY()/2); 
		lpDDSBack->Blt(&repair.getCRect(), repair.getObjSurf(0), &repair.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);

	}
	//display repair sign on paddle 2
	if(player2->isFrozen() && isTwoPlayerGame)
	{
		repair.setCoord(player2->getPaddle()->getCurX()+player2->getPaddle()->getConstSizeX()/2-repair.getConstSizeX()/2,
				player2->getPaddle()->getCurY()+player2->getPaddle()->getConstSizeY()/2-repair.getConstSizeY()/2); 
		lpDDSBack->Blt(&repair.getCRect(), repair.getObjSurf(0), &repair.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
	}

	// player 1 freese
	if(player1->getPaddle()->isInRect(ufo.getBullet()->getCurX(),ufo.getBullet()->getCurY()))
	{
		player1->freeze();
		player1->decreaseScoreBy(30); 
		// a1 is just empty animObject used to get its hasTimePassed() func
		a1.setLastTick(timeGetTime());// <- very important!! (a is just animObjectused to get its hasTimePassed() func)
		ufo.endShot(); 
	}
	//player 2 freeze
	if(isTwoPlayerGame && player2->getPaddle()->isInRect(ufo.getBullet()->getCurX(),ufo.getBullet()->getCurY()))
	{
		player2->freeze();
		player2->decreaseScoreBy(30); 
		a2.setLastTick(timeGetTime()); 
		ufo.endShot(); 
	}

	// unfreeze
	if(player1->isFrozen() && a1.hasTimePassed(3000))
		player1->unfreeze();
	if(player2->isFrozen() && a2.hasTimePassed(3000) && isTwoPlayerGame)
		player2->unfreeze();

	//move buttet
	if(ufo.isShooting())
	{
		//this if to make bullet follow paddle only on HARD
		if(curDifficulty == HARD)
		{
			if(ufo.getPaddleToShoot()==1)
				ufo.letBulletFolowPaddle(player1->getPaddle());
			else if(ufo.getPaddleToShoot()==2)
				ufo.letBulletFolowPaddle(player2->getPaddle());
		}
		ufo.getBullet()->setSpeed(4);
		ufo.getBullet()->setCoord(ufo.getBullet()->getCurX()+ufo.getBullet()->getStepX(), ufo.getBullet()->getCurY()+ufo.getBullet()->getStepY());
		lpDDSBack->Blt(&ufo.getBullet()->getCRect(), ufo.getBullet()->getObjSurf(ufo.getBullet()->getCurImage()), &ufo.getBullet()->getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);

	}
	//::::::::::BULLET


	//WRITING:::::::::::
	char buff[255];
	sprintf(buff, "%d", player1->getScore()); 
	if(player1->isGameOver())
		drawText("game over",5,0);
	else
		drawText(player1->getName(),5,0);
	if(int(getTextWidth(player1->getName())/2-getTextWidth(buff)/2)<=0)
		drawText(buff , 0 ,30);
	else
		drawText(buff , int(getTextWidth(player1->getName())/2-getTextWidth(buff)/2)  ,30);
	sprintf(buff, "%d balls", player1->getBallsLeft());
	drawText(buff , 5 ,60);
	
	if(isTwoPlayerGame)
	{
		sprintf(buff, "%d", player2->getScore()); 
		if(player2->isGameOver())
			drawText("game over",object::resX - getTextWidth("game over") ,0);
		else
			drawText(player2->getName(),object::resX - getTextWidth(player2->getName()) ,0);
		if(int(object::resX - getTextWidth(player2->getName())/2+getTextWidth(buff)/2)>=object::resX)
			drawText(buff , int(object::resX-getTextWidth(buff))  ,30);
		else
			drawText(buff , int(object::resX - getTextWidth(player2->getName())/2-getTextWidth(buff)/2)  ,30);
		sprintf(buff, "%d balls", player2->getBallsLeft());
		drawText(buff , int(object::resX-getTextWidth(buff)) ,60);
	}


	//level complete
	if(levelComplete)
	{	

		if(!lvlComplObj.hasTimePassed(3000))
		{
			if(curLevel <numLevels)
				drawText("level completed", object::resX/2-getTextWidth("level completed")/2, object::resY/2);
			else
			{
				drawText("campaign completed", object::resX/2-getTextWidth("campaign completed")/2, object::resY/2);
				drawText("congratulations", object::resX/2-getTextWidth("congratulations")/2, object::resY/2+40);

				if(player1->getScore()>player2->getScore())
				{
					char buff[100];
					sprintf(buff, "%s won",player1->getName() );
					drawText(buff, object::resX/2-getTextWidth(buff)/2, object::resY*2/3);
					drawText("campaign completed", object::resX/2-getTextWidth("campaign completed")/2, object::resY/2);
				}
				if(player2->getScore()>player1->getScore())
				{
					char buff[100];
					sprintf(buff, "%s won",player2->getName() );
					drawText(buff, object::resX/2-getTextWidth(buff)/2, object::resY*2/3);
					drawText("campaign completed", object::resX/2-getTextWidth("campaign completed")/2, object::resY/2);
				}

			}
		}
		else
		{
			lvlComplObj.setType(0); 
			
			if(curLevel >= numLevels)
			{
				recordScore();

				player1->setScore(0);
				player2->setScore(0); 
				player1->setGameOver(0); 
				player2->setGameOver(0); 


				curWindow = MAIN;
			}
			else
			{
				if(curLevel<=numLevels)
				curLevel++;
				char buff[50];
				sprintf(buff, "level%d.map",curLevel);
				field.readF(buff);
				if(player1->getBallsLeft()+player1->getHmBalls()+1<=20)
					player1->initForNewLevel(player1->getBallsLeft()+player1->getHmBalls()+1);
				else
					player1->initForNewLevel(20);

				if(player2->getBallsLeft()+player2->getHmBalls()+1<=20)
					player2->initForNewLevel(player2->getBallsLeft()+player2->getHmBalls()+1); 
				else
					player2->initForNewLevel(20); 

			}
		}
	}
	//:::::::::WRITING

	//POWERUPS:::::
	for(int f=0;f<m_upgrades.size();f++)
	{

		if(m_upgrades.at(f)->hasTimePassed(100))
			m_upgrades.at(f)->goNextImage();
		m_upgrades.at(f)->setCoord(m_upgrades.at(f)->getCurX(),m_upgrades.at(f)->getCurY()+2);
		if(player1->getPaddle()->isInRect(m_upgrades.at(f)->getCurX()+m_upgrades.at(f)->getConstSizeX()/2 , m_upgrades.at(f)->getCurY() + m_upgrades.at(f)->getConstSizeY() ))
		{
			player1->addPowerUp(m_upgrades.at(f));
			m_upgrades.erase(m_upgrades.begin()+f);
			continue;
		}
		if(player2->getPaddle()->isInRect(m_upgrades.at(f)->getCurX()+m_upgrades.at(f)->getConstSizeX()/2 , m_upgrades.at(f)->getCurY() + m_upgrades.at(f)->getConstSizeY() ))
		{
			player2->addPowerUp(m_upgrades.at(f));
			m_upgrades.erase(m_upgrades.begin()+f);
			continue;
		}
		if(m_upgrades.at(f)->getCurY()>object::resY)
		{
			m_upgrades.erase(m_upgrades.begin()+f);
			continue;
		}
		lpDDSBack->Blt(&m_upgrades.at(f)->getCRect(), m_upgrades.at(f)->getObjSurf(m_upgrades.at(f)->getCurImage() ), &m_upgrades.at(f)->getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);


	}
	//::::POWERUPS
	
	if(mx<=0)mx=0;
	if(mx+player1->getPaddle()->getConstSizeX()>=object::resX) mx=object::resX -player1->getPaddle()->getConstSizeX();
	//game over:
	if(player1->isGameOver() && !isTwoPlayerGame)
		gameOver=1;
	if(player1->isGameOver() && isTwoPlayerGame && player2->isGameOver())
		gameOver=1;
	//showtext(double(player1->getHmBalls()));//double(hmBalls)); 
	return 0;

}

void recordScore()
{
		int temp;
		char c[50];
		if(player1->getScore()>player2->getScore())
		{
			temp=player1->getScore();
			strcpy(c,player1->getName()); 
		}
		else
		{
			temp=player2->getScore();
			strcpy(c,player2->getName()); 
		}
		if(temp>scores[0].score )
		{
			scores[4].score=scores[3].score; 
			scores[3].score=scores[2].score;
			scores[2].score=scores[1].score;
			scores[1].score=scores[0].score;
			scores[0].score=temp;

			strcpy(scores[4].name,scores[3].name); 
			strcpy(scores[3].name,scores[2].name); 
			strcpy(scores[2].name,scores[1].name); 
			strcpy(scores[1].name,scores[0].name); 
			strcpy(scores[0].name,c); 

		}
		FILE *f;
		f=fopen("scores.txt","w");
		for(int z=0;z<5;z++)
			fprintf(f,"%s %d\n", scores[z].name, scores[z].score);

}

int dispalyLogo()
{
	//DWORD dwStatus;
    //lpDSBSounds[0]->GetStatus(&dwStatus);
    //if (!(dwStatus & DSBSTATUS_PLAYING)) 
	//{
	   
       //lpDSBSounds[0]->Play(0, 0, 0);
    //}
	logo.setCoord(logo.resX/2-logo.getCurSizeX()/2 , logo.resY/2-logo.getCurSizeY()/2);
	lpDDSBack->Blt(&logo.getCRect(), logo.getObjSurf(0), &logo.getRect(),  DDBLT_WAIT, NULL);
	return 0;
}

int dispalyStartOnePlayer()
{
	
	drawBackground(bg_mm);

	header_one_pl.setCoord(object::resX/2-header_one_pl.getConstSizeX()/2, object::resY/30); 
	lpDDSBack->Blt(&header_one_pl.getCRect(), header_one_pl.getObjSurf(0), &header_one_pl.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
	capt1_one_pl.setCoord(object::resX/4-capt1_one_pl.getConstSizeX(), object::resY/6); 
	lpDDSBack->Blt(&capt1_one_pl.getCRect(), capt1_one_pl.getObjSurf(0), &capt1_one_pl.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
	name_field_one_pl.setCoord(object::resX/4-capt1_one_pl.getConstSizeX(), object::resY/6+capt1_one_pl.getConstSizeY()*3/2 ); 
	lpDDSBack->Blt(&name_field_one_pl.getCRect(), name_field_one_pl.getObjSurf(0), &name_field_one_pl.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
	capt2_one_pl.setCoord(object::resX/4-capt1_one_pl.getConstSizeX(), object::resY/3+capt2_one_pl.getConstSizeY()/2 ); 
	lpDDSBack->Blt(&capt2_one_pl.getCRect(), capt2_one_pl.getObjSurf(0), &capt2_one_pl.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);

	easy_btn.setCoord(object::resX/4-capt1_one_pl.getConstSizeX(), capt2_one_pl.getCurY()+ capt2_one_pl.getConstSizeY()*3/2); 
	if(easy_btn.isInRect(mx,my) || curDifficulty==EASY)
	lpDDSBack->Blt(&easy_btn.getCRect(), easy_btn.getObjSurf(1), &easy_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
	else
	lpDDSBack->Blt(&easy_btn.getCRect(), easy_btn.getObjSurf(0), &easy_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);

	medium_btn.setCoord(object::resX/4-capt1_one_pl.getConstSizeX(), easy_btn.getCurY()+easy_btn.getConstSizeY()+ easy_btn.getConstSizeY()/5); 
	if(medium_btn.isInRect(mx,my) || curDifficulty==MEDIUM)
	lpDDSBack->Blt(&medium_btn.getCRect(), medium_btn.getObjSurf(1), &medium_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
	else
	lpDDSBack->Blt(&medium_btn.getCRect(), medium_btn.getObjSurf(0), &medium_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);

	hard_btn.setCoord(object::resX/4-capt1_one_pl.getConstSizeX(), medium_btn.getCurY()+medium_btn.getConstSizeY()+ medium_btn.getConstSizeY()/5); 
	if(hard_btn.isInRect(mx,my) || curDifficulty==HARD)
	lpDDSBack->Blt(&hard_btn.getCRect(), hard_btn.getObjSurf(1), &hard_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
	else
	lpDDSBack->Blt(&hard_btn.getCRect(), hard_btn.getObjSurf(0), &hard_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);

	capt3_one_pl.setCoord(object::resX/2+capt3_one_pl.getConstSizeX()/2, object::resY/3+capt2_one_pl.getConstSizeY()/2 ); 
	lpDDSBack->Blt(&capt3_one_pl.getCRect(), capt3_one_pl.getObjSurf(0), &capt3_one_pl.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);

	campaign_btn.setCoord(object::resX/2+capt3_one_pl.getConstSizeX()/2, capt2_one_pl.getCurY()+ capt2_one_pl.getConstSizeY()*3/2); 
	if(campaign_btn.isInRect(mx,my) || curLevConc == CAMPAIGN)
	lpDDSBack->Blt(&campaign_btn.getCRect(), campaign_btn.getObjSurf(1), &campaign_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
	else
	lpDDSBack->Blt(&campaign_btn.getCRect(), campaign_btn.getObjSurf(0), &campaign_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);

	userbuilt_btn.setCoord(object::resX/2+capt3_one_pl.getConstSizeX()/2, easy_btn.getCurY()+easy_btn.getConstSizeY()+ easy_btn.getConstSizeY()/5); 
	if(userbuilt_btn.isInRect(mx,my) || curLevConc == USERBUILT)
	lpDDSBack->Blt(&userbuilt_btn.getCRect(), userbuilt_btn.getObjSurf(1), &userbuilt_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
	else
	lpDDSBack->Blt(&userbuilt_btn.getCRect(), userbuilt_btn.getObjSurf(0), &userbuilt_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);

	random_btn.setCoord(object::resX/2+capt3_one_pl.getConstSizeX()/2, medium_btn.getCurY()+medium_btn.getConstSizeY()+ medium_btn.getConstSizeY()/5); 
	if(random_btn.isInRect(mx,my) || curLevConc == RANDOM)
	lpDDSBack->Blt(&random_btn.getCRect(), random_btn.getObjSurf(1), &random_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
	else
	lpDDSBack->Blt(&random_btn.getCRect(), random_btn.getObjSurf(0), &random_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);

	back_btn.setCoord(hard_btn.getCurX()+hard_btn.getConstSizeX()/2- back_btn.getConstSizeX()/2  , hard_btn.getCurY()+hard_btn.getConstSizeY()+ hard_btn.getConstSizeY()/3); 
	if(back_btn.isInRect(mx,my))
	lpDDSBack->Blt(&back_btn.getCRect(), back_btn.getObjSurf(1), &back_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
	else
	lpDDSBack->Blt(&back_btn.getCRect(), back_btn.getObjSurf(0), &back_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);

	play_btn.setCoord(random_btn.getCurX()+random_btn.getConstSizeX()/2- play_btn.getConstSizeX()/2  , hard_btn.getCurY()+hard_btn.getConstSizeY()+ hard_btn.getConstSizeY()/3); 
	if(play_btn.isInRect(mx,my))
	lpDDSBack->Blt(&play_btn.getCRect(), play_btn.getObjSurf(1), &play_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
	else
	lpDDSBack->Blt(&play_btn.getCRect(), play_btn.getObjSurf(0), &play_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);

	if(play_btn.isClicked(buttonDown[0], mx, my))
	{
		if(strlen(player1->getName())==0)
		{
			emptyNameFlag=1;
			emptyName.setLastTick(timeGetTime());
		}
		else
		{
			player1->initForNewLevel(3);
			player1->getPaddle()->setCoord(0, object::resY - 20 );
			player2->getPaddle()->setCoord(object::resX-player2->getPaddle()->getConstSizeX(), object::resY - 20 );
			curLevel=1;
			if(curLevConc==CAMPAIGN)
				field.readF("level1.map"); 
			else if(curLevConc==USERBUILT)
				field.readF("MyLevel.map"); 
			else if(curLevConc==RANDOM)
			{
				char buff[50];
				sprintf(buff,"Level%d.map",getRandom(1,19));
				field.readF(buff); 
			}

			ufo.endShot(); 
			m_upgrades.clear(); 
			curWindow=GAME;
			return 0;
		}
	}
	if(back_btn.isClicked(buttonDown[0], mx, my))
	{
		curWindow=MAIN;
		return 0;
	}

	if(easy_btn.isClicked(buttonDown[0], mx, my))
		curDifficulty = EASY; 
	if(medium_btn.isClicked(buttonDown[0], mx, my))
		curDifficulty = MEDIUM; 
	if(hard_btn.isClicked(buttonDown[0], mx, my))
		curDifficulty = HARD; 

	if(campaign_btn.isClicked(buttonDown[0], mx, my))
	{
		curLevConc = CAMPAIGN; 
		curLevel=1;
		numLevels=20;
	}
	if(userbuilt_btn.isClicked(buttonDown[0], mx, my))
	{
		curLevConc = USERBUILT; 
		curLevel=1;
		numLevels=1;
	}
	if(random_btn.isClicked(buttonDown[0], mx, my))
	{
		curLevConc = RANDOM;
		curLevel=1;
		numLevels=1;
	}

	//showtext(double(strlen(player1->getName())));
	if(emptyNameFlag)
	{
		if(!emptyName.hasTimePassed(2000))
			drawText("please enter your name", object::resX/2-getTextWidth("please enter your name")/2, object::resY/2);
		else
			emptyNameFlag=0;

	}		

	int lastCharPos=
		drawText(player1->getName(), name_field_one_pl.getCurX() +font.getConstSizeY() , name_field_one_pl.getCurY()+font.getConstSizeY()/3 );
	
	print_cursor.setCoord(lastCharPos, name_field_one_pl.getCurY()+font.getConstSizeY()/3); 
	lpDDSBack->Blt(&print_cursor.getCRect(), print_cursor.getObjSurf(0), &print_cursor.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);

	//print_cursor 

	//cursor::
	updateCoords(&mx, &my);
	cur.setCoord(mx,my);
	lpDDSBack->Blt(&cur.getCRect(), cur.getObjSurf(0), &cur.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
	


	return 0;
}

int dispalyStartTwoPlayer()
{
	drawBackground(bg_mm);

	header_two_pl.setCoord(object::resX/2-header_two_pl.getConstSizeX()/2, object::resY/30); 
	lpDDSBack->Blt(&header_two_pl.getCRect(), header_two_pl.getObjSurf(0), &header_two_pl.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
	capt1_one_pl.setCoord(object::resX/4-capt1_one_pl.getConstSizeX(), object::resY/8); 
	lpDDSBack->Blt(&capt1_one_pl.getCRect(), capt1_one_pl.getObjSurf(0), &capt1_one_pl.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
	
	name_field_one_pl.setCoord(object::resX/4-capt1_one_pl.getConstSizeX(), object::resY/8+capt1_one_pl.getConstSizeY()*5/4 ); 
	lpDDSBack->Blt(&name_field_one_pl.getCRect(), name_field_one_pl.getObjSurf(0), &name_field_one_pl.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
	
	name_field_two_pl.setCoord(object::resX/4-capt1_one_pl.getConstSizeX(), object::resY/8+capt1_one_pl.getConstSizeY()*5/4 + name_field_one_pl.getConstSizeY()); 
	lpDDSBack->Blt(&name_field_two_pl.getCRect(), name_field_two_pl.getObjSurf(0), &name_field_two_pl.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
	
	capt2_one_pl.setCoord(object::resX/4-capt1_one_pl.getConstSizeX(), object::resY/3+capt2_one_pl.getConstSizeY()*5/3 ); 
	lpDDSBack->Blt(&capt2_one_pl.getCRect(), capt2_one_pl.getObjSurf(0), &capt2_one_pl.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);

	easy_btn.setCoord(object::resX/4-capt1_one_pl.getConstSizeX(), capt2_one_pl.getCurY()+ capt2_one_pl.getConstSizeY()); 
	if(easy_btn.isInRect(mx,my) || curDifficulty == EASY)
	lpDDSBack->Blt(&easy_btn.getCRect(), easy_btn.getObjSurf(1), &easy_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
	else
	lpDDSBack->Blt(&easy_btn.getCRect(), easy_btn.getObjSurf(0), &easy_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);

	medium_btn.setCoord(object::resX/4-capt1_one_pl.getConstSizeX(), easy_btn.getCurY()+easy_btn.getConstSizeY()+ easy_btn.getConstSizeY()/5); 
	if(medium_btn.isInRect(mx,my) || curDifficulty == MEDIUM)
	lpDDSBack->Blt(&medium_btn.getCRect(), medium_btn.getObjSurf(1), &medium_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
	else
	lpDDSBack->Blt(&medium_btn.getCRect(), medium_btn.getObjSurf(0), &medium_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);

	hard_btn.setCoord(object::resX/4-capt1_one_pl.getConstSizeX(), medium_btn.getCurY()+medium_btn.getConstSizeY()+ medium_btn.getConstSizeY()/5); 
	if(hard_btn.isInRect(mx,my) || curDifficulty == HARD)
	lpDDSBack->Blt(&hard_btn.getCRect(), hard_btn.getObjSurf(1), &hard_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
	else
	lpDDSBack->Blt(&hard_btn.getCRect(), hard_btn.getObjSurf(0), &hard_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);

	capt3_one_pl.setCoord(object::resX/2+capt3_one_pl.getConstSizeX()/2, object::resY/3+capt2_one_pl.getConstSizeY()*5/3 ); 
	lpDDSBack->Blt(&capt3_one_pl.getCRect(), capt3_one_pl.getObjSurf(0), &capt3_one_pl.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);

	campaign_btn.setCoord(object::resX/2+capt3_one_pl.getConstSizeX()/2, capt2_one_pl.getCurY()+ capt2_one_pl.getConstSizeY()); 
	if(campaign_btn.isInRect(mx,my) || curLevConc == CAMPAIGN)
	lpDDSBack->Blt(&campaign_btn.getCRect(), campaign_btn.getObjSurf(1), &campaign_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
	else
	lpDDSBack->Blt(&campaign_btn.getCRect(), campaign_btn.getObjSurf(0), &campaign_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);

	userbuilt_btn.setCoord(object::resX/2+capt3_one_pl.getConstSizeX()/2, easy_btn.getCurY()+easy_btn.getConstSizeY()+ easy_btn.getConstSizeY()/5); 
	if(userbuilt_btn.isInRect(mx,my) || curLevConc == USERBUILT)
	lpDDSBack->Blt(&userbuilt_btn.getCRect(), userbuilt_btn.getObjSurf(1), &userbuilt_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
	else
	lpDDSBack->Blt(&userbuilt_btn.getCRect(), userbuilt_btn.getObjSurf(0), &userbuilt_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);

	random_btn.setCoord(object::resX/2+capt3_one_pl.getConstSizeX()/2, medium_btn.getCurY()+medium_btn.getConstSizeY()+ medium_btn.getConstSizeY()/5); 
	if(random_btn.isInRect(mx,my) || curLevConc == RANDOM)
	lpDDSBack->Blt(&random_btn.getCRect(), random_btn.getObjSurf(1), &random_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
	else
	lpDDSBack->Blt(&random_btn.getCRect(), random_btn.getObjSurf(0), &random_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);

	back_btn.setCoord(hard_btn.getCurX()+hard_btn.getConstSizeX()/2- back_btn.getConstSizeX()/2  , (object::resY - hard_btn.getCurY()-hard_btn.getConstSizeY())/2+ hard_btn.getCurY()+hard_btn.getConstSizeY() - back_btn.getCurSizeY()/2 ); 
	if(back_btn.isInRect(mx,my))
	lpDDSBack->Blt(&back_btn.getCRect(), back_btn.getObjSurf(1), &back_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
	else
	lpDDSBack->Blt(&back_btn.getCRect(), back_btn.getObjSurf(0), &back_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);

	play_btn.setCoord(random_btn.getCurX()+random_btn.getConstSizeX()/2- play_btn.getConstSizeX()/2 , (object::resY - hard_btn.getCurY()-hard_btn.getConstSizeY())/2+ hard_btn.getCurY()+hard_btn.getConstSizeY() - back_btn.getCurSizeY()/2 ); 
	if(play_btn.isInRect(mx,my))
	lpDDSBack->Blt(&play_btn.getCRect(), play_btn.getObjSurf(1), &play_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
	else
	lpDDSBack->Blt(&play_btn.getCRect(), play_btn.getObjSurf(0), &play_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);


	if(play_btn.isClicked(buttonDown[0], mx, my))
	{
		if(strlen(player1->getName())==0 || strlen(player2->getName())==0)
		{
			emptyNameFlag=1;
			emptyName.setLastTick(timeGetTime());
		}
		else
		{
			player1->initForNewLevel(3);
			player2->initForNewLevel(3);
			player1->getPaddle()->setCoord(0, object::resY - 20 );
			player2->getPaddle()->setCoord(object::resX-player2->getPaddle()->getConstSizeX(), object::resY - 20 );
			curLevel=1;
			if(curLevConc==CAMPAIGN)
				field.readF("level1.map"); 
			else if(curLevConc==USERBUILT)
				field.readF("MyLevel.map"); 
			else if(curLevConc==RANDOM)
			{
				char buff[50];
				sprintf(buff,"Level%d.map",getRandom(1,19));
				field.readF(buff); 
			}
			ufo.endShot(); 
			m_upgrades.clear(); 
			curWindow=GAME;
			return 0;
		}
	}
	if(back_btn.isClicked(buttonDown[0], mx, my))
	{
		curWindow=MAIN;
		return 0;
	}

	if(buttonDown[0] && name_field_one_pl.isInRect(mx, my))
		cur_name_field =1;
	if(buttonDown[0] && name_field_two_pl.isInRect(mx, my))
		cur_name_field =2;

	if(easy_btn.isClicked(buttonDown[0], mx, my))
		curDifficulty = EASY; 
	if(medium_btn.isClicked(buttonDown[0], mx, my))
		curDifficulty = MEDIUM; 
	if(hard_btn.isClicked(buttonDown[0], mx, my))
		curDifficulty = HARD; 

	if(campaign_btn.isClicked(buttonDown[0], mx, my))
	{
		curLevConc = CAMPAIGN; 
		curLevel=1;
		numLevels=20;
	}
	if(userbuilt_btn.isClicked(buttonDown[0], mx, my))
	{
		curLevConc = USERBUILT; 
		curLevel=1;
		numLevels=1;
	}
	if(random_btn.isClicked(buttonDown[0], mx, my))
	{
		curLevConc = RANDOM;
		curLevel=1;
		numLevels=1;
	}

	if(emptyNameFlag)
	{
		if(!emptyName.hasTimePassed(2000))
			drawText("please enter both names", object::resX/2-getTextWidth("please enter both names")/2, object::resY/2);
		else
			emptyNameFlag=0;

	}		

	//showtext(double(-111));

	int lastCharPos1, lastCharPos2;

	lastCharPos1 = drawText(player1->getName(), name_field_one_pl.getCurX() +font.getConstSizeY() , name_field_one_pl.getCurY()+font.getConstSizeY()/3 );
	lastCharPos2 = drawText(player2->getName(), name_field_two_pl.getCurX() +font.getConstSizeY() , name_field_two_pl.getCurY()+font.getConstSizeY()/3 );

	if(cur_name_field==1)
		print_cursor.setCoord(lastCharPos1, name_field_one_pl.getCurY()+font.getConstSizeY()/3); 
	else
		print_cursor.setCoord(lastCharPos2, name_field_two_pl.getCurY()+font.getConstSizeY()/3); 

	lpDDSBack->Blt(&print_cursor.getCRect(), print_cursor.getObjSurf(0), &print_cursor.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);

	//cursor::
	updateCoords(&mx, &my);
	cur.setCoord(mx,my);
	lpDDSBack->Blt(&cur.getCRect(), cur.getObjSurf(0), &cur.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
	
	return 0;
}

int displayMainMenu()
{
	drawBackground(bg_mm);

	decor_logo.setCoord(object::resX/2-decor_logo.getConstSizeX()/2, object::resY/20); 
	lpDDSBack->Blt(&decor_logo.getCRect(), decor_logo.getObjSurf(0), &decor_logo.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);

	decor_bricks.setCoord(object::resX/2-decor_bricks.getConstSizeX()/2, object::resY/6+decor_bricks.getCurSizeY()/2 ); 
	lpDDSBack->Blt(&decor_bricks.getCRect(), decor_bricks.getObjSurf(0), &decor_bricks.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);

	one_pl_btn.setCoord(object::resX/4-one_pl_btn.getConstSizeX()/2, object::resY/3+one_pl_btn.getConstSizeY()/2); 
	if(one_pl_btn.isInRect(mx, my))
	lpDDSBack->Blt(&one_pl_btn.getCRect(), one_pl_btn.getObjSurf(1), &one_pl_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
	else
	lpDDSBack->Blt(&one_pl_btn.getCRect(), one_pl_btn.getObjSurf(0), &one_pl_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);

	two_pl_btn.setCoord(object::resX/2+two_pl_btn.getConstSizeX()/4, object::resY/3+one_pl_btn.getConstSizeY()/2); 
	if(two_pl_btn.isInRect(mx, my))
	lpDDSBack->Blt(&two_pl_btn.getCRect(), two_pl_btn.getObjSurf(1), &two_pl_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
	else
	lpDDSBack->Blt(&two_pl_btn.getCRect(), two_pl_btn.getObjSurf(0), &two_pl_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);

	opt_btn.setCoord(object::resX/4-opt_btn.getConstSizeX()/2, object::resY/2+one_pl_btn.getConstSizeY()/2); 
	if(opt_btn.isInRect(mx, my))
	lpDDSBack->Blt(&opt_btn.getCRect(), opt_btn.getObjSurf(1), &opt_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
	else
	lpDDSBack->Blt(&opt_btn.getCRect(), opt_btn.getObjSurf(0), &opt_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);

	scores_btn.setCoord(object::resX/2+scores_btn.getConstSizeX()/4, object::resY/2+one_pl_btn.getConstSizeY()/2); 
	if(scores_btn.isInRect(mx, my))
	lpDDSBack->Blt(&scores_btn.getCRect(), scores_btn.getObjSurf(1), &scores_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
	else
	lpDDSBack->Blt(&scores_btn.getCRect(), scores_btn.getObjSurf(0), &scores_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);

	quit_btn.setCoord(object::resX/2+quit_btn.getConstSizeX()/4*3, object::resY-object::resY/5); 
	if(quit_btn.isInRect(mx, my))
	lpDDSBack->Blt(&quit_btn.getCRect(), quit_btn.getObjSurf(1), &quit_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
	else
	lpDDSBack->Blt(&quit_btn.getCRect(), quit_btn.getObjSurf(0), &quit_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);

	if(decor_paddle.hasTimePassed(10)) 
		decor_paddle.setCoord(decor_paddle.getCurX()+decor_daddle_delta, decor_paddle.getCurY()); 

	if(decor_paddle.getCurX() + decor_paddle.getCurSizeX() >= quit_btn.getCurX())
	{
		decor_paddle.setCoord(quit_btn.getCurX()-decor_paddle.getCurSizeX(),decor_paddle.getCurY() );
		decor_daddle_delta=-decor_daddle_delta;
	}
	if(decor_paddle.getCurX()<=0)
	{
		decor_paddle.setCoord(0,decor_paddle.getCurY());  
		decor_daddle_delta=-decor_daddle_delta; 
	}
	lpDDSBack->Blt(&decor_paddle.getCRect(), decor_paddle.getObjSurf(0), &decor_paddle.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);


	updateVectorOf(&decor_ball, false);
	if(isCollide(&decor_ball, &one_pl_btn))
		updateBrickCollision(&decor_ball, &one_pl_btn);
	if(isCollide(&decor_ball, &two_pl_btn))
		updateBrickCollision(&decor_ball, &two_pl_btn);
	if(isCollide(&decor_ball, &opt_btn))
		updateBrickCollision(&decor_ball, &opt_btn);
	if(isCollide(&decor_ball, &scores_btn))
		updateBrickCollision(&decor_ball, &scores_btn);
	if(isCollide(&decor_ball, &decor_paddle))
	{
		updateVectorOf(&decor_ball,&decor_paddle,false);
		updateBrickCollision(&decor_ball, &decor_paddle);
	}
	if(isCollide(&decor_ball,&quit_btn))
		updateBrickCollision(&decor_ball,&quit_btn);

	if(decor_ball.hasTimePassed(100))
		decor_ball.goNextImage();

	decor_ball.setCoord(decor_ball.getCurX()+ decor_ball.getStepX(), decor_ball.getCurY()+ decor_ball.getStepY());
	lpDDSBack->Blt(&decor_ball.getCRect(), decor_ball.getObjSurf(decor_ball.getCurImage() ), &decor_ball.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);

	updateCoords(&mx, &my);
	cur.setCoord(mx,my);
	lpDDSBack->Blt(&cur.getCRect(), cur.getObjSurf(0), &cur.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);

	if(one_pl_btn.isClicked(buttonDown[0], mx, my))
	{
		curWindow=ONEPLAYER;
		isTwoPlayerGame=false;
		buttonDown[0]=false;
		quitYN=0;
	}
	
	if(two_pl_btn.isClicked(buttonDown[0], mx, my))
	{
		curWindow=TWOPLAYER;
		isTwoPlayerGame=true;
		buttonDown[0]=false;
		quitYN=0;
	}

	if(opt_btn.isClicked(buttonDown[0], mx, my))
	{
		curWindow=OPTIONS;
	}
	if(scores_btn.isClicked(buttonDown[0], mx, my))
	{
		curWindow=SCORES;
	}

	
	if(quit_btn.isClicked(buttonDown[0], mx, my))
	{
		PostQuitMessage(0);
	}

	return 1;
}
int dispalyOptions()
{
	drawBackground(bg_mm);
	header_options.setCoord(object::resX/2-header_options.getConstSizeX()/2, object::resY/20); 
	lpDDSBack->Blt(&header_options.getCRect(), header_options.getObjSurf(0), &header_options.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);

	capt1_options.setCoord(object::resX/2-capt1_options.getConstSizeX()/2, object::resY/5); 
	lpDDSBack->Blt(&capt1_options.getCRect(), capt1_options.getObjSurf(0), &capt1_options.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);

	schema_space_btn.setCoord(object::resX/6-schema_space_btn.getConstSizeX()/2, object::resY/4+one_pl_btn.getConstSizeY()/2); 
	if(schema_space_btn.isInRect(mx, my) || curSchema==SPASE_SCHEMA)
	lpDDSBack->Blt(&schema_space_btn.getCRect(), schema_space_btn.getObjSurf(1), &schema_space_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
	else
	lpDDSBack->Blt(&schema_space_btn.getCRect(), schema_space_btn.getObjSurf(0), &schema_space_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);

	schema_ocean_btn.setCoord(object::resX/2-schema_ocean_btn.getConstSizeX()/2, object::resY/4+one_pl_btn.getConstSizeY()/2); 
	if(schema_ocean_btn.isInRect(mx, my) || curSchema==WATER_SCHEMA)
	lpDDSBack->Blt(&schema_ocean_btn.getCRect(), schema_ocean_btn.getObjSurf(1), &schema_ocean_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
	else
	lpDDSBack->Blt(&schema_ocean_btn.getCRect(), schema_ocean_btn.getObjSurf(0), &schema_ocean_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);

	schema_nature_btn.setCoord(object::resX*5/6-schema_nature_btn.getConstSizeX()/2, object::resY/4+one_pl_btn.getConstSizeY()/2); 
	if(schema_nature_btn.isInRect(mx, my) || curSchema==NATURE_SCHEMA)
	lpDDSBack->Blt(&schema_nature_btn.getCRect(), schema_nature_btn.getObjSurf(1), &schema_nature_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
	else
	lpDDSBack->Blt(&schema_nature_btn.getCRect(), schema_nature_btn.getObjSurf(0), &schema_nature_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);

	decor_bricks.setCoord(object::resX/2-decor_bricks.getConstSizeX()/2, object::resY/2); 
	lpDDSBack->Blt(&decor_bricks.getCRect(), decor_bricks.getObjSurf(0), &decor_bricks.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);

	controls_btn.setCoord(object::resX/2-controls_btn.getConstSizeX()/2, object::resY*2/3); 
	if(controls_btn.isInRect(mx, my))
	lpDDSBack->Blt(&controls_btn.getCRect(), controls_btn.getObjSurf(1), &controls_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
	else
	lpDDSBack->Blt(&controls_btn.getCRect(), controls_btn.getObjSurf(0), &controls_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);

	options_back_btn.setCoord(object::resX/2-options_back_btn.getConstSizeX()/2, object::resY*5/6); 
	if(options_back_btn.isInRect(mx, my))
	lpDDSBack->Blt(&options_back_btn.getCRect(), options_back_btn.getObjSurf(1), &options_back_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
	else
	lpDDSBack->Blt(&options_back_btn.getCRect(), options_back_btn.getObjSurf(0), &options_back_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);

 	updateCoords(&mx, &my);
	cur.setCoord(mx,my);
	lpDDSBack->Blt(&cur.getCRect(), cur.getObjSurf(0), &cur.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);

	if(options_back_btn.isClicked(buttonDown[0], mx, my))
	{
		curWindow=MAIN;
	}
	
	if(schema_space_btn.isClicked(buttonDown[0], mx, my))
	{
		curSchema=SPASE_SCHEMA;
	}
	if(schema_nature_btn.isClicked(buttonDown[0], mx, my))
	{
		curSchema=NATURE_SCHEMA;
	}
	if(schema_ocean_btn.isClicked(buttonDown[0], mx, my))
	{
		curSchema=WATER_SCHEMA;
	}
	if(controls_btn.isClicked(buttonDown[0], mx, my))
	{
		curWindow=CONTROLS;
	}
	//showtext(double(outc));
	return 1;
}
int displayControls()
{

	drawBackground(bg_mm);

	capt1_controls.setCoord(object::resX/2-capt1_controls.getConstSizeX()/2, object::resY/20); 
	lpDDSBack->Blt(&capt1_controls.getCRect(), capt1_controls.getObjSurf(0), &capt1_controls.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);


	LEFT_change_btn.setCoord(object::resX*4/5-LEFT_change_btn.getConstSizeX()/2, object::resY/4); 
	if(LEFT_change_btn.isInRect(mx, my))
	lpDDSBack->Blt(&LEFT_change_btn.getCRect(), LEFT_change_btn.getObjSurf(1), &LEFT_change_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
	else
	lpDDSBack->Blt(&LEFT_change_btn.getCRect(), LEFT_change_btn.getObjSurf(0), &LEFT_change_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);

	RIGHT_change_btn.setCoord(object::resX*4/5-RIGHT_change_btn.getConstSizeX()/2, object::resY/4 +RIGHT_change_btn.getConstSizeY()*2); 
	if(RIGHT_change_btn.isInRect(mx, my))
	lpDDSBack->Blt(&RIGHT_change_btn.getCRect(), RIGHT_change_btn.getObjSurf(1), &RIGHT_change_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
	else
	lpDDSBack->Blt(&RIGHT_change_btn.getCRect(), RIGHT_change_btn.getObjSurf(0), &RIGHT_change_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);

	FIRE_change_btn.setCoord(object::resX*4/5-FIRE_change_btn.getConstSizeX()/2, object::resY/4 +RIGHT_change_btn.getConstSizeY()*4); 
	if(FIRE_change_btn.isInRect(mx, my))
	lpDDSBack->Blt(&FIRE_change_btn.getCRect(), FIRE_change_btn.getObjSurf(1), &FIRE_change_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
	else
	lpDDSBack->Blt(&FIRE_change_btn.getCRect(), FIRE_change_btn.getObjSurf(0), &FIRE_change_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);

	controls_1player_btn.setCoord(object::resX/8-controls_1player_btn.getConstSizeX()/2, object::resY/4 +RIGHT_change_btn.getConstSizeY()*6); 
	if(controls_1player_btn.isInRect(mx, my) || cur_change_player==1)
	lpDDSBack->Blt(&controls_1player_btn.getCRect(), controls_1player_btn.getObjSurf(1), &controls_1player_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
	else
	lpDDSBack->Blt(&controls_1player_btn.getCRect(), controls_1player_btn.getObjSurf(0), &controls_1player_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);

	controls_2player_btn.setCoord(object::resX/8-controls_2player_btn.getConstSizeX()/2, object::resY/4 +RIGHT_change_btn.getConstSizeY()*7); 
	if(controls_2player_btn.isInRect(mx, my) || cur_change_player==2)
	lpDDSBack->Blt(&controls_2player_btn.getCRect(), controls_2player_btn.getObjSurf(1), &controls_2player_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
	else
	lpDDSBack->Blt(&controls_2player_btn.getCRect(), controls_2player_btn.getObjSurf(0), &controls_2player_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);

	controls_back_btn.setCoord(object::resX*4/5-FIRE_change_btn.getConstSizeX()/2, object::resY/4 +RIGHT_change_btn.getConstSizeY()*7); 
	if(controls_back_btn.isInRect(mx, my))
	lpDDSBack->Blt(&controls_back_btn.getCRect(), controls_back_btn.getObjSurf(1), &controls_back_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
	else
	lpDDSBack->Blt(&controls_back_btn.getCRect(), controls_back_btn.getObjSurf(0), &controls_back_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);

	control_usemouse_btn.setCoord(object::resX/2-control_usemouse_btn.getConstSizeX()/2, object::resY/4 +RIGHT_change_btn.getConstSizeY()*6); 
	if(control_usemouse_btn.isInRect(mx, my) || (cur_change_player==1&&player1->isUsingMouse()) || (cur_change_player==2&&player2->isUsingMouse()))
	lpDDSBack->Blt(&control_usemouse_btn.getCRect(), control_usemouse_btn.getObjSurf(1), &control_usemouse_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
	else
	lpDDSBack->Blt(&control_usemouse_btn.getCRect(), control_usemouse_btn.getObjSurf(0), &control_usemouse_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);



	char buff[50];
	strcpy(buff,"move left");
	drawText(buff,object::resX/8-controls_1player_btn.getConstSizeX()/2, object::resY/4);
	strcpy(buff,"move right");
	drawText(buff,object::resX/8-controls_1player_btn.getConstSizeX()/2, object::resY/4 +RIGHT_change_btn.getConstSizeY()*2);
	strcpy(buff,"fire");
	drawText(buff,object::resX/8-controls_1player_btn.getConstSizeX()/2, object::resY/4 +RIGHT_change_btn.getConstSizeY()*4);

	
	switch(cur_change_player)
	{
	case 1:
		if(cur_change_contr==1)
			strcpy(buff,"press any key");
		else
			getWordFromChar(player1->LEFT_BTN, buff);
		drawText(buff,object::resX/3, object::resY/4);

		if(cur_change_contr==2)
			strcpy(buff,"press any key");
		else
			getWordFromChar(player1->RIGHT_BTN, buff); 
		drawText(buff,object::resX/3, object::resY/4 +RIGHT_change_btn.getConstSizeY()*2);

		if(cur_change_contr==3)
			strcpy(buff,"press any key");
		else
			getWordFromChar(player1->FIRE_BTN, buff); 
		drawText(buff,object::resX/3, object::resY/4 +RIGHT_change_btn.getConstSizeY()*4);
		
		controls_devider.setConstSizeY(object::resY/4-object::resY/4 +RIGHT_change_btn.getConstSizeY()*6+controls_1player_btn.getConstSizeY()/2 ); 
		controls_devider.setCoord(object::resX/9-controls_1player_btn.getConstSizeX()/2, object::resY/4); 
		lpDDSBack->Blt(&controls_devider.getCRect(), controls_devider.getObjSurf(0), &controls_devider.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);


		break;
	case 2:
		if(cur_change_contr==4)
			strcpy(buff,"press any key");
		else
			getWordFromChar(player2->LEFT_BTN, buff);
		drawText(buff,object::resX/3, object::resY/4);

		if(cur_change_contr==5)
			strcpy(buff,"press any key");
		else
			getWordFromChar(player2->RIGHT_BTN, buff); 
		drawText(buff,object::resX/3, object::resY/4 +RIGHT_change_btn.getConstSizeY()*2);

		if(cur_change_contr==6)
			strcpy(buff,"press any key");
		else
			getWordFromChar(player2->FIRE_BTN, buff); 
		drawText(buff,object::resX/3, object::resY/4 +RIGHT_change_btn.getConstSizeY()*4);
		
		controls_devider.setConstSizeY(object::resY/4-object::resY/4 +RIGHT_change_btn.getConstSizeY()*7+controls_1player_btn.getConstSizeY()/2); 
		controls_devider.setCoord(object::resX/9-controls_1player_btn.getConstSizeX()/2, object::resY/4); 
		lpDDSBack->Blt(&controls_devider.getCRect(), controls_devider.getObjSurf(0), &controls_devider.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);


		break;
	}
	
	if(control_usemouse_btn.isClicked(buttonDown[0], mx, my))
	{
		if(cur_change_player==1)
		{
			if(player1->isUsingMouse())
				player1->setUsingMouse(0);
			else
			{
				player1->setUsingMouse(1);
				player2->setUsingMouse(0);
			}
		}
		else
		{
			if(player2->isUsingMouse())
				player2->setUsingMouse(0);
			else
			{
				player2->setUsingMouse(1);
				player1->setUsingMouse(0);
			}

		}
	}

	if(controls_back_btn.isClicked(buttonDown[0], mx, my))
	{
		cur_change_contr=0;
		curWindow=OPTIONS;
	}

	if(controls_1player_btn.isClicked(buttonDown[0], mx, my))
	{
		cur_change_contr=0;
		cur_change_player=1;
	}
	if(controls_2player_btn.isClicked(buttonDown[0], mx, my))
	{
		cur_change_contr=0;
		cur_change_player=2;
	}

	if(cur_change_player==1)
	{
		if(LEFT_change_btn.isClicked(buttonDown[0], mx, my))
			cur_change_contr=1; 
		else if(RIGHT_change_btn.isClicked(buttonDown[0], mx, my))
			cur_change_contr=2;
		else if(FIRE_change_btn.isClicked(buttonDown[0], mx, my))
			cur_change_contr=3;
	}
	else if(cur_change_player==2)
	{
		if(LEFT_change_btn.isClicked(buttonDown[0], mx, my))
			cur_change_contr=4; 
		else if(RIGHT_change_btn.isClicked(buttonDown[0], mx, my))
			cur_change_contr=5;
		else if(FIRE_change_btn.isClicked(buttonDown[0], mx, my))
			cur_change_contr=6;
	}



	char b;
	
	b = char(player1->LEFT_BTN);
	//showtext(double(player1->LEFT_BTN));


 //  controls_1player_btn,controls_2player_btn,
//		
//int cur_change_player=1;

	updateCoords(&mx, &my);
	cur.setCoord(mx,my);
	lpDDSBack->Blt(&cur.getCRect(), cur.getObjSurf(0), &cur.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);

	return 1;
}

int displayScores()
{
	drawBackground(bg_mm);

	scores_header.setCoord(object::resX/2-scores_header.getConstSizeX()/2, object::resY/20); 
	lpDDSBack->Blt(&scores_header.getCRect(), scores_header.getObjSurf(0), &scores_header.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);


	scores_table.setCoord(object::resX/2-scores_table.getConstSizeX()/2, object::resY/20+scores_header.getConstSizeY()); 
	lpDDSBack->Blt(&scores_table.getCRect(), scores_table.getObjSurf(0), &scores_table.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);

	scores_clear_btn.setCoord(object::resX*1/5-scores_back_btn.getConstSizeX()/2, scores_table.getCurY()+scores_table.getConstSizeY()+object::resY/20 ); 
	if(scores_clear_btn.isInRect(mx, my))
	lpDDSBack->Blt(&scores_clear_btn.getCRect(), scores_clear_btn.getObjSurf(1), &scores_clear_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
	else
	lpDDSBack->Blt(&scores_clear_btn.getCRect(), scores_clear_btn.getObjSurf(0), &scores_clear_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);


	scores_back_btn.setCoord(object::resX*4/5-scores_back_btn.getConstSizeX()/2, scores_table.getCurY()+scores_table.getConstSizeY()+object::resY/20 ); 
	if(scores_back_btn.isInRect(mx, my))
	lpDDSBack->Blt(&scores_back_btn.getCRect(), scores_back_btn.getObjSurf(1), &scores_back_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
	else
	lpDDSBack->Blt(&scores_back_btn.getCRect(), scores_back_btn.getObjSurf(0), &scores_back_btn.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);


	for(int i=0; i<5;i++)
	{
		char buff[100];
		sprintf(buff,"%d",scores[i].score);
		drawText(scores[i].name, scores_table.getCurX()+scores_table.getConstSizeX()/40, scores_table.getCurY()+scores_table.getConstSizeY()/8+(scores_table.getConstSizeY()/8)*i);  
		drawText(buff, scores_table.getCurX()+scores_table.getConstSizeX()- getTextWidth(buff)-scores_table.getConstSizeX()/40, scores_table.getCurY()+scores_table.getConstSizeY()/8+(scores_table.getConstSizeY()/8)*i);  
	}
	if(scores_back_btn.isClicked(buttonDown[0], mx, my))
	{
		curWindow=MAIN;
	}

	if(scores_clear_btn.isClicked(buttonDown[0], mx, my))
	{
		FILE *f;
		if((f=fopen("scores.txt","w"))==NULL)
			return 0;
		for(int r=0; r<5; r++)
		{
			strcpy(scores[r].name,"empty"); 
			scores[r].score=0;
		}
		fclose(f);
	}
	updateCoords(&mx, &my);
	cur.setCoord(mx,my);
	lpDDSBack->Blt(&cur.getCRect(), cur.getObjSurf(0), &cur.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);

	return 1;
}
int drawNatureBg()
{
	//char b[10];
	RECT r;
	r.top =0; r.left =0; r.right=object::resX; r.bottom =object::resY*3/4;
	lpDDSBack->Blt(&r, bg_blue.getObjSurf(0) , NULL,  DDBLT_WAIT, NULL);
	r.top =object::resY/2; r.left =0; r.right=object::resX; r.bottom =object::resY;
	lpDDSBack->Blt(&r, bg_green.getObjSurf(0) , NULL,  DDBLT_WAIT|DDBLT_KEYSRC, NULL);


	for(int i=0; 1; i++)
	{
		if(0-tree_delta<=0-cloud.getConstSizeX()-200)
				tree_delta=0;
		cloud.setCoord(0-tree_delta+(cloud.getConstSizeX()+200)*i,object::resY/4);

		lpDDSBack->Blt(&cloud.getCRect(), cloud.getObjSurf(curJup), &cloud.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
		if(cloud.getCurX()+cloud.getConstSizeX()>=object::resX)
			break;
	}
	tree_delta++;
	
	
	return 1;
}
int drawSpaceBg()
{
	RECT r;
//	jupBG1.setCoord(0-mvJup,0);
//	lpDDSBack->Blt(&jupBG1.getCRect(), jupBG1.getObjSurf(curJup), &jupBG1.getRect(),  DDBLT_WAIT, NULL);
	for(int i=0; 1; i++)
	{
		if(0-mvJup<=0-jupBG1.getConstSizeX())
			{mvJup=0;curJup==0?curJup++:curJup--;}
		jupBG1.setCoord(0-mvJup+jupBG1.getConstSizeX()*i,0);
		if(i%2!=0)
			lpDDSBack->Blt(&jupBG1.getCRect(), jupBG1.getObjSurf(curJup==0?1:0), &jupBG1.getRect(),  DDBLT_WAIT, NULL);
		else
			lpDDSBack->Blt(&jupBG1.getCRect(), jupBG1.getObjSurf(curJup), &jupBG1.getRect(),  DDBLT_WAIT, NULL);
		if(jupBG1.getCurX()+jupBG1.getConstSizeX()>=object::resX)
			break;
	}
	
	mvJup++;
	
	r.left = object::resX-640;r.top = object::resY-493;r.right = object::resX;r.bottom = object::resY;
	lpDDSBack->Blt(&r, europa.getObjSurf(0) , NULL,  DDBLT_WAIT|DDBLT_KEYSRC, NULL);

	return 1;
}
int drawOceanBg()
{
	for(int i=0; 1 ; i++)
	{
		for(int j=0; 1; j++)
		{
			if(0-water_delta<=0-water.getConstSizeY())
				water_delta=0;
			water.setCoord(water.getConstSizeX()*j ,0-water_delta+(water.getConstSizeY())*i);

			lpDDSBack->Blt(&water.getCRect(), water.getObjSurf(curJup), &water.getRect(),  DDBLT_WAIT|DDBLT_KEYSRC, NULL);
			if(water.getCurX()+water.getConstSizeX()>=object::resX)
				break;
		}

		if(water.getCurY()+water.getConstSizeY()>=object::resY)
			break;

	}

water_delta+=0.5;
return 1;
}
int drawBackground(object bg)
{
	RECT r;
	r.left = 0;r.top = 0;r.right = long(object.resX) ;r.bottom = long(object.resY);
	lpDDSBack->Blt(&r, bg.getObjSurf(0) , NULL,  DDBLT_WAIT, NULL);
	return 0;
}

int showtext(double num)
{
	HDC                 hdc;
	char                str[255];

	for(int i=0; i<3; i++)

		if ( FAILED( lpDDSBack->GetDC( &hdc ) ) )
		{
			return 0;
		}
		else
		{
     	    SetBkColor( hdc, RGB( 0, 0, 0 ) );
			SetTextColor( hdc, RGB( 255, 0, 0 ) );
			SetTextAlign( hdc, TA_CENTER );
			if(num==-111)
			{
				sprintf( str, "FPS: %d;  %d * %d ", dwFrames, curDifficulty,curLevConc);
				TextOut( hdc, long(object::resX/2) , 0, str, lstrlen( str ) );
			}
			else
				sprintf( str, "FPS: %d; Balls: %f; Buttons: Q,A; Z,X; O,L; P;", 
					dwFrames, num);
				TextOut( hdc, long(object::resX/2) , 0, str, lstrlen( str ) );

				
     		// Don't ever forget this.
			lpDDSBack->ReleaseDC( hdc );
		}
		return 1;
}
int showtext(char in)
{
	HDC                 hdc;
	char                str[255];


	if ( FAILED( lpDDSBack->GetDC( &hdc ) ) )
	{
		return 0;
	}
	else
	{
   	    SetBkColor( hdc, RGB( 0, 0, 0 ) );
		SetTextColor( hdc, RGB( 255, 0, 0 ) );
		SetTextAlign( hdc, TA_CENTER );
	
		sprintf( str, "Str: %c", in);
		TextOut( hdc, long(object::resX/2) , 0, str, lstrlen( str ) );
		// Don't ever forget this.
		lpDDSBack->ReleaseDC( hdc );
	}
		return 1;
}

int drawText(char * text, double _x, double _y)
{
	int i=0;
	int topMargin=0, leftMargin=0;
	int x=int(_x); 
	int y=int(_y);

	while(text[i]!='\0')
	{
		int curInt=int(text[i]);
		if(curInt>=48 && curInt<=57) //nums
		{
			topMargin=1;
			leftMargin= 1+19*(curInt-48)+(curInt-48);

			RECT r;
			r.left = leftMargin;r.top = topMargin;r.right = leftMargin+19; r.bottom = topMargin+30;
			
			RECT r2;
			r2.left = x+19*i; r2.top = y; r2.right = x+19+19*i; r2.bottom = y+30;
			
		    lpDDSBack->Blt(&r2, font.getObjSurf(font.getCurImage()), &r,  DDBLT_WAIT|DDBLT_KEYSRC, NULL);


		}
		else if(curInt>=97 && curInt<=122) //chars
		{
			topMargin=32;
			leftMargin= 1+19*(curInt-97)+(curInt-97);

			RECT r;
			r.left = leftMargin;r.top = topMargin;r.right = leftMargin+19; r.bottom = topMargin+30;
			
			RECT r2;
			r2.left = x+19*i; r2.top = y; r2.right = x+19+19*i; r2.bottom = y+30;
			
		    lpDDSBack->Blt(&r2, font.getObjSurf(font.getCurImage()), &r,  DDBLT_WAIT|DDBLT_KEYSRC, NULL);

		}
		i++;
	}
		return x+19+19*(i-1);
}
int getTextWidth(char * text)
{
	int i=0;
	while(text[i]!='\0') i++;
	return 19+19*(i-1);
}

int checkKeyboard()
{	
	char kbuf[256];
	if(pKeyboard->GetDeviceState(sizeof(kbuf),(LPVOID)&kbuf)!=DI_OK)
	{
		ErrStr="Error in pKeyboard->GetDeviceState()";
		return 0;
	}
	if(KEYDOWN(kbuf, DIK_UP))
	{};
	if(KEYDOWN(kbuf, DIK_DOWN))
	{};
	//player1 LEFT
	if(KEYDOWN(kbuf, charToDinput(player1->LEFT_BTN)))
	{
		if(!player1->isFrozen())
		player1->getPaddle()->setCoord(
				player1->getPaddle()->getCurX()-10, 
				player1->getPaddle()->getCurY()
				);  
		updatePositionOf(player1->getPaddle());
	}
	//player1 RIGHT
	if(KEYDOWN(kbuf, charToDinput(player1->RIGHT_BTN)))
	{
		if(!player1->isFrozen())
		player1->getPaddle()->setCoord(
				player1->getPaddle()->getCurX()+10, 
				player1->getPaddle()->getCurY()
				);  
		updatePositionOf(player1->getPaddle());
	}
	//player2 LEFT
	if(KEYDOWN(kbuf, charToDinput(player2->LEFT_BTN)))
	{
		if(!player2->isFrozen())
		player2->getPaddle()->setCoord(
				player2->getPaddle()->getCurX()-10, 
				player2->getPaddle()->getCurY()
				);  
		updatePositionOf(player2->getPaddle());
	}
	//player2 RIGHT
	if(KEYDOWN(kbuf, charToDinput(player2->RIGHT_BTN)))
	{
		if(!player2->isFrozen())
		player2->getPaddle()->setCoord(
				player2->getPaddle()->getCurX()+10, 
				player2->getPaddle()->getCurY()
				);  
		updatePositionOf(player2->getPaddle());
	}

	//if(KEYDOWN(kbuf, DIK_Q))
	//{
	//	player1->getPaddle()->setConstSizeX(player1->getPaddle()->getConstSizeX()+5); 
	//	player2->getPaddle()->setConstSizeX(player2->getPaddle()->getConstSizeX()+5); 
	//}
	//if(KEYDOWN(kbuf, DIK_A))
	//{
	//	player1->getPaddle()->setConstSizeX(player1->getPaddle()->getConstSizeX()-5); 
	//	player2->getPaddle()->setConstSizeX(player2->getPaddle()->getConstSizeX()-5); 
	//}
	
	return 1;
}

int checkMouse()
{
	if (NULL != pMouse) 
	{
		DIMOUSESTATE dims;      // DirectInput mouse state structure
		HRESULT hr;
		hr = DIERR_INPUTLOST;
			// if input is lost then acquire and keep trying 
		while ( DIERR_INPUTLOST == hr ) 
		{
			// get the input's device state, and put the state in dims
			hr = pMouse->GetDeviceState( sizeof(DIMOUSESTATE), &dims );
			if ( hr == DIERR_INPUTLOST )
			{
				hr = pMouse->Acquire();
				if ( FAILED(hr) )  
				{
					ErrStr="Error in pMouse->GetDeviceState() or pMouse->Acquire()";
					return FALSE;
				}
			}
		}
		
		if ( FAILED(hr) )  
		{
			ErrStr="Error in pMouse->GetDeviceState() or pMouse->Acquire()  (2!)";	
			return FALSE;
		}
		
		if (dims.rgbButtons[0] & 0x80) buttonDown[0]=true;
		else buttonDown[0]=false;
		if (dims.rgbButtons[1] & 0x80) buttonDown[1]=true;
		else buttonDown[1]=false;
		if (dims.rgbButtons[2] & 0x80) buttonDown[2]=true;
		else buttonDown[2]=false;
		
		if( (curWindow == GAME && player2->isUsingMouse() && !player2->isGameOver() && !player2->isFrozen() && player2->getPaddle()->getCurX()>=0 &&  player2->getPaddle()->getCurX() + player2->getPaddle()->getConstSizeX() <= object::resX) ||
			(curWindow == GAME && player1->isUsingMouse() && !player1->isGameOver() && !player1->isFrozen() && player1->getPaddle()->getCurX()>=0 &&  player1->getPaddle()->getCurX() + player1->getPaddle()->getConstSizeX() <= object::resX) ||
			curWindow != GAME)
		{
			mx+=dims.lX; 
			my+=dims.lY;
		}
		
	}
	return 1;
}




BOOL CALLBACK
SampleDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   
	HWND hWndCombo;
	int idxCombo=0;
	hWndCombo = GetDlgItem(hDlg, IDC_COMBO);
    switch (uMsg)
    {
        case WM_INITDIALOG:
			//MessageBox(hWnd,"FFFF","FGGGG",1);
			
			ComboBox_AddString(hWndCombo, "800x600x16");
			ComboBox_AddString(hWndCombo, "1024x768x16");
			ComboBox_AddString(hWndCombo, "800x600x8");
			ComboBox_AddString(hWndCombo, "1024x768x8");
			ComboBox_AddString(hWndCombo, "800x600x24");
			ComboBox_AddString(hWndCombo, "1024x768x24");
			ComboBox_AddString(hWndCombo, "800x600x32");
			ComboBox_AddString(hWndCombo, "1024x768x32 (optional)");
			ComboBox_SetCurSel(hWndCombo, 7);
            return TRUE;
            
        case WM_COMMAND:
		switch (LOWORD(wParam))
        {
           	    case IDOK:
					idxCombo = ComboBox_GetCurSel(hWndCombo);
					if (idxCombo >= 0)
					{
						switch (idxCombo)
						{
						case 0:
								rX=800; rY=600; rD=16;
							break;
						case 1:
								rX=1024; rY=768;rD=16;
							break;
						case 2:
								rX=800; rY=600;rD=8;
							break;
						case 3:
								rX=1024; rY=768;rD=8;
							break;
						case 4:
								rX=800; rY=600;rD=24;
							break;
						case 5:
								rX=1024; rY=768;rD=24;
							break;
						case 6:
								rX=800; rY=600;rD=32;
							break;
						case 7:
								rX=1024; rY=768;rD=32;
							break;
						}


					}
					EndDialog(hDlg, 1);
					return TRUE;
					
                case IDCANCEL:
				                       
                    EndDialog(hDlg, 0);
                    return TRUE;
				
		}
            break;
        case WM_CLOSE:
            hDlg = (HWND )NULL;
             EndDialog(hDlg, 1);
            break;
    }
	 
    return FALSE;
}
//------ Windows Message Handler ------//

LRESULT CALLBACK 
WindowProc(HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam)
{

	int i=0;
    switch (uMsg)
    {

        case WM_DESTROY:
			Cleanup();
            PostQuitMessage(0);
            break;

        case WM_KEYDOWN: 
			//dwFrames = (DWORD)wParam;
			if (wParam==VK_ESCAPE) 
			{
				if(curWindow==GAME)
				{
					if(quitYN==0)
					{
						
						quitYN=1;
					}
					else
					{
						
						quitYN=0;
					}
				}
				else if(curWindow==MAIN)
					DestroyWindow(hWnd);
				else if(curWindow==OPTIONS)
					curWindow=MAIN;
				else if(curWindow==CONTROLS)
					curWindow=OPTIONS;
				else if(curWindow==SCORES)
					curWindow=MAIN;
				
			}
			outc=int(wParam);
			//if(wParam=='W')
			//{
			//	if(curWindow<6001)
			//		curWindow+=1000;
			//	else curWindow=2001;
			//}

			if(curWindow==GAME)
			{
				if(quitYN || gameOver)
					if(wParam=='y' || wParam=='Y')
					{
						gameOver=0;
						recordScore();
						player1->setScore(0);
						player2->setScore(0); 
						player1->setGameOver(0); 
						player2->setGameOver(0); 

						curWindow=MAIN;

					}
				
				if(wParam+32 == player1->FIRE_BTN)
				{
					if(player1->getBallsLeft()>0)
					{
						//player1->setHmBalls(player1->getHmBalls()+1); 
						player1->addBall(); 
						player1->getBall(player1->getHmBalls()-1)->setUnitXY(cosDeg(90),-sinDeg(90));
						player1->getBall(player1->getHmBalls()-1)->setCoord(player1->getPaddle()->getCurX()+player1->getPaddle()->getConstSizeX()/2,player1->getPaddle()->getCurY()-player1->getBall(0)->getConstSizeY()-1);
						player1->getBall(player1->getHmBalls()-1)->setDelay(0.01);
						player1->getBall(player1->getHmBalls()-1)->setSpeed(4);
						if(player1->getBallsLeft()-1>=0)
							player1->setBallsLeft(player1->getBallsLeft()-1);
					}
				}
				if(wParam+32 == player2->FIRE_BTN)
				{
					if(player2->getBallsLeft()>0)
					{
						//player2->setHmBalls(player2->getHmBalls()+1); 
						player2->addBall();
						player2->getBall(player2->getHmBalls()-1)->setUnitXY(cosDeg(90),-sinDeg(90));
						player2->getBall(player2->getHmBalls()-1)->setCoord(player2->getPaddle()->getCurX()+player2->getPaddle()->getConstSizeX()/2,player2->getPaddle()->getCurY()-player2->getBall(0)->getConstSizeY()-1); 
						player2->getBall(player2->getHmBalls()-1)->setDelay(0.01);
						player2->getBall(player2->getHmBalls()-1)->setSpeed(4);
						if(player2->getBallsLeft()-1>=0)
							player2->setBallsLeft(player2->getBallsLeft()-1);
					}
						
				}
				int a;int h;
				switch (wParam) 
				{ 
                 	upgrade *tempU; 
				    //case 'C':
					//	if(curSchema<=2002)
					//		curSchema+=1000;
					//	else curSchema=1002;
					//	loadAllgraphics(curSchema);

					//	break;
					case VK_F12:
						for(a=0; a<field.getHmY();a++ )
							for(h=0;h<field.getHmX();h++ )
								if(field.getTypeAt(a,h)!=UNDESTRUCTABLE)
									field.setTypeAt(0,a,h);
						break;

					case VK_F11:
						if(!ufo.isShooting())
						demoShot=1;
						break;
					case VK_F9:
						for(a=0; a<field.getHmY();a++ )
							for(h=0;h<field.getHmX();h++ )
								if(field.getTypeAt(a,h)!=UNDESTRUCTABLE)
									field.setTypeAt(0,a,h);

						if(curLevConc = CAMPAIGN)
							curLevel=numLevels-1;
						break;
					case VK_F8:
							tempU= new upgrade(ballSpeedUp); 
							tempU->setCoord(object::resX/2 , 0); 
							m_upgrades.push_back(tempU);
							break;
					case VK_F7:
							tempU= new upgrade(ballSpeedDown); 
							tempU->setCoord(object::resX/2 , 0); 
							m_upgrades.push_back(tempU);
							break;
					case VK_F6:
							tempU= new upgrade(paddleSizeUp); 
							tempU->setCoord(object::resX/2 , 0); 
							m_upgrades.push_back(tempU);
							break;
					case VK_F5:
							tempU= new upgrade(paddleSizeDown); 
							tempU->setCoord(object::resX/2 , 0); 
							m_upgrades.push_back(tempU);
							break;
					case VK_F4:
							tempU= new upgrade(ballIsPowerball); 
							tempU->setCoord(object::resX/2 , 0);  
							m_upgrades.push_back(tempU);
							break;
					case VK_F3:
							tempU= new upgrade(ballMultiBall); 
							tempU->setCoord(object::resX/2 ,0 );  
							m_upgrades.push_back(tempU);
							break;


					case 'P':
						pause=!pause;
						m_dwLastTick=timeGetTime();
						break;
					//case 'Z':	
					//	for(i =0;i<player1->getHmBalls() ;i++)
					//		player1->getBall(i)->setSpeed(player1->getBall(i)->getSpeed()+1);
					//	for(i =0;i<player2->getHmBalls() ;i++)
					//		player2->getBall(i)->setSpeed(player2->getBall(i)->getSpeed()+1);

					//	break;
					//case 'X':	
					//	for(i =0;i<player1->getHmBalls() ;i++)
					//		player1->getBall(i)->setSpeed(player1->getBall(i)->getSpeed()-1);
					//	for(i =0;i<player2->getHmBalls() ;i++)
					//		player2->getBall(i)->setSpeed(player2->getBall(i)->getSpeed()-1);

					//	break;
					case 'L':
						if(player1->getHmBalls() >0)
							player1->removeBall(player1->getHmBalls()-1); 
							//player1->setHmBalls(player1->getHmBalls()-1); 
						if(player2->getHmBalls() >0)
							player2->removeBall(player2->getHmBalls()-1); 
							//player2->setHmBalls(player2->getHmBalls()-1); 
						break;

					 // Process other non-character keystrokes. 
					 default: 
						break; 
				}
			}

			if(curWindow==ONEPLAYER || curWindow==TWOPLAYER)
			{
				if((wParam>='A' && wParam<='Z') || wParam==32 || (wParam>='0' && wParam<= '9'))
				{			


					int curInd;
					if(cur_name_field==1)
						curInd=strlen(player1->getName());
					else
						curInd=strlen(player2->getName());

					if(curInd>=20) return 0L;
					if(wParam==32 || (wParam>='0' && wParam<= '9'))
					{
						if(cur_name_field==1)
							player1->getName()[curInd] = (char)wParam;
						else
							player2->getName()[curInd] = (char)wParam;
					}
					else
					{
						if(cur_name_field==1)
							player1->getName()[curInd] = (char)wParam+32;
						else
							player2->getName()[curInd] = (char)wParam+32;
					}
					if(cur_name_field==1)
						player1->getName()[curInd+1]='\0';
					else
						player2->getName()[curInd+1]='\0';
				
				}
				if(wParam==8) //backspace
				{
					if(cur_name_field==1)
						player1->getName()[strlen(player1->getName())-1] ='\0';
					else
						player2->getName()[strlen(player2->getName())-1] ='\0';
				}
			}
			if(curWindow==CONTROLS)
			{
				if(cur_change_contr!=0)
				{
					switch(cur_change_contr)
					{
					case 1: player1->LEFT_BTN  = wParam+32;cur_change_contr=0; break;
					case 2: player1->RIGHT_BTN = wParam+32;cur_change_contr=0; break;
					case 3: player1->FIRE_BTN  = wParam+32;cur_change_contr=0; break;
					case 4: player2->LEFT_BTN  = wParam+32;cur_change_contr=0; break;
					case 5: player2->RIGHT_BTN = wParam+32;cur_change_contr=0; break;
					case 6: player2->FIRE_BTN  = wParam+32;cur_change_contr=0; break;

					}
				}
			}
			

        default:
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
	return 0L;
}

//------ Function to Initialize DirectDraw and the Application ------//

static BOOL Init(HINSTANCE hInstance, int nCmdShow)
{
    WNDCLASS                    wc;
    HRESULT                     ddrval;
    LPDIRECTDRAW                pDD;

    // Set up and register window class

    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = (WNDPROC) WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof(DWORD);
    wc.hInstance = hInstance;
    wc.hIcon = NULL;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = szClass;
    if (!RegisterClass(&wc)) {
		ErrStr=Err_Reg_Class;
        return FALSE;
	}

    // Get dimensions of display

    int ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
    int ScreenHeight = GetSystemMetrics(SM_CYSCREEN);




    // Create a window and display

	HWND hWnd;

    hWnd = CreateWindow(szClass,							// class
                        szCaption,							// caption
						WS_VISIBLE|WS_POPUP,				// style 
						0,									// left
						0,									// top
						ScreenWidth,						// width
						ScreenHeight,						// height
                        NULL,								// parent window
                        NULL,								// menu 
                        hInstance,							// instance
                        NULL);								// parms
    if (!hWnd) {
		ErrStr=Err_Create_Win;
        return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // Create the main DirectDraw object

    ddrval = DirectDrawCreate(NULL, &pDD, NULL);
    if (ddrval != DD_OK) {
		ErrStr=Err_DirectDrawCreate;
		return FALSE;
	}

    // Fetch DirectDraw4 interface

    ddrval = pDD->QueryInterface(IID_IDirectDraw4, (LPVOID *) & lpDD);
    if (ddrval != DD_OK) {
        ErrStr=Err_Query;
		return FALSE;
	}

    // Set our cooperative level

	

    ddrval = lpDD->SetCooperativeLevel(hWnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN );
    if (ddrval != DD_OK) {
		ErrStr=Err_Coop;
		return FALSE;
	}

	// Set the display mode



	ddrval = lpDD->SetDisplayMode( rX, rY, rD, 0, 0);
	if (ddrval !=DD_OK) {
		ErrStr=Err_DispMode;
		return FALSE;
	}

    // Create the primary surface with 1 back buffer

    DDSURFACEDESC2 ddsd;
	DDSCAPS2 ddscaps;
	ZeroMemory(&ddsd,sizeof(ddsd));
    ddsd.dwSize = sizeof( ddsd );
    ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE |
                          DDSCAPS_FLIP | 
                          DDSCAPS_COMPLEX;
    ddsd.dwBackBufferCount = 1;
    ddrval = lpDD->CreateSurface( &ddsd, &lpDDSPrimary, NULL );
	if (ddrval!=DD_OK) {
		ErrStr=Err_CreateSurf;
		return FALSE;
	}

	// Fetch back buffer interface

	ddscaps.dwCaps=DDSCAPS_BACKBUFFER;
	ddrval=lpDDSPrimary->GetAttachedSurface(&ddscaps,&lpDDSBack);
	if (ddrval!=DD_OK) {
		ErrStr=Err_CreateSurf;
		return FALSE; 
	}

		/////////////DIRECT INPUT:::::::::::::::::::::::::
	if(DirectInputCreate(hInstance,DIRECTINPUT_VERSION,&lpDI,NULL)!=DI_OK)
	{
		ErrStr="Error in DirectInputCreate()";
		return FALSE;
	}

	///// KEYBOARD:::::::::::::::
	if(lpDI->CreateDevice(GUID_SysKeyboard, &pKeyboard,NULL)!=DI_OK)
	{
		ErrStr="Error in CreateDevice(GUID_SysKeyboard, &pKeyboard,NULL)";
		return FALSE;
	}
	if(pKeyboard->SetDataFormat(&c_dfDIKeyboard)!=DI_OK)
	{
		ErrStr="Error in SetDataFormat(&c_dfDIKeyboard)";
		return FALSE;
	}
	if(pKeyboard->SetCooperativeLevel(hWnd,DISCL_FOREGROUND|DISCL_NONEXCLUSIVE)!=DI_OK)
	{
		ErrStr="Error in pKeyboard->SetCooperativeLevel()";
		return FALSE;
	}
	if(pKeyboard->Acquire()!=DI_OK)
	{
		ErrStr="Error in pKeyboard->Acquire()";
		return FALSE;
	}
		///MOUSE:::::::::::::
	HRESULT hr;
    hr = lpDI->CreateDevice( GUID_SysMouse, &pMouse, NULL );
    if ( FAILED(hr) ) 
    {
		ErrStr="Error in CreateDevice( GUID_SysMouse, &pMouse, NULL )";
		return FALSE;
	}
	hr = pMouse->SetDataFormat( &c_dfDIMouse );
    if ( FAILED(hr) ) 
    {
		ErrStr="Error in pMouse->SetDataFormat()";
		return FALSE;
	}

    hr = pMouse->SetCooperativeLevel( hWnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
    if ( FAILED(hr) ) 
    {
		ErrStr="Error in pMouse->SetCooperativeLevel()";
		return FALSE;
	}
	if(pMouse->Acquire()!=DI_OK)
	{
		ErrStr="Error in pMouse->Acquire()";
		return FALSE;
	}

	// Create the DS object
	if (DirectSoundCreate(NULL, &lpDS, NULL) != DS_OK)
	{
		ErrStr = Err_DirectSoundCreate;
		return FALSE;
	}

	// Set the cooperation level for the DS object
	if (lpDS->SetCooperativeLevel(hWnd, DSSCL_NORMAL) != DS_OK)
	{
		ErrStr = Err_Coop;
		return FALSE;
	}

	// Initialize the DS buffers
	if (!load_sounds())
	{
		return FALSE;
	}
	object::setResolution(rX,rY);

	
	player1 = new player();
	player2 = new player();

	player1->setPaddle(new animObject());
	player2->setPaddle(new animObject()); 

		//--try to load all graphics--//


	
    curSchema=SPASE_SCHEMA;
	if(loadAllgraphics(curSchema) == FALSE)
	{
		DestroyWindow(hWnd);
		return FALSE;
	}
	if(!field.readF("MyLevel.map"))
	{
		ErrStr=Err_LoadLevel;
		DestroyWindow(hWnd);
		return FALSE;
	}
	for(int x=numLevels;x>0;x--)
	{
		char buff[50];
		sprintf(buff, "Level%d.map", x);
		if(!field.readF(buff))
		{
			ErrStr=Err_LoadLevel;
			DestroyWindow(hWnd);
			return FALSE;
		}
	}

	FILE *in;
	if((in=fopen("scores.txt","r"))==NULL)
		if((in=fopen("scores.txt","w+"))==NULL)
			return FALSE;
	for(int r=0; r<5; r++)
	{
		strcpy(scores[r].name,"empty"); 
		scores[r].score=0;
	}
	for(int k=0; k<5; k++)
	{
		if(fscanf(in, "%[a-z ]%d\n", scores[k].name, &scores[k].score)==EOF)
			break;
	}

	fclose(in);
	//scores[0].score=12345;
	//scores[0].name="dmitrii";

	curWindow = LOGO;
	logo.setLastTick(timeGetTime());
	pause=0;

		// return success to caller

	return TRUE;
}

BOOL loadAllgraphics(int schema)
{
	cur.bitmap_surface(lpDD,0,"cur.mgf",NULL,true);
	if(cur.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}

	bg.bitmap_surface(lpDD,0,"space.bmp",NULL,false);
	if(bg.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
	
	//SEND TO NATURE SCHEMA::
	bg_blue.bitmap_surface(lpDD,0,"bg_blue.bmp",NULL,true);
	if(bg_blue.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
	bg_green.bitmap_surface(lpDD,0,"bg_green.bmp",NULL,true);
	if(bg_green.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}

	tree.bitmap_surface(lpDD,0,"tree.bmp",NULL,true);
	if(tree.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}

	cloud.bitmap_surface(lpDD,0,"cloud.bmp",NULL,true);
	if(cloud.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}

	//:::SEND TO NATURE SCHEMA::
	water.bitmap_surface(lpDD,0,"water.bmp",NULL, true);
	if(water.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}

	//SEND TO WATER::


	//::WATER

	logo.bitmap_surface(lpDD,0,"mslogo.bmp",NULL,true);
	if(logo.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}

	moon.bitmap_surface(lpDD,0,"moon.bmp",NULL,true);
	if(moon.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}

	font.bitmap_surface(lpDD,0,"font.bmp",NULL,false);
	if(font.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
	
	print_cursor.bitmap_surface(lpDD,0,"print_cursor.bmp",NULL,false);
	if(print_cursor.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}

	repair.bitmap_surface(lpDD,0,"repair.bmp",NULL,true);
	if(repair.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}

	europa.bitmap_surface(lpDD,0,"europa.bmp",NULL,true);
	if(europa.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return NULL;}
	jupBG1.bitmap_surface(lpDD,0,"JupGB1.bmp",NULL,true);
	if(jupBG1.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return NULL;}
	jupBG1.bitmap_surface(lpDD,1,"JupGB2.bmp",NULL,true);
	if(jupBG1.getObjSurf(1)==NULL) {ErrStr=Err_LoadImage;return NULL;}
	jupBG1.setConstSizeX(96*(object::resX/267));
	jupBG1.setConstSizeY(object::resY);

	



	//main menu:::
	one_pl_btn.bitmap_surface(lpDD,0,"oneplayer_btn_up.bmp",NULL,false);
	if(one_pl_btn.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
	one_pl_btn.bitmap_surface(lpDD,1,"oneplayer_btn_down.bmp",NULL,false);
	if(one_pl_btn.getObjSurf(1)==NULL) {ErrStr=Err_LoadImage;return FALSE;}

	two_pl_btn.bitmap_surface(lpDD,0,"twoplayer_btn_up.bmp",NULL,false);
	if(two_pl_btn.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
	two_pl_btn.bitmap_surface(lpDD,1,"twoplayer_btn_down.bmp",NULL,false);
	if(two_pl_btn.getObjSurf(1)==NULL) {ErrStr=Err_LoadImage;return FALSE;}

	opt_btn.bitmap_surface(lpDD,0,"opt_btn_up.bmp",NULL,false);
	if(opt_btn.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
	opt_btn.bitmap_surface(lpDD,1,"opt_btn_down.bmp",NULL,false);
	if(opt_btn.getObjSurf(1)==NULL) {ErrStr=Err_LoadImage;return FALSE;}

	scores_btn.bitmap_surface(lpDD,0,"scores_btn_up.bmp",NULL,false);
	if(scores_btn.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
	scores_btn.bitmap_surface(lpDD,1,"scores_btn_down.bmp",NULL,false);
	if(scores_btn.getObjSurf(1)==NULL) {ErrStr=Err_LoadImage;return FALSE;}

	quit_btn.bitmap_surface(lpDD,0,"quit_btn_up.bmp",NULL,false);
	if(quit_btn.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
	quit_btn.bitmap_surface(lpDD,1,"quit_btn_down.bmp",NULL,false);
	if(quit_btn.getObjSurf(1)==NULL) {ErrStr=Err_LoadImage;return FALSE;}

	decor_paddle.bitmap_surface(lpDD,0,"decor_paddle_mm.bmp",NULL,false);
	if(decor_paddle.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
	decor_bricks.bitmap_surface(lpDD,0,"decor_bricks_mm.bmp",NULL,false);
	if(decor_bricks.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
	decor_logo.bitmap_surface(lpDD,0,"logo_mm.bmp",NULL,false);
	if(decor_logo.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
	bg_mm.bitmap_surface(lpDD,0,"bg_mm.bmp",NULL,false);
	if(bg_mm.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
	
	decor_ball.bitmap_surface(lpDD,0,"decor_ball1.bmp",NULL,true);
	if(decor_ball.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
	decor_ball.bitmap_surface(lpDD,1,"decor_ball2.bmp",NULL,true);
	if(decor_ball.getObjSurf(1)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
	decor_ball.bitmap_surface(lpDD,2,"decor_ball3.bmp",NULL,true);
	if(decor_ball.getObjSurf(2)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
	decor_ball.bitmap_surface(lpDD,3,"decor_ball4.bmp",NULL,true);
	if(decor_ball.getObjSurf(3)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
	
	decor_ball.setDelay(0.00001); 
	decor_ball.setCoord(object::resX/2-decor_ball.getConstSizeX()/2 ,object::resY/2-decor_ball.getConstSizeY()/2); 
	decor_ball.setSpeed(4);
	decor_ball.setUnitXY(cosDeg(200),-sinDeg(200));
	
	decor_paddle.setCoord(decor_paddle.getConstSizeX()/3, object::resY-object::resY/5); 
	//:::main menu

	//one player menu:::
	header_one_pl.bitmap_surface(lpDD,0,"header_one_pl.bmp",NULL,false);
	if(header_one_pl.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
	capt1_one_pl.bitmap_surface(lpDD,0,"capt1_one_pl.bmp",NULL,false);
	if(capt1_one_pl.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
	capt2_one_pl.bitmap_surface(lpDD,0,"capt2_one_pl.bmp",NULL,false);
	if(capt2_one_pl.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
	capt3_one_pl.bitmap_surface(lpDD,0,"capt3_one_pl.bmp",NULL,false);
	if(capt3_one_pl.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
	name_field_one_pl.bitmap_surface(lpDD,0,"name_field_one_pl.bmp",NULL,false);
	if(name_field_one_pl.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
	
	easy_btn.bitmap_surface(lpDD,0,"easy_btn_up.bmp",NULL,false);
	if(easy_btn.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
	easy_btn.bitmap_surface(lpDD,1,"easy_btn_down.bmp",NULL,false);
	if(easy_btn.getObjSurf(1)==NULL) {ErrStr=Err_LoadImage;return FALSE;}

	medium_btn.bitmap_surface(lpDD,0,"medium_btn_up.bmp",NULL,false);
	if(medium_btn.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
	medium_btn.bitmap_surface(lpDD,1,"medium_btn_down.bmp",NULL,false);
	if(medium_btn.getObjSurf(1)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
	
	hard_btn.bitmap_surface(lpDD,0,"hard_btn_up.bmp",NULL,false);
	if(hard_btn.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
	hard_btn.bitmap_surface(lpDD,1,"hard_btn_down.bmp",NULL,false);
	if(hard_btn.getObjSurf(1)==NULL) {ErrStr=Err_LoadImage;return FALSE;}

	campaign_btn.bitmap_surface(lpDD,0,"campaign_btn_up.bmp",NULL,false);
	if(campaign_btn.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
	campaign_btn.bitmap_surface(lpDD,1,"campaign_btn_down.bmp",NULL,false);
	if(campaign_btn.getObjSurf(1)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
	
	userbuilt_btn.bitmap_surface(lpDD,0,"userbuilt_btn_up.bmp",NULL,false);
	if(userbuilt_btn.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
	userbuilt_btn.bitmap_surface(lpDD,1,"userbuilt_btn_down.bmp",NULL,false);
	if(userbuilt_btn.getObjSurf(1)==NULL) {ErrStr=Err_LoadImage;return FALSE;}

	random_btn.bitmap_surface(lpDD,0,"random_btn_up.bmp",NULL,false);
	if(random_btn.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
	random_btn.bitmap_surface(lpDD,1,"random_btn_down.bmp",NULL,false);
	if(random_btn.getObjSurf(1)==NULL) {ErrStr=Err_LoadImage;return FALSE;}

	back_btn.bitmap_surface(lpDD,0,"back_btn_up.bmp",NULL,false);
	if(back_btn.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
	back_btn.bitmap_surface(lpDD,1,"back_btn_down.bmp",NULL,false);
	if(back_btn.getObjSurf(1)==NULL) {ErrStr=Err_LoadImage;return FALSE;}

	play_btn.bitmap_surface(lpDD,0,"play_btn_up.bmp",NULL,false);
	if(play_btn.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
	play_btn.bitmap_surface(lpDD,1,"play_btn_down.bmp",NULL,false);
	if(play_btn.getObjSurf(1)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
	
	//:::one player menu

	//two player::::::
	header_two_pl.bitmap_surface(lpDD,0,"header_two_pl.bmp",NULL,false);
	if(header_two_pl.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
	name_field_two_pl.bitmap_surface(lpDD,0,"name_field_one_pl.bmp",NULL,false);
	if(name_field_two_pl.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}

	//::::two player

	//options::::
	header_options.bitmap_surface(lpDD,0,"header_options.bmp",NULL,false);
	if(header_options.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
	capt1_options.bitmap_surface(lpDD,0,"capt1_options.bmp",NULL,false);
	if(capt1_options.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
	schema_space_btn.bitmap_surface(lpDD,0,"schema_space_btn_up.bmp",NULL,false);
	if(schema_space_btn.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
	schema_space_btn.bitmap_surface(lpDD,1,"schema_space_btn_down.bmp",NULL,false);
	if(schema_space_btn.getObjSurf(1)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
	
	schema_ocean_btn.bitmap_surface(lpDD,0,"schema_ocean_btn_up.bmp",NULL,false);
	if(schema_ocean_btn.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
	schema_ocean_btn.bitmap_surface(lpDD,1,"schema_ocean_btn_down.bmp",NULL,false);
	if(schema_ocean_btn.getObjSurf(1)==NULL) {ErrStr=Err_LoadImage;return FALSE;}

	schema_nature_btn.bitmap_surface(lpDD,0,"schema_nature_btn_up.bmp",NULL,false);
	if(schema_nature_btn.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
	schema_nature_btn.bitmap_surface(lpDD,1,"schema_nature_btn_down.bmp",NULL,false);
	if(schema_nature_btn.getObjSurf(1)==NULL) {ErrStr=Err_LoadImage;return FALSE;}

	controls_btn.bitmap_surface(lpDD,0,"controls_btn_up.bmp",NULL,false);
	if(controls_btn.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
	controls_btn.bitmap_surface(lpDD,1,"controls_btn_down.bmp",NULL,false);
	if(controls_btn.getObjSurf(1)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
	
	options_back_btn.bitmap_surface(lpDD,0,"options_back_btn_up.bmp",NULL,false);
	if(options_back_btn.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
	options_back_btn.bitmap_surface(lpDD,1,"options_back_btn_down.bmp",NULL,false);
	if(options_back_btn.getObjSurf(1)==NULL) {ErrStr=Err_LoadImage;return FALSE;}


	//:::options

	//controls::

	capt1_controls.bitmap_surface(lpDD,0,"capt1_controls.bmp",NULL,false);
	if(capt1_controls.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}

	controls_1player_btn.bitmap_surface(lpDD,0,"controls_1player_btn_up.bmp",NULL,false);
	if(controls_1player_btn.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
	controls_1player_btn.bitmap_surface(lpDD,1,"controls_1player_btn_down.bmp",NULL,false);
	if(controls_1player_btn.getObjSurf(1)==NULL) {ErrStr=Err_LoadImage;return FALSE;}

	controls_2player_btn.bitmap_surface(lpDD,0,"controls_2player_btn_up.bmp",NULL,false);
	if(controls_2player_btn.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
	controls_2player_btn.bitmap_surface(lpDD,1,"controls_2player_btn_down.bmp",NULL,false);
	if(controls_2player_btn.getObjSurf(1)==NULL) {ErrStr=Err_LoadImage;return FALSE;}

	LEFT_change_btn.bitmap_surface(lpDD,0,"controls_change_btn_up.bmp",NULL,false);
	if(LEFT_change_btn.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
	LEFT_change_btn.bitmap_surface(lpDD,1,"controls_change_btn_down.bmp",NULL,false);
	if(LEFT_change_btn.getObjSurf(1)==NULL) {ErrStr=Err_LoadImage;return FALSE;}

	RIGHT_change_btn.bitmap_surface(lpDD,0,"controls_change_btn_up.bmp",NULL,false);
	if(RIGHT_change_btn.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
	RIGHT_change_btn.bitmap_surface(lpDD,1,"controls_change_btn_down.bmp",NULL,false);
	if(RIGHT_change_btn.getObjSurf(1)==NULL) {ErrStr=Err_LoadImage;return FALSE;}

	FIRE_change_btn.bitmap_surface(lpDD,0,"controls_change_btn_up.bmp",NULL,false);
	if(FIRE_change_btn.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
	FIRE_change_btn.bitmap_surface(lpDD,1,"controls_change_btn_down.bmp",NULL,false);
	if(FIRE_change_btn.getObjSurf(1)==NULL) {ErrStr=Err_LoadImage;return FALSE;}

	control_usemouse_btn.bitmap_surface(lpDD,0,"control_usemouse_btn_up.bmp",NULL,false);
	if(control_usemouse_btn.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
	control_usemouse_btn.bitmap_surface(lpDD,1,"control_usemouse_btn_down.bmp",NULL,false);
	if(control_usemouse_btn.getObjSurf(1)==NULL) {ErrStr=Err_LoadImage;return FALSE;}

	controls_back_btn.bitmap_surface(lpDD,0,"options_back_btn_up.bmp",NULL,false);
	if(controls_back_btn.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
	controls_back_btn.bitmap_surface(lpDD,1,"options_back_btn_down.bmp",NULL,false);
	if(controls_back_btn.getObjSurf(1)==NULL) {ErrStr=Err_LoadImage;return FALSE;}

	controls_devider.bitmap_surface(lpDD,0,"controls_devider.bmp",NULL,true);
	if(controls_devider.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}

	//::controls

	//scores::
	scores_header.bitmap_surface(lpDD,0,"scores_header.bmp",NULL,false);
	if(scores_header.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}

	scores_table.bitmap_surface(lpDD,0,"scores_table.bmp",NULL,false);
	if(scores_table.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
	scores_table.setConstSizeX(object::resX*9/10);
	scores_table.setConstSizeY(object::resY*2/3);
	

	scores_clear_btn.bitmap_surface(lpDD,0,"scores_clear_btn_up.bmp",NULL,false);
	if(scores_clear_btn.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
	scores_clear_btn.bitmap_surface(lpDD,1,"scores_clear_btn_down.bmp",NULL,false);
	if(scores_clear_btn.getObjSurf(1)==NULL) {ErrStr=Err_LoadImage;return FALSE;}

	scores_back_btn.bitmap_surface(lpDD,0,"options_back_btn_up.bmp",NULL,false);
	if(scores_back_btn.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
	scores_back_btn.bitmap_surface(lpDD,1,"options_back_btn_down.bmp",NULL,false);
	if(scores_back_btn.getObjSurf(1)==NULL) {ErrStr=Err_LoadImage;return FALSE;}

	//::scores

	player1->LEFT_BTN=  's';
	player1->RIGHT_BTN= 'f';
	player1->FIRE_BTN=  'q';
	player2->LEFT_BTN=   37+32;//left key button
	player2->RIGHT_BTN=  39+32;//right key button
	player2->FIRE_BTN=   13+32;//enter key button

	player1->setUsingMouse(0);
	player2->setUsingMouse(0);

	player1->setGameOver(0); 
	player2->setGameOver(0); 
	
	player1->setName(new char[20]);
	player2->setName(new char[20]);
	strcpy(player1->getName(), "");
	strcpy(player2->getName(), "");
	player1->unfreeze();
	player2->unfreeze();
	emptyNameFlag=0;

	ufo.setBullet(new Ball);
	ufo.getBullet()->bitmap_surface(lpDD,0,"bullet.bmp",NULL,true);
	if(ufo.getBullet()->getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}



			
			brick.bitmap_surface(lpDD,WEAK_BRICK,"brick_weak.bmp",NULL,true);
			if(brick.getObjSurf(WEAK_BRICK)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
			brick.bitmap_surface(lpDD,MEDIUM_BRICK,"brick_medium.bmp",NULL,true);
			if(brick.getObjSurf(MEDIUM_BRICK)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
			brick.bitmap_surface(lpDD,CR1_MEDIUM_BRICK,"brick_medium_cracked1.bmp",NULL,true);
			if(brick.getObjSurf(CR1_MEDIUM_BRICK)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
			brick.bitmap_surface(lpDD,STRONG_BRICK,"brick_strong.bmp",NULL,true);
			if(brick.getObjSurf(STRONG_BRICK)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
			brick.bitmap_surface(lpDD,CR1_STRONG_BRICK,"brick_strong_cracked1.bmp",NULL,true);
			if(brick.getObjSurf(CR1_STRONG_BRICK)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
			brick.bitmap_surface(lpDD,CR2_STRONG_BRICK,"brick_strong_cracked2.bmp",NULL,true);
			if(brick.getObjSurf(CR2_STRONG_BRICK)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
			brick.bitmap_surface(lpDD,SPECIAL_BRICK,"brick_special.bmp",NULL,true);
			if(brick.getObjSurf(SPECIAL_BRICK)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
			brick.bitmap_surface(lpDD,UNDESTRUCTABLE,"brick_undestructable.bmp",NULL,true);
			if(brick.getObjSurf(UNDESTRUCTABLE)==NULL) {ErrStr=Err_LoadImage;return FALSE;}

			/*POWERUPS::::::::*/
			
			
			ballSpeedUp.bitmap_surface(lpDD,0,"ballSpeedUp_update1.bmp",NULL,true);
			if(ballSpeedUp.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
			ballSpeedUp.bitmap_surface(lpDD,1,"ballSpeedUp_update2.bmp",NULL,true);
			if(ballSpeedUp.getObjSurf(1)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
			ballSpeedUp.bitmap_surface(lpDD,2,"ballSpeedUp_update3.bmp",NULL,true);
			if(ballSpeedUp.getObjSurf(2)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
			ballSpeedUp.bitmap_surface(lpDD,3,"ballSpeedUp_update4.bmp",NULL,true);
			if(ballSpeedUp.getObjSurf(3)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
			ballSpeedUp.setType(1); 

			ballSpeedDown.bitmap_surface(lpDD,0,"ballSpeedDown_update1.bmp",NULL,true);
			if(ballSpeedDown.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
			ballSpeedDown.bitmap_surface(lpDD,1,"ballSpeedDown_update2.bmp",NULL,true);
			if(ballSpeedDown.getObjSurf(1)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
			ballSpeedDown.bitmap_surface(lpDD,2,"ballSpeedDown_update3.bmp",NULL,true);
			if(ballSpeedDown.getObjSurf(2)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
			ballSpeedDown.bitmap_surface(lpDD,3,"ballSpeedDown_update4.bmp",NULL,true);
			if(ballSpeedDown.getObjSurf(3)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
			ballSpeedDown.setType(2); 

			paddleSizeUp.bitmap_surface(lpDD,0,"paddle_size_up_powerup1.bmp",NULL,true);
			if(paddleSizeUp.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
			paddleSizeUp.bitmap_surface(lpDD,1,"paddle_size_up_powerup2.bmp",NULL,true);
			if(paddleSizeUp.getObjSurf(1)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
			paddleSizeUp.bitmap_surface(lpDD,2,"paddle_size_up_powerup3.bmp",NULL,true);
			if(paddleSizeUp.getObjSurf(2)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
			paddleSizeUp.bitmap_surface(lpDD,3,"paddle_size_up_powerup4.bmp",NULL,true);
			if(paddleSizeUp.getObjSurf(3)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
			paddleSizeUp.setType(3); 
			
			paddleSizeDown.bitmap_surface(lpDD,0,"paddle_size_down_powerup1.bmp",NULL,true);
			if(paddleSizeDown.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
			paddleSizeDown.bitmap_surface(lpDD,1,"paddle_size_down_powerup2.bmp",NULL,true);
			if(paddleSizeDown.getObjSurf(1)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
			paddleSizeDown.bitmap_surface(lpDD,2,"paddle_size_down_powerup3.bmp",NULL,true);
			if(paddleSizeDown.getObjSurf(2)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
			paddleSizeDown.bitmap_surface(lpDD,3,"paddle_size_down_powerup4.bmp",NULL,true);
			if(paddleSizeDown.getObjSurf(3)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
			paddleSizeDown.setType(4);
			
			ballIsPowerball.bitmap_surface(lpDD,0,"powerball_update1.bmp",NULL,true);
			if(ballIsPowerball.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
			ballIsPowerball.bitmap_surface(lpDD,1,"powerball_update2.bmp",NULL,true);
			if(ballIsPowerball.getObjSurf(1)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
			ballIsPowerball.bitmap_surface(lpDD,2,"powerball_update3.bmp",NULL,true);
			if(ballIsPowerball.getObjSurf(2)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
			ballIsPowerball.bitmap_surface(lpDD,3,"powerball_update4.bmp",NULL,true);
			if(ballIsPowerball.getObjSurf(3)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
			ballIsPowerball.setType(5);

			ballMultiBall.bitmap_surface(lpDD,0,"multiball_update1.bmp",NULL,true);
			if(ballMultiBall.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
			ballMultiBall.bitmap_surface(lpDD,1,"multiball_update2.bmp",NULL,true);
			if(ballMultiBall.getObjSurf(1)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
			ballMultiBall.bitmap_surface(lpDD,2,"multiball_update3.bmp",NULL,true);
			if(ballMultiBall.getObjSurf(2)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
			ballMultiBall.bitmap_surface(lpDD,3,"multiball_update4.bmp",NULL,true);
			if(ballMultiBall.getObjSurf(3)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
			ballMultiBall.setType(6);

			
			
			/*:::::POWERUPS*/
			
				
			
		

			player1->getPaddle()->bitmap_surface(lpDD,0,"paddle2.bmp",NULL,true);
			if(player1->getPaddle()->getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
			player2->getPaddle()->bitmap_surface(lpDD,0,"paddle3.bmp",NULL,true);
			if(player2->getPaddle()->getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}

			player1->getPaddle()->setCoord(0, object::resY - 20 );
			player2->getPaddle()->setCoord(object::resX-player2->getPaddle()->getConstSizeX(), object::resY - 20 );
			
			//player1->FIRE_BTN='O';
			//player1->LEFT_BTN=DIK_LEFT;
			//player1->RIGHT_BTN=DIK_RIGHT;

			//player2->FIRE_BTN='O';
			

			for(int k=0; k<50;k++)
			{
				Ball *temp = new Ball();
			
				temp->bitmap_surface(lpDD,0,"STAR1.bmp",NULL,true);
				if(temp->getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
				temp->bitmap_surface(lpDD,1,"STAR2.bmp",NULL,true);
				if(temp->getObjSurf(1)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
				temp->bitmap_surface(lpDD,2,"STAR3.bmp",NULL,true);
				if(temp->getObjSurf(2)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
				temp->bitmap_surface(lpDD,3,"STAR4.bmp",NULL,true);
				if(temp->getObjSurf(3)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
				
				player2->setBall(temp); 
				
				Ball *temp2 = new Ball();
			
				temp2->bitmap_surface(lpDD,0,"player2_ball1.bmp",NULL,true);
				if(temp2->getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
				temp2->bitmap_surface(lpDD,1,"player2_ball2.bmp",NULL,true);
				if(temp2->getObjSurf(1)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
				temp2->bitmap_surface(lpDD,2,"player2_ball3.bmp",NULL,true);
				if(temp2->getObjSurf(2)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
				temp2->bitmap_surface(lpDD,3,"player2_ball4.bmp",NULL,true);
				if(temp2->getObjSurf(3)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
				
				player1->setBall(temp2); 
				

			}
		

			player1->addBall();
			player2->addBall(); 
			player1->getBall(0)->setCoord(player1->getPaddle()->getCurX()+player1->getPaddle()->getConstSizeX()/2,player1->getPaddle()->getCurY()-player1->getBall(0)->getConstSizeY()-1); 
			player2->getBall(0)->setCoord(player2->getPaddle()->getCurX()+player2->getPaddle()->getConstSizeX()/2,player2->getPaddle()->getCurY()-player2->getBall(0)->getConstSizeY()-1); 
			
			player1->getBall(0)->setUnitXY(cosDeg(90),-sinDeg(90));
			player2->getBall(0)->setUnitXY(cosDeg(90),-sinDeg(90));

			player1->getBall(0)->setSpeed(4);
			player2->getBall(0)->setSpeed(4);


			player1->setScore(0);
			player2->setScore(0); 
			isTwoPlayerGame=false;
			
			ufo.bitmap_surface(lpDD,0,"UFO1.bmp",NULL,true);
			if(ufo.getObjSurf(0)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
			ufo.bitmap_surface(lpDD,1,"UFO2.bmp",NULL,true);
			if(ufo.getObjSurf(1)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
			ufo.bitmap_surface(lpDD,2,"UFO3.bmp",NULL,true);
			if(ufo.getObjSurf(2)==NULL) {ErrStr=Err_LoadImage;return FALSE;}
			ufo.bitmap_surface(lpDD,3,"UFO4.bmp",NULL,true);
			if(ufo.getObjSurf(3)==NULL) {ErrStr=Err_LoadImage;return FALSE;}

			ufo.setDelay(0.01); 
			ufo.setCoord(0,0); 
			ufo.setUnitXY(-0.5,-0.5);
			ufo.setSpeed(0.5); 




	return TRUE;
}

BOOL load_sounds()
{
    // Initialize waves
    CWave waves[NUMSOUNDS];
    waves[0].Create("Earth7.wav"); 
  

    // Initialize secondary DS buffers
    for (int i = 0; i < NUMSOUNDS; i++) {
        // Get the wave information
        DWORD         dwDataLen = waves[i].GetDataLen();
        WAVEFORMATEX  wfFormat;
        waves[i].GetFormat(wfFormat);

        // Setup the DS buffer description
        DSBUFFERDESC  dsbdDesc;
        ZeroMemory(&dsbdDesc, sizeof(DSBUFFERDESC));
        dsbdDesc.dwSize = sizeof(DSBUFFERDESC);
        dsbdDesc.dwFlags = DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPAN |
					DSBCAPS_CTRLVOLUME | DSBCAPS_STATIC;
        dsbdDesc.dwBufferBytes = dwDataLen;
        dsbdDesc.lpwfxFormat = &wfFormat;

        // Create the DS buffer
        if (lpDS->CreateSoundBuffer(&dsbdDesc,
            &lpDSBSounds[i], NULL) != DS_OK)
        {
            ErrStr = Err_CreateBuff;
            return FALSE;
        }

        // Lock the DS buffer
        BYTE* pDSBuffData;
        if (lpDSBSounds[i]->Lock(0, dwDataLen, (void**)&pDSBuffData,
            &dwDataLen, NULL, 0, 0) != DS_OK)
        {
            ErrStr = Err_LoadWAV;
            return FALSE;
        }

        // Write wave data to the DS buffer
        dwDataLen = waves[i].GetData(pDSBuffData, dwDataLen);

        // Unlock the DS buffer
        if (lpDSBSounds[i]->Unlock(pDSBuffData, dwDataLen, NULL, 0) !=
            DS_OK)
        {
            ErrStr = Err_LoadWAV;
            return FALSE;
        }
    }

    return TRUE;
}

//------ Application Loop ------//

int APIENTRY WinMain(HINSTANCE hInstance, 
					 HINSTANCE hPrevInstance,
					 LPSTR lpCmdLine, int nCmdShow)
{
	// initialize the application, exit on failure

	/* Creation of dialog box, that asks for resolution */
	HINSTANCE     g_hInstance=NULL;
	if(! DialogBox(g_hInstance,
                   MAKEINTRESOURCE(IDD_RESDIALOG),
                   NULL, (DLGPROC) SampleDlgProc)
	   )return FALSE;


    if (!Init(hInstance, nCmdShow)) 
	{
		Cleanup();
		//PostQuitMessage(0); 
        return 0;
	}

	// run till completed 
	bool  nd           =true;

	DWORD dwFrameTime  = 0;    // Time of the last frame.
	DWORD dwFrameCount = 0;   // Frames displayed.
	MSG   msg;   
	
	while(nd)
	{
		if(PeekMessage(&msg,NULL,0,0, PM_REMOVE)) 
		{
			if(msg.message==WM_QUIT)
				nd=false;
			
			TranslateMessage(&msg);
			DispatchMessage(&msg); 
		}
		else
		{
			/////////////////MAIN LOOP////////////////////
			if(curWindow==GAME && !pause)
			checkKeyboard();
			checkMouse();

			DWORD    dwTime;
			dwTime = timeGetTime();
			if(!pause)
			{
			    if(draw_slide()) 
				   dwFrameCount++;
			}
				
			dwTime = timeGetTime() - dwFrameTime;
			if ( dwTime > 1000 )
			{
				dwFrames = ( dwFrameCount*1000 )/dwTime;
				dwFrameTime = timeGetTime();
				dwFrameCount = 0;
			}
		
		}
	}

	// exit returning final message
    return (msg.wParam);
}

