#include <windows.h>
#include <windowsx.h>
#include <ddraw.h>
#include <math.h>
#include <stdio.h>
#include "class.h"


#ifndef __timer2_h__
#define __timer2_h__

class landscape//: public object
{
private:
	
	int screenX, screenY;
	
	int hmX;
	int hmY;
	int arr[500][500];

public:
	landscape(){};
	
	int getHmX(){return hmX;}
	int getHmY(){return hmY;}

	int getTypeAt(int i,int j){return arr[i][j];}
	void setTypeAt(int type, int i, int j) {arr[i][j] = type;}

	//functions, returning coordinates of brick by given array indexes 
	double getXAt(int j, object brick);
	double getYAt(int i, object brick);

	int readF(char fn[]);
	int save();
	
};
#endif