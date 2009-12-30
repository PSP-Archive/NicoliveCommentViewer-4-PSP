#include <pspkernel.h>
#include <pspctrl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Libs/GUListView.h"
#include "Libs/GUMenu.h"

#include "types.h"
#include "render.h"

int RenderThread(SceSize args, void *argp)
{
	char temp[128];
	// �����I�Ƀf�[�^��ǉ����Ă݂�[
	for(int q=1; q<80; q++)
	{
		// �R����
		sprintf(temp, "%d", q);
		lview->getColumn(0)->addItem(temp);
		
		// �R�����g���e
		sprintf(temp, "(���O�ցO)�߷�߷");
		lview->getColumn(1)->addItem(temp);

		// ID/�R�e�n��
		sprintf(temp, "%p", (void *)(lview->getColumn(2)->itemlist_current));
		lview->getColumn(2)->addItem(temp);	
	}
	lview->Render(NULL, NULL);


	SceCtrlData oldpad;
	memset(&oldpad, 0, sizeof(SceCtrlData));
	while(1)
	{
		lview->Render(&currpad, NULL);



		oldpad = currpad;
		sceKernelDelayThread(50 * 1000);
	}

	return 0;
}
