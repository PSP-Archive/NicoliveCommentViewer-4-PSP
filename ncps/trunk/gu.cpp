#include <psptypes.h>
#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspdebug.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include <pspgu.h>
#include <pspgum.h>

#include "types.h"

#include "Libs/intraFont/intraFont.h"

#include "Libs/NicoAPI.h"
#include "Libs/NetworkClass.h"

#include "Libs/GUDraw.h"
#include "Libs/GUMenu.h"
#include "Libs/GUDiag.h"
#include "Libs/GUListView.h"


#include "gu.h"
#include "dialogs.h"


unsigned int __attribute__((aligned(16))) list[262144];
int DrawThread(SceSize args, void *argp)
{
	FILE *fp;
	char trace_buf[64];

	guData.inited = false;
	guData.started = false;


	sceGuInit();
	sceGuStart(GU_DIRECT, list);

	sceGuDrawBuffer(GU_PSM_8888, (void*)0, BUF_WIDTH);
	sceGuDispBuffer(SCR_WIDTH, SCR_HEIGHT, (void*)0x88000, BUF_WIDTH);
	sceGuDepthBuffer((void*)0x110000, BUF_WIDTH);
 
	sceGuOffset(2048 - (SCR_WIDTH/2), 2048 - (SCR_HEIGHT/2));
	sceGuViewport(2048, 2048, SCR_WIDTH, SCR_HEIGHT);
	sceGuDepthRange(65535, 0);
	sceGuScissor(0, 0, SCR_WIDTH, SCR_HEIGHT);
	sceGuEnable(GU_SCISSOR_TEST);
	sceGuDepthFunc(GU_GEQUAL);
	sceGuEnable(GU_DEPTH_TEST);
	sceGuFrontFace(GU_CW);
	sceGuShadeModel(GU_SMOOTH);
	sceGuEnable(GU_CULL_FACE);
	sceGuEnable(GU_CLIP_PLANES);
	sceGuEnable(GU_BLEND);
	sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
	sceGuFinish();
	sceGuSync(0,0);
 
	sceDisplayWaitVblankStart();
	sceGuDisplay(GU_TRUE);

	guData.inited = true;

	while(1)
	{	
		GUSTART;
		sceGuClearColor(WHITE);
		sceGuClearDepth(0);
		GUCLEAR;
		intraFontSetStyle(jpn0, 0.8f, BLACK, 0, NULL);

		lview->Draw(0, 0);
		GUT_stat->Draw(0, 272-GUTextBox::GUTEXTBOX_HEIGHT, 480);

	//	intraFontPrintf(jpn0, 0, 50, "&oldpad: %p", &oldpad);
	//	intraFontPrintf(jpn0, 0, 70, "currpad.Buttons: %08X", currpad.Buttons);
	//	intraFontPrintf(jpn0, 0, 90, "oldpad.Buttons: %08X", oldpad.Buttons);



		//intraFontPrint(jpn0, 0, 480-13-13, userData.mail);
		//intraFontPrint(jpn0, 240, 480-13-13, userData.pass);
		//intraFontPrint(jpn0, 0, 480-13-13, userData.user_session);
		//menubar->Draw();


		GUFINISH;
		GUSYNC;
		GUFLIP;

		sceKernelDelayThread(50 * 1000);
	}

	return 0;
}

