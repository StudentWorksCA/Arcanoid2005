#ifndef _player_
#define _player_

#include "class.h"
#include "ball.h"
#include "upgrade.h"
#include "phisics.h"
#include <dinput.h>
#include <vector>

using namespace std;

#define MAX_BALLS 20

int charToDinput(int c);
void getWordFromChar(char c, char *buff);
class player
{
protected:
	char *m_name;
	int m_score;

	animObject *m_paddle;
	//Ball *m_balls[50];
	int m_isFrozen;
	//int m_hmBalls;
	int m_ballsLeft;
	vector <Ball *> m_balls_on;
	vector <Ball *> m_balls_off;

	int m_useMouse;
	int m_isGameOver;

public:
	int LEFT_BTN;
	int RIGHT_BTN;
	int FIRE_BTN;


public:

	int isGameOver(){return m_isGameOver;}
	void setGameOver(int isGameOver){m_isGameOver=isGameOver;}

	int isUsingMouse(){return m_useMouse;}
	void setUsingMouse(int isMouseUsed){m_useMouse=isMouseUsed;}
	
	inline int getBallsLeft() {return m_ballsLeft;}
	inline setBallsLeft(int bl) {m_ballsLeft=bl;}
	inline void setName(char *name) {m_name=name;}
	inline int getScore() {return m_score;}

	inline void setScore(int score) {m_score=score;}
	inline char *getName()  {return m_name;}
	inline void increaseScoreBy(int score){m_score+=score;}
	void decreaseScoreBy(int score)
	{
		if(m_score-score>=0)
			m_score-=score;
		else score=0;
	}
	
	inline void setPaddle(animObject *paddle) {m_paddle=paddle;}
	inline animObject *getPaddle() {return m_paddle;}

	inline void freeze() {m_isFrozen=1;}
	inline void unfreeze() {m_isFrozen=0;}
	int isFrozen() {return m_isFrozen;}

	//inline void setHmBalls(int hmBalls) {m_hmBalls = hmBalls;}
	inline int getHmBalls() {return m_balls_on.size();}

	inline void setBall(Ball *ball) {m_balls_off.push_back(ball);}

	inline Ball *getBall(int i) {return m_balls_on.at(i);}
	void addBall() 
	{
		if(getHmBalls()<MAX_BALLS)
		{
			m_balls_on.push_back(m_balls_off.back());
			m_balls_off.pop_back();
		}
	}
	void removeBall(int i)
	{
		m_balls_on.at(i)->setPowerBall(0);
		m_balls_off.push_back(m_balls_on.at(i));
		m_balls_on.erase(m_balls_on.begin()+i);
	}

	void addPowerUp(upgrade *up)
	{
		int i;
		switch(up->getType())
		{
		case 1: //balls speed up
			for(i=0; i<m_balls_on.size();i++)
				if(m_balls_on.at(i)->getSpeed()+1<=15)
				m_balls_on.at(i)->setSpeed(m_balls_on.at(i)->getSpeed()+1);
			break;
		case 2: //balls speed down
			for(i=0; i<m_balls_on.size();i++)
				if(m_balls_on.at(i)->getSpeed()-1>1)
					m_balls_on.at(i)->setSpeed(m_balls_on.at(i)->getSpeed()-1);
			break;
		case 3: //paddle upzise
			if(m_paddle->getConstSizeX()+20<=object::resX/4 )
			{
				m_paddle->setConstSizeX(m_paddle->getConstSizeX()+20); 
				m_paddle->setCoord(m_paddle->getCurX()-10,m_paddle->getCurY());
			}
			break;
		case 4: //paddle downsize
			if(m_paddle->getConstSizeX()-20>=object::resX/30 )
			{
				m_paddle->setConstSizeX(m_paddle->getConstSizeX()-20); 
				m_paddle->setCoord(m_paddle->getCurX()+10,m_paddle->getCurY());
			}
			break;
		case 5: //powerball
			for(i=0; i<m_balls_on.size();i++)
				m_balls_on.at(i)->setPowerBall(1);
			break;
		case 6: //multiball
			if(getHmBalls()>0)
			{
				if(getHmBalls()<MAX_BALLS)
				{
					addBall();
					m_balls_on.at(getHmBalls()-1)->setCoord(m_balls_on.at(0)->getCurX(), m_balls_on.at(0)->getCurY());
					m_balls_on.at(getHmBalls()-1)->setUnitXY(m_balls_on.at(0)->getUnitX(), m_balls_on.at(0)->getUnitY());
					m_balls_on.at(getHmBalls()-1)->setSpeed(m_balls_on.at(0)->getSpeed());
					turnVector(m_balls_on.at(getHmBalls()-1), 120);
				}
				if(getHmBalls()<MAX_BALLS)
				{
					addBall();
					m_balls_on.at(getHmBalls()-1)->setCoord(m_balls_on.at(0)->getCurX(), m_balls_on.at(0)->getCurY());
					m_balls_on.at(getHmBalls()-1)->setUnitXY(m_balls_on.at(0)->getUnitX(), m_balls_on.at(0)->getUnitY());
					m_balls_on.at(getHmBalls()-1)->setSpeed(m_balls_on.at(0)->getSpeed());
					turnVector(m_balls_on.at(getHmBalls()-1), 240);
				}
			}
			break;

		default:
			break;

		}
	}

	void initForNewLevel(int ballsLeft)
	{
		for(int i=m_balls_on.size()-1; i>=0; i--)
			removeBall(i);
		m_ballsLeft=ballsLeft;
		m_isFrozen=0;
	}



};

#endif