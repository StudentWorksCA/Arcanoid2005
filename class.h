#include <windows.h>
#include <windowsx.h>
#include <ddraw.h>
#include <math.h>
/********************************************************************************/
/*						Main Kernel for Graphics Engines by						*/
/*								MAYOR systems									*/
/********************************************************************************/
#ifndef __timer_h__
#define __timer_h__
#define MAX_SURF 50
class object
{
	protected:
		double m_x;
		double m_y;
		double cur_x;
		double cur_y;
		int m_type;
		double constSizeX;
		double constSizeY;
		double curSizeX;
		double curSizeY;
		double realSizeX;
		double realSizeY;

		int m_curImage;
		RECT dr;
		LPDIRECTDRAWSURFACE4 lpObjectSurf[MAX_SURF]; //surface of the object
		int m_hmImages;
	
		
	public:
		static double resX, resY;
		object(int x,int y){cur_x=m_x=x;cur_y=m_y=y;}
		object(){m_curImage=0;}

		
		double getDistance(int x, int y);
		
		int IsContact(object ob);
		/****************CANNOT USE THIS TOGETHER!! USE setCoord!!*************/	
		/**/  double inline setX(double x) {cur_x=m_x=x; checkOut();return m_x;}	/**/
		/**/  double inline setY(double y) {cur_y=m_y=y; checkOut();return m_y;}	/**/
		/**********************************************************************/
		/*int inline setCurX(int x) {return cur_x=x;}//
		//int inline setCurY(int y) {return cur_y=y;}*/

		int inline setCoord(double x,double y) {cur_x=m_x=x;cur_y=m_y=y;checkOut();return 1;}

		//Should call checkOut() after setConstSizeX/Y !!!
			int inline setConstSizeX(double cSizeX) {curSizeX=constSizeX=cSizeX;return 1;}
			int inline setConstSizeY(double cSizeY) {curSizeY=constSizeY=cSizeY;return 1;}

		int inline setType(int type) {return m_type=type;}

		double inline getX(double *x) {*x=m_x;return m_x;}
		double inline getX() {return m_x;}
		double inline getCurX(double *x) {*x=cur_x;return cur_x;}
		double inline getCurX() {return cur_x;}

		double inline getY(double *y) {*y=m_y;return m_y;}
		double inline getY() {return m_y;}
		double inline getCurY(double *y) {*y=cur_y;return cur_y;}
		double inline getCurY() {return cur_y;}
																														
			double inline getConstSizeX() {return constSizeX;}																
			double inline getConstSizeY() {return constSizeY;}	

			double inline getCurSizeX() {return curSizeX;}	
			double inline getCurSizeY() {return curSizeY;}	

		int inline getType(int *type) {*type=m_type;}
		int inline getType() {return m_type;}

		int inline getNumOfObjects(int *num) {*num=m_hmImages; return m_hmImages;}
		int inline getNumOfObjects() {return m_hmImages;}
	
		int inline getCurImage() {return m_curImage;}	
		int inline setCurImage(int cIm) {m_curImage = cIm;}	

		int  getNextImage();
		void goNextImage();

		RECT inline getRect(){return dr;}
		RECT		getCRect();
		int IsRandomSuccess(int posibility);
		
		
		
		LPDIRECTDRAWSURFACE4 bitmap_surface(LPDIRECTDRAW4 lpDD,int index,LPCTSTR file_name,RECT *dims,bool black);

		int setSurfaceToObject(int index,LPDIRECTDRAWSURFACE4 lpSurf);
		void checkOut();
		void checkOutSingle();

		LPDIRECTDRAWSURFACE4 inline getObjSurf(int index)
		{	
			if(index<MAX_SURF)
			return lpObjectSurf[index];
			else
				return NULL;
		}
		void static setResolution(double x, double y){resX=x; resY=y;}
		/*I don	t use them (using object::resX or object::resY), but  ...still...*/
		double static getResX(){ return resX;}
		double static getResY(){ return resY;}

		double getRealSizeX(){return realSizeX;}
		double getRealSizeY(){return realSizeY;}
	int isInRect(double x, double y);
	private:
		void restoreOriginalObjectSettings();
		

};

//Class anim Object
class animObject: public object
{
protected:
	
	DWORD m_dwLastTick;
public:

	animObject(){setLastTick(timeGetTime());}
	
	inline void setLastTick(int LastTick) {m_dwLastTick=LastTick;}
	inline DWORD getLastTick() {return m_dwLastTick;}

	bool hasTimePassed(DWORD howLong);

};

#endif