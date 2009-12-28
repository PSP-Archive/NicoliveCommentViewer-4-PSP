#include <pspkernel.h>
#include <pspctrl.h>
#include <psputility.h>
#include <pspgu.h>
#include <pspdisplay.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "dialogs.h"
#include "gu.h"
#include "net.h"

#include "Libs/intraFont/intraFont.h"
#include "Libs/GUDraw.h"
#include "Libs/GUListView.h"


void LoginDialog(char *mail_init, char *pass_init, char *mail, char *pass)
{
	char trace_buf[64];
	FILE *fp;

	GUTextBox *GUT_mail = new GUTextBox;
	GUTextBox *GUT_pass = new GUTextBox;
	GUButton *GUB_login = new GUButton;

	GUT_mail->setType(GUTextBox::GUTEXTBOX_TYPE_EDITABLE);
	GUT_mail->setState(GUTextBox::GUTEXTBOX_STATE_NORMAL);
	GUT_mail->setStyle(GUTextBox::GUTEXTBOX_STYLE_NOSCROLL);

	GUT_mail->setEditTitle("メールアドレス");
	GUT_mail->setEditType(
		PSP_UTILITY_OSK_INPUTTYPE_LATIN_DIGIT
		| PSP_UTILITY_OSK_INPUTTYPE_LATIN_LOWERCASE
		| PSP_UTILITY_OSK_INPUTTYPE_LATIN_UPPERCASE
		);
	if(mail_init != NULL)
	{
		GUT_mail->setValue(mail_init);
	}


	GUT_pass->setType(GUTextBox::GUTEXTBOX_TYPE_EDITABLE);
	GUT_pass->setState(GUTextBox::GUTEXTBOX_STATE_NORMAL);
	GUT_pass->setStyle(GUTextBox::GUTEXTBOX_STYLE_NOSCROLL);

	GUT_pass->setEditTitle("パスワード");
	GUT_pass->setEditType(
		PSP_UTILITY_OSK_INPUTTYPE_LATIN_DIGIT
		| PSP_UTILITY_OSK_INPUTTYPE_LATIN_LOWERCASE
		| PSP_UTILITY_OSK_INPUTTYPE_LATIN_UPPERCASE
		);
	if(pass_init != NULL)
	{
		GUT_pass->setValue(pass_init);
	}

	GUB_login->setState(GUButton::GUBUTTON_STATE_NORMAL);
	GUB_login->setValue("ログイン");


	unsigned int cursor = 0;

	SceCtrlData oldpad;
	memset(&oldpad, 0, sizeof(SceCtrlData));
	while(1)
	{	
		if(!(oldpad.Buttons & PSP_CTRL_UP)
			&& (currpad.Buttons & PSP_CTRL_UP))
		{
			if(cursor > 0)
			{
				cursor--;
			}
		}
		else if(!(oldpad.Buttons & PSP_CTRL_DOWN)
			&& (currpad.Buttons & PSP_CTRL_DOWN))
		{
			if(cursor < 2)
			{
				cursor++;
			}
		}

		GUT_mail->setState(GUTextBox::GUTEXTBOX_STATE_NORMAL);
		GUT_pass->setState(GUTextBox::GUTEXTBOX_STATE_NORMAL);
		GUB_login->setState(GUButton::GUBUTTON_STATE_NORMAL);

		switch(cursor)
		{
			case 0:
				GUT_mail->setState(GUTextBox::GUTEXTBOX_STATE_SELECTED);
				break;

			case 1:
				GUT_pass->setState(GUTextBox::GUTEXTBOX_STATE_SELECTED);
				break;

			case 2:
				GUB_login->setState(GUButton::GUBUTTON_STATE_SELECTED);
				break;

			default:
				break;
		}

/*		if(GUT_mail->Render(&currpad) == GUTextBox::GUTEXTBOX_RESULT_CHANGED)
		{
			continue;
		}
		if(GUT_pass->Render(&currpad) == GUTextBox::GUTEXTBOX_RESULT_CHANGED)
		{
			continue;
		}
*/		GUB_login->Render(&currpad);

		if(GUB_login->getState() == GUButton::GUBUTTON_STATE_PUSHED)
		{
			break;
		}

		GUSTART
		DrawSquare(0, 0, 480, 272, RGBA(128, 128, 128, 160), true);
		GUT_mail->Draw(100, 50, 280);
		GUT_pass->Draw(100, 150, 280);
		GUB_login->Draw((480-GUB_login->getWidth())/2, 200);

		intraFontPrintf(jpn0, 20, 20, "GUT_mail:%s", GUT_mail->getValue());
		intraFontPrintf(jpn0, 20, 40, "GUT_pass:%s", GUT_pass->getValue());
		GUFINISH
		GUSYNC
		GUFLIP

		oldpad = currpad;
		sceKernelDelayThread(50 * 1000);
	}

	mail = (char *)malloc(strlen(GUT_mail->getValue())+1);
	pass = (char *)malloc(strlen(GUT_pass->getValue())+1);

	strcpy(mail, GUT_mail->getValue());
	strcpy(pass, GUT_pass->getValue());
	
	delete GUT_mail;
	delete GUT_pass;
	delete GUB_login;
}
