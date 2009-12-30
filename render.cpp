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
	// 試験的にデータを追加してみるー
	for(int q=1; q<80; q++)
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


	SceCtrlData oldpad;
	memset(&oldpad, 0, sizeof(SceCtrlData));
	while(1)
	{
		ret = menubar->Render(&currpad);

		if(ret != 0)
		{
			switch(ret)
			{
				case 0: // 「APに接続」
					if(!thData.try_netconnect)
					{
						thData.try_netconnect = true;
					}
					break;

				case 1: // 「ログイン」
					if(!thData.try_login)
					{
						thData.try_login = true;
					}
					break;

				case 2: // 「番組に接続」
					if(!thData.try_liveconnect)
					{
						thData.try_liveconnect = true;
					}
					break;

				case 10: // 「ログを消去」
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
