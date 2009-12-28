#ifndef _GUDECL
#define _GUDECL

#include <pspkernel.h>

typedef struct _GUData
{
	bool inited;
	bool started;
}GUData;
extern GUData guData;

#define GUSTART		{if(!guData.started){sceGuStart(GU_DIRECT, list);guData.started = true;}}
#define GUCLEAR		{if(guData.inited){GUSTART;sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);}}
#define GUFINISH	{if(guData.started){sceGuFinish();guData.started = false;}}
#define GUSYNC		{GUFINISH;sceGuSync(0,0);}
#define GUFLIP		{if(guData.inited){GUSYNC;sceDisplayWaitVblankStart();sceGuSwapBuffers();}}


int DrawThread(SceSize args, void *argp);

#endif