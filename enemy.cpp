#include "stdafx.h"
#include "enemy.h"

void enemy::endShot()
{
	m_isSooting = false; 
	m_bullet->setCoord(-1, -1);
}

int enemy::letBulletFolowPaddle(object *paddle)
{
	double x1,y1,x2,y2;
	
	x2=paddle->getCurX()+ paddle->getCurSizeX()/2;
	y2=paddle->getCurY()+ paddle->getCurSizeY()/2;
	
	x1=getCurX()+ getCurSizeX()/2;
	y1=getCurY()+ getCurSizeY()/2;

	double len = sqrt((x2-x1)*(x2-x1)+(y1-y2)*(y1-y2));
	this->getBullet()->setUnitXY((x2-x1)/len, (y2-y1)/len ); 

	return 1;
} 

void enemy::shoot() 
{
	m_bullet->setCoord(this->getCurX()+this->getCurSizeX()/2, 
		this->getCurY()+this->getCurSizeY()/2);
	m_isSooting = true;
}
