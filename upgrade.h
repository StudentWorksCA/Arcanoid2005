#include "ball.h"
#include <vector>
using namespace std;

#ifndef _upgrade_
#define _upgrade_



class upgrade: public Ball
{
protected:
	int m_type;
	
public:
	upgrade(){}

	upgrade(const upgrade& p) 
	{
		m_x = p.m_x;
		m_y= p.m_y;
		cur_x= p.cur_x;
		cur_y= p.cur_y;
		m_type= p.m_type;
		constSizeX= p.constSizeX;
		constSizeY= p.constSizeY;
		curSizeX= p.curSizeX;
		curSizeY= p.curSizeY;
		realSizeX= p.realSizeX;
		realSizeY= p.realSizeY;

		m_curImage = p.m_curImage;
		dr = p.dr;
		for(int i=0;i<p.m_hmImages;i++)
			lpObjectSurf[i] = p.lpObjectSurf[i]; 
		m_hmImages=p.m_hmImages;
	
	}

	
	inline void setType(int type) {m_type=type;}
	inline int getType()  {return m_type;}

};

#endif