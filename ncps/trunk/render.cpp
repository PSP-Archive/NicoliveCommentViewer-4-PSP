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
	int ret = 0;
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
		ret = menubar->Render(&currpad);

		if(ret != 0)
		{
			switch(ret)
			{
				case 0: // �uAP�ɐڑ��v
					if(!thData.try_netconnect)
					{
						thData.try_netconnect = true;
					}
					break;

				case 1: // �u���O�C���v
					if(!thData.try_login)
					{
						thData.try_login = true;
					}
					break;

				case 2: // �u�ԑg�ɐڑ��v
					if(!thData.try_liveconnect)
					{
						thData.try_liveconnect = true;
					}
					break;

				case 10: // �u���O�������v
					for(int i=0; i<lview->_column_num; i++)
					{
						lview->getColumn(i)->flushItem();
					}
					lview->Render(NULL, (unsigned int *)0);
					break;


				default:
					break;
			}
		}


		if(menubar->getState() == GUMenu::GUMENU_STATE_HIDE)
		{
			lview->Render(&currpad, NULL);
		}

		oldpad = currpad;
		sceKernelDelayThread(50 * 1000);
	}

	return 0;
}
