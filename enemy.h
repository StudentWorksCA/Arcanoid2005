#include "ball.h"

#ifndef _enemy_
#define _enemy_



class enemy: public Ball
{
protected:
	int m_type;
	int m_value;

	bool m_isSooting;

	Ball *m_bullet;
	int m_whichPaddle;

public:

	
	inline void setType(int type) {m_type=type;}
	inline void setValue(int value) {m_value=value;}


	inline int getType()  {return m_type;}
	inline int getValue() {return m_value;}
	bool isShooting() {return m_isSooting;}

	void shoot();
	void endShot();

	void setBullet(Ball *bullet) {m_bullet=bullet;}
	Ball *getBullet() {return m_bullet;}

	void setPaddleToShoot(int p) {m_whichPaddle = p;}
	int getPaddleToShoot() {return m_whichPaddle;}

	int letBulletFolowPaddle(object *paddle);

};
#endif