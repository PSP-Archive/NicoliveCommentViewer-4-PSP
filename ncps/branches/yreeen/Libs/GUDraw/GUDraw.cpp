#include <pspgu.h>
#include <pspdisplay.h>
#include <math.h>


#include "../GUDraw.h"


void DrawSquare(float x1, float y1, float x2, float y2, unsigned int color, bool fillflg)
{
	Vertex *vertices;
	
	if(fillflg)
	{
		// draw a square and fill inside
		vertices = (Vertex *)sceGuGetMemory(2 * sizeof(Vertex));

		vertices[0].color = color;
		vertices[0].x = x1;
		vertices[0].y = y1;
		vertices[0].z = 0.0f;

		vertices[1].color = color;
		vertices[1].x = x2;
		vertices[1].y = y2 + 1.0f; //‚¿‚å‚Á‚Æ’á‚­‚È‚é‚Á‚Û‚¢
		vertices[1].z = 0.0f;

		GUDISABLE(GU_TEXTURE_2D);
		sceGuDrawArray(GU_SPRITES, GU_COLOR_8888|GU_VERTEX_32BITF|GU_TRANSFORM_2D, 2, 0, vertices);
	}
	else
	{
		vertices = (Vertex *)sceGuGetMemory(5 * sizeof(Vertex));
		
		for(int i=0; i<5; i++)
		{
			vertices[i].color = color;
			vertices[i].z = 0.0f;
		}

		vertices[0].x = x1;
		vertices[0].y = y1;

		vertices[1].x = x2 - 1.0f;
		vertices[1].y = y1;

		vertices[2].x = x2 - 1.0f;
		vertices[2].y = y2;

		vertices[3].x = x1;
		vertices[3].y = y2;

		vertices[4].x = x1;
		vertices[4].y = y1;
		
		GUDISABLE(GU_TEXTURE_2D);
		sceGuDrawArray(GU_LINE_STRIP, GU_COLOR_8888|GU_VERTEX_32BITF|GU_TRANSFORM_2D, 5, 0, vertices);
	}
}


void DrawPolygon(float *x, float *y, int count, unsigned int color)
{
        Vertex *vertices = (Vertex *)sceGuGetMemory((count+1) * sizeof(Vertex));

        for(int i=0; i<count; i++)
        {
                vertices[i].color = color;
                vertices[i].x = x[i];
                vertices[i].y = y[i];
                vertices[i].z = 0.0f;
        }

        vertices[count].color = color;
        vertices[count].x = x[0];
        vertices[count].y = y[0];
        vertices[count].z = 0.0f;

		GUDISABLE(GU_TEXTURE_2D);
        sceGuDrawArray(GU_LINE_STRIP, GU_COLOR_8888|GU_VERTEX_32BITF|GU_TRANSFORM_2D, count+1, 0, vertices);
}

void FillPolygon(float *x, float *y, int count, unsigned int color)
{
        Vertex *vertices = (Vertex *)sceGuGetMemory(count * sizeof(Vertex));

        for(int i=0; i<count; i++)
        {
                vertices[i].color = color;
                vertices[i].x = x[i];
                vertices[i].y = y[i];
                vertices[i].z = 0.0f;
        }

		GUDISABLE(GU_TEXTURE_2D);
		sceGuDrawArray(GU_TRIANGLE_FAN, GU_COLOR_8888|GU_VERTEX_32BITF|GU_TRANSFORM_2D, count, 0, vertices);
}

void DrawLineEx(float x1, float y1, float x2, float y2, float width, unsigned int color)
{
        float dy=y2-y1;
        float dx=x2-x1;
        if(dy==0 && dx==0)
		{
			return;
		}

        float l = (float)hypot(dx, dy);

        float x[4];
        float y[4];

        x[0] = x1+width*(y2-y1)/l;
        y[0] = y1-width*(x2-x1)/l;

        x[1] = x1-width*(y2-y1)/l;
        y[1] = y1+width*(x2-x1)/l;

        x[2] = x2-width*(y2-y1)/l;
        y[2] = y2+width*(x2-x1)/l;

        x[3] = x2+width*(y2-y1)/l;
        y[3] = y2-width*(x2-x1)/l;

        FillPolygon(x, y, 4, color);
}


