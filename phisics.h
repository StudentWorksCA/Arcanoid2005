

#ifndef _phisics_
#define _phisics_

#include "ball.h"

int gFileChange(char *argv, int plus);


//updates vector of ball
int updateVectorOf(Ball *obj, object *paddle,bool flyAwayAble);
int updateVectorOf(Ball *obj,bool flyAwayAble);
//updates any object's coordinates on screen (keeps it in bounds)
int updatePositionOf(object *obj);

//updates any coordinates on screen (keeps it in bounds)
int updateCoords(int *x, int *y);

//update paddle collision (used in updateVectorOf)
int updatePaddleCollision(object *paddle, Ball *obj); 

//turns vector on certain angle
int turnVector(Ball *obj, int angle);

//detects collision
int isCollide(Ball *obj, object *brick);
int updateBrickCollision(Ball *obj, object *brick);

//allows to enter angles in degrees
double sinDeg(double deg);
double cosDeg(double deg);

int getRandom(int lower, int upper);

#endif