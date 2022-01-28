#ifndef RECPP_H

#define RECPP_H
#include "raylib.h"

//RecPP.h version 1.2
//changelog
//1.1.1
    //added reczero
//1.2 added rotate rec

#define RECZERO (Rectangle){0,0,0,0}

Rectangle addVec2ToRec(Rectangle rec, Vector2 vec2);

Rectangle newRec(int x, int y, int sizeX, int sizeY);

Rectangle addRec(Rectangle rec1, Rectangle rec2, bool addSize);

Rectangle resizeRec(Rectangle input, float scale);

Rectangle combineVec2(Vector2 position, Vector2 size);

bool isPointInRec(Rectangle rec, Vector2 point);
Vector2 recToVec(Rectangle input);

Rectangle RotateRecCW(Rectangle rec);

#endif // recpp_h