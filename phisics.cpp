#include "stdafx.h"
#include "phisics.h"

#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <math.h>

int gFileChange(char *argv, int plus)
{

	int fd;
	int buff[2];
	
	if((fd=open(argv, O_RDWR))==-1)
	{
		//printf("File Error\n");
		return 0;
	}

	read(fd, buff,2);

	lseek(fd, -2, SEEK_CUR);
	if(plus)
	{
		buff[0]++;
		buff[1]++;
	}
	else
	{
		buff[0]--;
		buff[1]--;
	}
	write(fd,buff,2);
	
	return 1;
}
int getRandom(int lower, int upper)
{
	int _num=0;
	if(lower < 0 || upper < 0)
		return -1;

	_num=lower+int((upper-lower)*rand()/(RAND_MAX + 1.0)); 
	return _num;
}

int updateVectorOf(Ball *obj,bool flyAwayAble)
{
	double unitX = obj->getUnitX();
	double unitY = obj->getUnitY();
	
	double x = obj->getCurX(); 
	double y = obj->getCurY();
	
	double height = obj->getConstSizeY();
	double width  = obj->getConstSizeX(); 

	double stepX = obj->getStepX();// unitX * obj->getSpeed();
	double stepY = obj->getStepY();// unitY * obj->getSpeed();

	if(x + stepX <= 0)
	{
		obj->setCoord(0, y);
		obj->setUnitXY(-unitX, unitY); 
	}
	if(x + stepX + width > obj->getResX() )
	{
		obj->setCoord(obj->getResX()-width, y);
		obj->setUnitXY(-unitX, unitY); 
	}
	if(y + stepY <= 0)
	{
		obj->setCoord(x, 0);
		obj->setUnitXY(unitX, -unitY); 
	}
	if(flyAwayAble && y + stepY > obj->getResY())
	{
		
		obj->setCoord(x, obj->getResY()-height);
		obj->setUnitXY(unitX, -unitY); 
		return 0; // if go out of the screen (down)
	}
	if(!flyAwayAble && y + height + stepY > obj->getResY())
	{
		obj->setCoord(x, obj->getResY()-height);
		obj->setUnitXY(unitX, -unitY); 
	}
	return 1;
}
int updateVectorOf(Ball *obj, object *paddle,bool flyAwayAble)
{
	int rc=1;
	rc=updateVectorOf(obj,flyAwayAble);
	updatePaddleCollision(paddle, obj);

	return rc;
}


int updatePositionOf(object *obj)
{


	double x = obj->getCurX(); 
	double y = obj->getCurY();
	
	double height = obj->getConstSizeY();
	double width  = obj->getConstSizeX(); 

	
	if(x <= 0)
		obj->setCoord(0, y);

	if(x+width > obj->getResX() )

		obj->setCoord(obj->getResX()-width, y);
	
	if(y <= 0)
		obj->setCoord(x, 0);

	if(y + height > obj->getResY())
		obj->setCoord(x, obj->getResY()-height);

	return 1;
}

int updateCoords(int *x, int *y)
{
	
	if(*x <= 0)
		*x=0;

	if(*x > long(object::resX))

		*x=long(object::resX);
	
	if(*y <= 0)
		*y=0;

	if(*y > long(object::resY))
		*y=long(object::resY);

	return 1;
}

double sinDeg(double deg)
{
	return sin(deg*(3.141592654/180.0));
}

double cosDeg(double deg)
{
	return cos(deg*(3.141592654/180.0));
}


int updatePaddleCollision(object *paddle, Ball *obj)
{


	double left = double(obj->getCurX()); 
	double top = double(obj->getCurY());
	
	double height = double(obj->getConstSizeY());
	double width  = double(obj->getConstSizeX()); 

	double right = left+width;
	double bottom= top+height;


	double Pleft = double(paddle->getCurX()); 
	double Ptop = double(paddle->getCurY());
	
	double Pheight = double(paddle->getConstSizeY());
	double Pwidth  = double(paddle->getConstSizeX()); 

	double Pright = Pleft+Pwidth;
	double Pbottom= Ptop+Pheight;

	double Pcenter = Pwidth/2;

	double hit = left+width/2;

	double unitX = obj->getUnitX();
	double unitY = obj->getUnitY();
	
	double stepX = obj->getStepX() ;
	double stepY = obj->getStepY();

	if(hit >= Pleft && hit <=Pright && bottom >= Ptop && top < Ptop)
	{
		//1st half
		if(hit >= Pleft && hit <= Pleft+Pcenter)
		{
			unitX = -((Pleft+Pcenter - hit)/(Pcenter));
			//if(unitX < cosDeg(160))
			//	unitX = cosDeg(160);
			//else if(unitX <=cosDeg(100) && unitX >= cosDeg(120))
			//	unitX = cosDeg(120);
			unitY = sqrt(1-unitX*unitX);
		}
		//2nd half
		else if(hit > Pleft+Pcenter && hit <=Pright)
		{
			unitX = (hit-(Pleft+Pcenter))/(Pcenter);
			//if(unitX > cosDeg(20.0))
			//	unitX = cosDeg(20.0);
			//else if(unitX >=cosDeg(60) && unitX <= cosDeg(80))
			//	unitX = cosDeg(60);
			unitY = sqrt(1-unitX*unitX);
		}
		unitY=-unitY;
		obj->setUnitXY(unitX,unitY);
		obj->setCoord(obj->getCurX(), Ptop-height); 
		
	}
	if(hit >= Pleft && hit <=Pright && top <= Pbottom && top > Ptop)
	{
		unitY=-unitY;
		obj->setUnitXY(unitX,unitY);
		obj->setCoord(obj->getCurX(), Pbottom); 
	}

	return 1;
}



int turnVector(Ball *obj, int angle)
{
	obj->setUnitXY(
		(cosDeg(angle) * obj->getUnitX()) + (-sinDeg(angle) * obj->getUnitY()),
		(sinDeg(angle) * obj->getUnitX()) + ( cosDeg(angle) * obj->getUnitY())
		);
	return 1;
}
double BallX;
double BallY;
double BallWidth;
double BallHight;

double brickX;
double brickY;
double brickWidth;
double brickHight;

double ballCtrX;
double ballCtrY;

double brickCtrX;
double brickCtrY;


double projX;
double projY;

double curAngle;

double convRad = 180 / 3.141592654;
double rangeAngle;

int updateBrickCollision(Ball *obj, object *brick)
{

	//if collision:
	if(isCollide(obj, brick))
	{
		BallX = obj->getCurX() ; 
		BallY = obj->getCurY() ; 
		BallWidth = obj->getConstSizeX(); 
		BallHight = obj->getConstSizeY(); 

		brickX = brick->getCurX(); 
		brickY = brick->getCurY(); 
		brickWidth = brick->getConstSizeX(); 
		brickHight = brick->getConstSizeY(); 

		ballCtrX = BallX+BallWidth/2;
		ballCtrY = BallY+BallHight/2;

		brickCtrX = brickX+brickWidth/2;
		brickCtrY = brickY+brickHight/2;


		projX = ballCtrX-brickCtrX;
		projY = brickCtrY-ballCtrY;

	 	if(projX > 0 && projY > 0)
			curAngle = atan(projY/projX)*convRad;
		else if(projX < 0 && projY > 0)
			curAngle = 180 + atan(projY/projX)*convRad;
		else if(projX < 0 && projY < 0)
			curAngle = 180 + atan(projY/projX)*convRad; 
		else if(projX > 0 && projY < 0)
			curAngle = 360 + atan(projY/projX)*convRad; 

		// half of range angle:
		if(projX<0)projX=-projX;
		if(projY<0)projY=-projY;

		rangeAngle = atan(((BallHight+brickHight)/2)/((BallWidth+brickWidth)/2))*convRad;

		//right hit
		if( (curAngle >= 0 && curAngle<= rangeAngle) || (curAngle > 360-rangeAngle && curAngle <360))
		{
			obj->setUnitXY(-obj->getUnitX(), obj->getUnitY() );
			//obj->setCoord(obj->getCurX()+ obj->getStepX(), obj->getCurY()+ obj->getStepY());
			obj->setCoord(brickX+brickWidth+ obj->getStepX(), BallY+obj->getStepY());
		
		}
		//top hit
		else if(curAngle > rangeAngle && curAngle < 180-rangeAngle)
		{
			obj->setUnitXY(obj->getUnitX(), -obj->getUnitY() );
			//obj->setCoord(obj->getCurX()+ obj->getStepX(), obj->getCurY()+ obj->getStepY());
			obj->setCoord(BallX+obj->getStepX(), brickY-BallHight + obj->getStepY());
			

		}
		//left hit
		else if(curAngle >= 180-rangeAngle && curAngle < 180+rangeAngle)
		{
			obj->setUnitXY(-obj->getUnitX(), obj->getUnitY() );
			//obj->setCoord(obj->getCurX()+ obj->getStepX(), obj->getCurY()+ obj->getStepY());
			obj->setCoord(brickX-BallWidth + obj->getStepX(), BallY+obj->getStepY());
			
        
		}
		//bottom hit
		else if(curAngle > 180+rangeAngle && curAngle < 360-rangeAngle)
		{
			obj->setUnitXY(obj->getUnitX(), -obj->getUnitY() );
			//obj->setCoord(obj->getCurX()+ obj->getStepX(), obj->getCurY()+ obj->getStepY());
			obj->setCoord(BallX+obj->getStepX(), brickY + brickHight + obj->getStepY());
		}
		return 1;
	}
	return 0;
}


int isCollide(Ball *obj, object *brick)
{

	projX = (obj->getCurX()+obj->getStepX()+obj->getConstSizeX()/2)-(brick->getCurX()+brick->getConstSizeX()/2);
	projY = (brick->getCurY()+brick->getConstSizeY()/2)-(obj->getCurY()+obj->getStepY()+obj->getConstSizeY()/2);

	if(projX<0)projX=-projX;
	if(projY<0)projY=-projY;

	if(projX <= (obj->getConstSizeX()+brick->getConstSizeX())/2 && 
		projY <= (obj->getConstSizeY()+brick->getConstSizeY())/2)
		return 1;
	else return 0;


}