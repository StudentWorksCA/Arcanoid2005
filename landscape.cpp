#include "stdafx.h"
#include "landscape.h"


int landscape::readF(char fn[])
{
	FILE *in;
	int curN=0;
	hmX=0,hmY=0;
	if((in=fopen(fn,"r"))==NULL)
		return 0;
	if((hmX = fgetc(in))==EOF)
		return 0;
	if((hmY = fgetc(in))==EOF)
		return 0;
	for(int i=0;i<hmY;i++)
	{
		for(int j=0;j<hmX;j++)
		{
			curN=fgetc(in);
			if(curN==EOF || curN<0 || curN>8)
				return 0;
			else
				arr[i][j]=curN;
				//printf("%d",arr[i][j]);
		}
	}
	fclose(in);
	return 1;
}

int landscape::save()
{
	FILE *in;
	int curN=0;
	int hmx=0,hmy=0;
	hmx=hmX; hmy=hmY;
	if((in=fopen("aaa1.map","w"))==NULL)
		return 0;

		fputc(hmX,in);
	fputc(hmY,in);
	for(int i=0;i<hmY;i++)
	{ 
		for(int j=0;j<hmX;j++)
		{
			fputc(arr[i][j],in);
		}
	}
	fclose(in);
	return 1;

}


double landscape::getXAt(int j, object brick)
{
	/*RECT r;
	r.left = 0;r.top = 0;r.right = long(object.resX) ;r.bottom = long(object.resY);
	double marginX=(r.right/this->getHmX()-brick.getCurSizeX())/2;
	return marginX+(j*(marginX*2+brick.getCurSizeX()));*/
	double marginX = object.resX/2-(this->getHmX()*brick.getCurSizeX())/2;
	return marginX + j*brick.getCurSizeX();

}

double landscape::getYAt(int i, object brick)
{
	/*RECT r;
	r.left = 0;r.top = 0;r.right = long(object.resX) ;r.bottom = long(object.resY);
	double marginY=(r.bottom/this->getHmY()/3-brick.getCurSizeY())/2;
	return marginY+(i*(marginY*2+brick.getCurSizeY()));*/
	double marginY = object.resY/3-(this->getHmY()*brick.getCurSizeY())/2;
	return marginY + i*brick.getCurSizeY();


}