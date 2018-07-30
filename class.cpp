#include "stdafx.h"
#include "class.h"


double object::resX = 800;
double object::resY = 600;
double object::getDistance(int x, int y)
		{
			//double a = ((x-(this->m_x+curSizeX/2))*(x-(this->m_x+curSizeX/2))) + (((this->m_y+curSizeY/2)-y)*((this->m_y+curSizeY/2)-y)));
			return 0;//sqrt(abs(int(a));
		}

int object::IsContact(object ob)
		{
			if(ob.getX()==m_x && ob.getY()==m_y)
			{
				return 1;
			}
			return 0;
		}

int object::IsRandomSuccess(int posibility)
		{
			int rn=0;
			rn=(1+rand()%(posibility));
			if(rn == int(posibility/2))
				return rn;
			else
				return 0;
		}




int object::setSurfaceToObject(int index, LPDIRECTDRAWSURFACE4 lpSurf)
		{	if(index<50)
			lpObjectSurf[index] = lpSurf;
			if(lpObjectSurf[index]) {m_hmImages++;return 1; }
			else			 return 0;
		}




LPDIRECTDRAWSURFACE4 object::bitmap_surface(LPDIRECTDRAW4 lpDD,int index,LPCTSTR file_name,RECT *dims=NULL,bool black=true)
{
	HDC hdc;
	HBITMAP bit;
	LPDIRECTDRAWSURFACE4 surf;
	DDCOLORKEY  bColorKey;

	// load the bitmap
	if(index>=50) return NULL;

	bit=(HBITMAP) LoadImage(NULL,file_name,IMAGE_BITMAP,0,0,
								LR_DEFAULTSIZE|LR_LOADFROMFILE);
	if (!bit) 

		// failed to load, return failure to caller

		return NULL;

	// get bitmap dimensions

	BITMAP bitmap;
    GetObject( bit, sizeof(BITMAP), &bitmap );
	int surf_width=bitmap.bmWidth;
	int surf_height=bitmap.bmHeight;

	constSizeX=curSizeX=realSizeX=bitmap.bmWidth;
	constSizeY=curSizeY=realSizeY=bitmap.bmHeight;
	dr.bottom = long(realSizeY);
	dr.left =0;
	dr.right = long(realSizeX);
	dr.top = 0;
	// create surface

	HRESULT ddrval;
	DDSURFACEDESC2 ddsd;
	ZeroMemory(&ddsd,sizeof(ddsd));
	ddsd.dwSize = sizeof(DDSURFACEDESC2);
	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT ;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN; 
	
	ddsd.dwWidth = surf_width;
	ddsd.dwHeight = surf_height; 
	
	// attempt to create surface

	ddrval=lpDD->CreateSurface(&ddsd,&surf,NULL);

	// created ok?
	if(black)
	{
	bColorKey.dwColorSpaceLowValue = RGB(0, 0,0 );
	bColorKey.dwColorSpaceHighValue = RGB(0, 0, 0);
	}
	else
	{
	bColorKey.dwColorSpaceLowValue = RGB(255,255,255 );
	bColorKey.dwColorSpaceHighValue = RGB(255, 255,255);
	}
    surf->SetColorKey(DDCKEY_SRCBLT, &bColorKey);
	if (ddrval!=DD_OK) {

		// no, release the bitmap and return failure to caller

		DeleteObject(bit);
		return NULL;

	} else {

		// yes, get a DC for the surface

		surf->GetDC(&hdc);

		// generate a compatible DC

		HDC bit_dc=CreateCompatibleDC(hdc);

		// blit the interface to the surface

		SelectObject(bit_dc,bit);
		BitBlt(hdc,0,0,surf_width,surf_height,bit_dc,0,0,SRCCOPY);

		// release the DCs

		surf->ReleaseDC(hdc);
		DeleteDC(bit_dc);

		// save the dimensions if rectangle pointer provided

		
	}
	
	// clear bitmap 
	DeleteObject(bit);

	if(index==0) m_hmImages=0;

	lpObjectSurf[index] = surf;
	if(lpObjectSurf[index]) m_hmImages++;
	else					return NULL;
	// return pointer to caller

	return surf;
}

		
void object::checkOut()
{		 /*_________________________________*/	
		   restoreOriginalObjectSettings();		
		 /*---------------------------------*/
	if(m_x+constSizeX>=resX) {curSizeX=resX-m_x; dr.right=long(realSizeX-((constSizeX-curSizeX)*realSizeX/constSizeX)); } //shipX=1;
	if(cur_x<=0) {m_x =0;curSizeX = constSizeX + cur_x;dr.left = long(0 - cur_x*realSizeX/constSizeX);}
	if(cur_y<=0) {m_y =0;curSizeY = constSizeY + cur_y;dr.top  = long(0 - cur_y*realSizeY/constSizeY);}
	if(m_y+constSizeY>=resY) {curSizeY=resY-m_y; dr.bottom=long(realSizeY-((constSizeY-curSizeY)*realSizeY/constSizeY));} //shipY=0;
	
}

void object::checkOutSingle()
{		 
	if(m_x+constSizeX>=resX) {curSizeX=resX-m_x; dr.right=long(realSizeX-((constSizeX-curSizeX)*realSizeX/constSizeX)); } //shipX=1;
	if(cur_x<=0) {m_x =0;curSizeX = constSizeX + cur_x;dr.left = long(0 - cur_x*realSizeX/constSizeX);}
	if(cur_y<=0) {m_y =0;curSizeY = constSizeY + cur_y;dr.top  = long(0 - cur_y*realSizeY/constSizeY);}
	if(m_y+constSizeY>=resY) {curSizeY=resY-m_y; dr.bottom=long(realSizeY-((constSizeY-curSizeY)*realSizeY/constSizeY));} //shipY=0;

}


void object::restoreOriginalObjectSettings()
{
	curSizeX=constSizeX;
	curSizeY=constSizeY;

	dr.bottom = long(realSizeY);
	dr.left =0;
	dr.right = long(realSizeX);
	dr.top = 0;

}

RECT object::getCRect()
{
	RECT rr;
	rr.bottom = long(curSizeY+m_y);//  shipsizeY+shipY;
	rr.left =long(m_x);
	rr.right = long(curSizeX+m_x);//  shipsizeX+shipX;
	rr.top = long(m_y);
	
return rr;
}

int object::getNextImage()
{
	if(m_curImage < m_hmImages-1)
	{
		m_curImage++;
		return m_curImage;
	}
	else
		return m_curImage=0;
} 
void object::goNextImage()
{
	if(m_curImage < m_hmImages-1)
		m_curImage++;
	else
		m_curImage=0;
} 
int object::isInRect(double x, double y)
{
	if(x >= getCurX() && x<= getCurX()+getConstSizeX() &&
		y>=getCurY() && y<= getCurY()+getConstSizeY())
		return 1;
	else return 0;
}

/********************************************************
//animObject implementaton
/********************************************************/
bool animObject::hasTimePassed(DWORD howLong)
{
	
	DWORD m_dwCurrTick=timeGetTime();
	if(m_dwCurrTick - m_dwLastTick > howLong)
	{
		m_dwLastTick=m_dwCurrTick;
		return true;
	}
	else
		return false;
}
