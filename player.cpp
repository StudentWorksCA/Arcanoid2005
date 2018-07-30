#include "stdafx.h"
#include "player.h"  

	int charToDinput(int c)
	{
		switch(c)
		{
		case 97:  return DIK_A;
		case 98:  return DIK_B;
		case 99:  return DIK_C;
		case 100: return DIK_D;
		case 101: return DIK_E;
		case 102: return DIK_F;
		case 103: return DIK_G;
		case 104: return DIK_H;
		case 105: return DIK_I;
		case 106: return DIK_J;
		case 107: return DIK_K;
		case 108: return DIK_L;
		case 109: return DIK_M;
		case 110: return DIK_N;
		case 111: return DIK_O;
		case 112: return DIK_P;
		case 113: return DIK_Q;
		case 114: return DIK_R;
		case 115: return DIK_S;
		case 116: return DIK_T;
		case 117: return DIK_U;
		case 118: return DIK_V;
		case 119: return DIK_W;
		case 120: return DIK_X;
		case 121: return DIK_Y;
		case 122: return DIK_Z;

		case 37+32: return DIK_LEFT;
		case 38+32: return DIK_UP;
		case 39+32: return DIK_RIGHT;
		case 40+32: return DIK_DOWN;
		case 32+32: return DIK_SPACE;
		case 13+32: return DIK_RETURN;
		}
		return 0;
	}
	void getWordFromChar(char c, char *buff)
	{
		strcpy(buff, "");
		if(c>='a' && c<='z') sprintf(buff, "%c", c);
		else if(c==37+32) strcpy(buff, "left");
		else if(c==38+32) strcpy(buff, "up");
		else if(c==39+32) strcpy(buff, "right");
		else if(c==40+32) strcpy(buff, "down");
		else if(c==32+32) strcpy(buff, "space");
		else if(c==13+32) strcpy(buff, "enter");
	    else strcpy(buff, "undefined");
	}