#include "class.h"

#ifndef _ball_
#define _ball_



class Ball: public animObject
{
protected:
	int m_type;
	int m_value;

	double m_unitX;
	double m_unitY;
	double m_delay;
	double m_speed;
	int m_powerBall;

public:
	Ball() {m_powerBall=0;}

 	inline void setType(int type) {m_type=type;}
	inline void setValue(int value) {m_value=value;}

	inline int getType()  {return m_type;}
	inline int getValue() {return m_value;}

	inline void setUnitXY(double _x, double _y) {m_unitX=_x; m_unitY= _y;}
	inline double getUnitX()  {return m_unitX;}
	inline double getUnitY()  {return m_unitY;}

	inline void setSpeed(double _speed) {m_speed = _speed;}
	inline double getSpeed()  {return m_speed;}

	//get step (unitX*speed)
	inline double getStepX() {return double(m_unitX*m_speed/**m_delay*/);}
	inline double getStepY() {return double(m_unitY*m_speed/**m_delay*/);}

	//delay used to update coordinates accordingly with time
	inline void setDelay(double delay) {if(delay >= 0.0001 && delay < 1) m_delay=delay;}
	inline double getDelay(){return m_delay;}

	inline void setPowerBall(int p) {m_powerBall=p;}
	inline bool isPowerBall() {if(m_powerBall) return true; else return false;}

};
#endif