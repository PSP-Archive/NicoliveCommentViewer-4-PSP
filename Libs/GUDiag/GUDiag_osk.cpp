#include <stdlib.h>
#include <string.h>
#include <pspkernel.h>
#include <pspgu.h>
#include <pspdisplay.h>
#include <psputility.h>

#include "../GUDiag_osk.h"
#include "../GUDraw.h"
#include "../cpcnv.h"

extern unsigned int __attribute__((aligned(16))) list[262144];

int GetOSKText(char *buf, int buflen, int inputmode, const char *title, const char *init)
{
	if(buf == NULL)return -1;

	u16 *winit, *wtitle, *wresult;
	char nulstr[] = "";
	if(init == NULL) init = nulstr;
	if(title == NULL)title = nulstr;
	winit = (u16 *)malloc((strlen(init) + 1) * 2);
	wtitle = (u16 *)malloc((strlen(title) + 1) * 2);
	wresult = (u16 *)malloc((buflen + 1) * 2);

	if(winit == NULL || wtitle == NULL || wresult == NULL)
	{
		free(winit);
		free(wtitle);
		free(wresult);
		return -1;
	}

	
	dxpCpCode_toUcs2(winit,strlen(init) + 1,(u8*)init,DXP_CP_SJIS);
	dxpCpCode_toUcs2(wtitle,strlen(title) + 1,(u8*)title,DXP_CP_SJIS);


	SceUtilityOskData data;
	memset(&data, 0, sizeof(SceUtilityOskData));
	sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_LANGUAGE, &data.language);
	data.unk_00 = 1;	//äøéö
	data.lines = 1;
	data.unk_24 = 1;
	data.inputtype = inputmode;
	data.desc = wtitle;
	data.intext = winit;
	data.outtextlength = buflen;
	data.outtextlimit = buflen;
	data.outtext = wresult;

	SceUtilityOskParams params;
	memset(&params,0x00,sizeof(params));
	params.base.size = sizeof(params);
	sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_LANGUAGE,&params.base.language);
	sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_UNKNOWN,&params.base.buttonSwap);
	params.base.graphicsThread = 0x11;
	params.base.accessThread = 0x13;
	params.base.fontThread = 0x12;
	params.base.soundThread = 0x10;
	params.datacount = 1;
	params.data = &data;

	sceUtilityOskInitStart(&params);

	int done = 0;
	while(!done)
	{
		GUSTART
		sceGuClearDepth(0);
		GUCLEAR
		DrawSquare(0, 0, 480, 272, BLACK, true);
		GUENABLE(GU_TEXTURE_2D);
		GUFINISH
		//GUSYNC

		switch(sceUtilityOskGetStatus())
		{
			case PSP_UTILITY_DIALOG_INIT:
				break;
			
			case PSP_UTILITY_DIALOG_VISIBLE:
				sceUtilityOskUpdate(1); // 2 Ç…Ç∑ÇÈÇ∆ë¨ìxå¸è„!?
				break;
			
			case PSP_UTILITY_DIALOG_QUIT:
				sceUtilityOskShutdownStart();
				break;
			
			case PSP_UTILITY_DIALOG_FINISHED:
				break;
				
			case PSP_UTILITY_DIALOG_NONE:
				done = 1;
				break;

			default:
				break;
		}
		GUFLIP
	}

	free(winit);
	free(wtitle);

	switch(data.result)
	{
		case PSP_UTILITY_OSK_RESULT_UNCHANGED:
			break;

		case PSP_UTILITY_OSK_RESULT_CHANGED:
			dxpCpSJIS_fromUcs2((u8*)buf,buflen,wresult);
			break;

		case PSP_UTILITY_OSK_RESULT_CANCELLED:
			break;
	
		default:
			break;
	}
	free(wresult);	

	return data.result;
}