#ifndef _GUDRAWDECL
#define _GUDRAWDECL

#include "../gu.h"

#define FREE(_Memory) if(_Memory!=NULL){free(_Memory);_Memory=NULL;}

#define GUENABLE(STATUS) {if(!sceGuGetStatus(STATUS))sceGuEnable(STATUS);}
#define GUDISABLE(STATUS) {if(sceGuGetStatus(STATUS))sceGuDisable(STATUS);}

#define ARGB(a, r, g, b) ((a << 24) | (r << 16) | (g << 8) | b)
#define RGBA(r, g, b, a) ((a << 24) | (b << 16) | (g << 8) | r)

typedef struct _Vertex
{
	unsigned int  color;
	float x, y, z;
}Vertex;

enum colors
{
	RED =	0xFF0000FF,
	GREEN =	0xFF00FF00,
	BLUE =	0xFFFF0000,
	WHITE =	0xFFFFFFFF,
	LITEGRAY = 0xFFBFBFBF,
	GRAY =  0xFF7F7F7F,
	DARKGRAY = 0xFF3F3F3F,		
	BLACK = 0xFF000000,	
};

void DrawSquare(float x1, float y1, float x2, float y2, unsigned int color, bool fillflg);

void DrawPolygon(float *x, float *y, int count, unsigned int color, bool fillflg);
void DrawLine(float x1, float y1, float x2, float y2, unsigned int color);
void DrawLineEx(float x1, float y1, float x2, float y2, float width, unsigned int color);


#endif
