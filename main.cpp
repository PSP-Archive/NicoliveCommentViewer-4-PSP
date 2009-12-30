#include <pspkernel.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <pspsdk.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

/*
#include <pspnet.h>
#include <pspnet_inet.h>
#include <pspnet_apctl.h>
#include <pspnet_resolver.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <netdb.h>
*/

#include <pspgu.h>
#include <pspdisplay.h>

#include "Libs/NetworkClass.h"
#include "Libs/NicoAPI.h"
#include "Libs/intraFont/intraFont.h"

#include "types.h"

#include "Libs/GUDraw.h"
#include "Libs/GUMenu.h"
#include "Libs/GUDiag.h"
#include "Libs/GUListView.h"

#include "gu.h"
#include "net.h"
#include "render.h"



//PSP_MODULE_INFO("NCPS", PSP_MODULE_USER, 1, 1);
//PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);

PSP_MODULE_INFO("NCPS", 0x1000, 1, 1);
PSP_MAIN_THREAD_ATTR(0);

PSP_HEAP_SIZE_KB(-1);



intraFont *ltn[16];
intraFont *jpn0;
intraFont *kr0;
intraFont *arib;
intraFont *chn;

bool g_controllable = true;
GUMenu *menubar;
GUListView *lview;
GUTextBox *GUT_stat;

int DrawThreadId = -1;
int RenderThreadId = -1;
int NetThreadId = -1;
int RecvThreadId = -1;

GUData guData;
ConData conData;
LiveData liveData;
UserData userData;
ThreadData thData;

SceCtrlData currpad;


int ExitCallback(int arg1, int arg2, void *common)
{
	for(int i=0; i<16; i++)
	{
		intraFontUnload(ltn[i]);
	}

	intraFontUnload(jpn0);
	intraFontUnload(kr0);
	intraFontUnload(arib);
	intraFontUnload(chn);
	
	intraFontShutdown();

	pspSdkInetTerm();
	sceGuTerm();
	sceKernelExitGame();
	return 0;
}

int SetupThread(SceSize args, void *argp)
{
	int cbid;

	cbid = sceKernelCreateCallback("ExitCallback", ExitCallback, NULL);
	sceKernelRegisterExitCallback(cbid);
	sceKernelSleepThreadCB();

	return 0;
}


int SetupCallback(void)
{
	int thid = 0;

	thid = sceKernelCreateThread("SetupThread", SetupThread, 0x11, 0xFA0, PSP_THREAD_ATTR_USER, 0);
	if(thid >= 0)
	{
		sceKernelStartThread(thid, 0, 0);
	}

	return thid;
}




int main(int argc, char *argv[])
{
	SetupCallback();		//�I�����̃R�[���o�b�N���Z�b�g
	pspDebugScreenInit();

	// �K�v�ȃ��W���[�������[�h
	if(pspSdkLoadInetModules() < 0)
	{
		pspDebugScreenPrintf("failed to load network modules\n");
		sceKernelSleepThread();
	}

	pspDebugScreenPrintf("Loading intraFont...\n");
	intraFontInit();
	char file[40];
	for(int i=0; i<16; i++)
	{
		sprintf(file, "flash0:/font/ltn%d.pgf", i);
		ltn[i] = intraFontLoad(file, 0);
		
		intraFontSetStyle(ltn[i], 1.0f, WHITE, DARKGRAY, 0);
	}

	jpn0 = intraFontLoad("flash0:/font/jpn0.pgf", INTRAFONT_STRING_SJIS|INTRAFONT_CACHE_ALL);
	intraFontSetStyle(jpn0, 0.8f, WHITE, 0, 0);

	kr0 = intraFontLoad("flash0:/font/kr0.pgf", INTRAFONT_STRING_UTF8);
	intraFontSetStyle(kr0, 0.8f, WHITE, 0, 0);

	arib = intraFontLoad("flash0:/font/arib.pgf", 0);
	intraFontSetStyle(arib, 0.8f, WHITE, 0, 0);

	chn = intraFontLoad("flash0:/font/gb3s1518.bwfon", 0);
	intraFontSetStyle(chn, 0.8f, WHITE, 0, 0);

	intraFontSetAltFont(ltn[8], jpn0);                     //japanese font is used for chars that don't exist in latin
	intraFontSetAltFont(jpn0, chn);                        //chinese font (bwfon) is used for chars that don't exist in japanese (and latin)
	intraFontSetAltFont(chn, kr0);                         //korean font is used for chars that don't exist in chinese (and jap and ltn)
	intraFontSetAltFont(kr0, arib);                        //symbol font is used for chars that don't exist in korean (and chn, jap & ltn)

	pspDebugScreenPrintf("IntraFont loaded\n");
	
	menubar = new GUMenu;
	menubar->setColumn(0, "�ڑ�");
	menubar->getColumn(0)->setItem(0, "AP�ɐڑ�", NULL);
	menubar->getColumn(0)->getItem(0)->setValid();
	menubar->getColumn(0)->setItem(1, "���O�C��", NULL);
	menubar->getColumn(0)->getItem(1)->setValid();
	menubar->getColumn(0)->setItem(2, "�ԑg�ɐڑ�", NULL);
	menubar->getColumn(0)->getItem(2)->setValid();
	menubar->getColumn(0)->setValid();

	menubar->setColumn(1, "�ҏW");
	menubar->getColumn(1)->setItem(0, "���O������", NULL);
	menubar->getColumn(1)->getItem(0)->setValid();
	menubar->getColumn(1)->setValid();

	menubar->setState(GUMenu::GUMENU_STATE_HIDE);


	lview = new GUListView(3, 100, 9);
	lview->setColumn(0, "");
	lview->getColumn(0)->width = 30;
	lview->getColumn(0)->setValid();
	lview->getColumn(0)->setScrollStyle(GUTextBox::GUTEXTBOX_STYLE_NOSCROLL);

	lview->setColumn(1, "�R�����g");
	lview->getColumn(1)->width = 300;
	lview->getColumn(1)->setValid();
	
	lview->setColumn(2, "ID");
	lview->getColumn(2)->width = 480 - 300;
	lview->getColumn(2)->setValid();


	GUT_stat = new GUTextBox;
	GUT_stat->setStyle(GUTextBox::GUTEXTBOX_STYLE_SCROLL);
	GUT_stat->setType(GUTextBox::GUTEXTBOX_TYPE_NORMAL);
	GUT_stat->colors.normal.fill = WHITE;
	GUT_stat->colors.normal.border = BLUE;


	DrawThreadId = sceKernelCreateThread("DrawThread", DrawThread, 0x22, 0x10000, PSP_THREAD_ATTR_USER, NULL);
	if(DrawThreadId < 0)
	{
		pspDebugScreenPrintf("error: failed to create draw thread\n");
		sceKernelSleepThreadCB();
	}
	sceKernelStartThread(DrawThreadId, 0, NULL);

	RenderThreadId = sceKernelCreateThread("RenderThread", RenderThread, 0x18, 0x10000, PSP_THREAD_ATTR_USER, NULL);
	if(RenderThreadId < 0)
	{
		GUT_stat->setValue("�����_�[�X���b�h�̐����Ɏ��s���܂���");
		sceKernelSleepThreadCB();
	}
	sceKernelStartThread(RenderThreadId, 0, NULL);
	GUT_stat->setValue("�����_�[�X���b�h�̐����ɐ���");

	NetThreadId = sceKernelCreateThread("NetThread", NetThread, 0x20, 0x10000, PSP_THREAD_ATTR_USER, NULL);
	if(NetThreadId < 0)
	{
		GUT_stat->setValue("�ʐM�X���b�h�̐����Ɏ��s���܂���");
		sceKernelSleepThreadCB();
	}
	sceKernelStartThread(NetThreadId, 0, NULL);
	GUT_stat->setValue("�ʐM�X���b�h�̐����ɐ���");


	memset(&currpad, 0, sizeof(SceCtrlData));
	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
	while(1)
	{
		sceCtrlReadBufferPositive(&currpad, 1);
	}
	sceKernelSleepThreadCB();
	return 0;
}
