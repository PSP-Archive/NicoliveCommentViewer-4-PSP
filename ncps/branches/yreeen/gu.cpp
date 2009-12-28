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

GUData guData;

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


	char temp[128];
	// 試験的にデータを追加してみるー
	for(int q=1; q<8; q++)
	{
		// コメ番
		sprintf(temp, "%d", q);
		lview->getColumn(0)->addItem(temp);
		
		// コメント内容
		sprintf(temp, "(＃＾ω＾)ﾋﾟｷﾋﾟｷ");
		lview->getColumn(1)->addItem(temp);

		// ID/コテハン
		sprintf(temp, "%p", (void *)(lview->getColumn(2)->itemlist_current));
		lview->getColumn(2)->addItem(temp);	
	}
	lview->Render(NULL, NULL);

	char *mail_init = NULL, *pass_init = NULL;
	char *mail = NULL, *pass = NULL;
	while(1)
	{	
		GUSTART;
		sceGuClearColor(WHITE);
		sceGuClearDepth(0);
		GUCLEAR;
		intraFontSetStyle(jpn0, 0.8f, BLACK, 0, NULL);

		if(!(oldpad.Buttons & PSP_CTRL_SQUARE)
			&& (currpad.Buttons & PSP_CTRL_SQUARE))
		{
			GUT_stat->setValue("■");
			FREE(mail);
			FREE(pass);
			LoginDialog("unko_king@live.jp", "unkoking", mail, pass);
			if(mail != NULL && pass != NULL)
			{
				FREE(userData.mail);
				FREE(userData.pass);
				userData.mail = (char *)malloc(strlen(mail)+1);
				userData.pass = (char *)malloc(strlen(pass)+1);
				strcpy(userData.mail, mail);
				strcpy(userData.pass, pass);
			}
		}

		lview->Render(&currpad, NULL);
		lview->Draw(0, 0);
		intraFontPrint(jpn0, 0, 480-13-13, userData.mail);
		intraFontPrint(jpn0, 240, 480-13-13, userData.pass);
		intraFontPrint(jpn0, 0, 480-13-13, userData.user_session);
		//menubar->Draw();

		GUT_stat->Draw(0, 272-GUTextBox::GUTEXTBOX_HEIGHT, 480);

		GUFINISH;
		GUSYNC;
		GUFLIP;

		sceKernelDelayThread(50 * 1000);
	}

	return 0;
}

