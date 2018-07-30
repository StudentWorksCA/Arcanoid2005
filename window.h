#ifndef _window_
#define _window_

#include "class.h"


class Button: public object
{
	bool m_pressed;
	bool m_outClick;
public:
	Button(){m_pressed=false;m_outClick=false;}

	bool isClicked(int mouse_state, double x, double y)
	{
		if(mouse_state && !this->isInRect(x,y))
			m_outClick=true;
		else if(mouse_state && this->isInRect(x,y) && !m_outClick)
			m_pressed = true;

		if(!mouse_state && m_pressed && this->isInRect(x,y))
		{
			m_pressed=false;
			return true;
		}
		else if(!mouse_state)
		{
			m_pressed=false;
			m_outClick=false;
		}

		return false;
	}

	bool isDown(double x, double y)
	{
		if(m_pressed && this->isInRect(x,y))
			return true;
		else return false;
	}

};

#endif