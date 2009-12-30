#include <pspkernel.h>
#include <pspsdk.h>
#include <pspctrl.h>
#include <psputility.h>
#include <pspgu.h>
#include <pspdisplay.h>

#include <pspnet.h>
#include <pspnet_apctl.h>
#include <pspnet_inet.h>
#include <pspnet_resolver.h>

#include <time.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <errno.h>
#include <netdb.h>

#include "types.h"
#include "Libs/intraFont/intraFont.h"

#include "Libs/GUDraw.h"
#include "Libs/GUDiag.h"

#include "Libs/NetworkClass.h"
#include "Libs/NicoAPI.h"

#include "gu.h"
#include "net.h"

#define tostr(n) tostr_(n)
#define tostr_(n) #n
#define TRACEF {sprintf(trace_buf, "TRACE: (%s:%s)\r\n", __FILE__, tostr(__LINE__));fp=fopen("ms0:/debug.txt", "a");fputs(trace_buf, fp);fclose(fp);}


//<thread thread="1000000000" version="20061206" res_from="-XX" />
//http://live.nicovideo.jp/api/getplayerstatus?v=lvXXXXXXX
int NetThread(SceSize args, void *argp)
{
	int ret = 0;
	char temp[4096];

	conData.net_connected = false;
	conData.live_connected = false;
	conData.gotcookie = false;

	thData.try_netconnect = false;
	thData.try_login = false;
	thData.try_liveconnect = false;

	NicoAPIStart();
	userData.mail = (char *)malloc(128);
	userData.pass = (char *)malloc(128);
	strcpy(userData.mail, "unko_king@live.jp");
	strcpy(userData.pass, "unkoking");

	//�l�b�g���[�N�����������
	ret = pspSdkInetInit();
	if(ret != 0)
	{
		char temp[64];
		sprintf(temp, "���W���[���̏������Ɏ��s���܂���(%08X)", ret);
		GUT_stat->setValue(temp);
		sceKernelSleepThread();
	}

	GUT_stat->setValue("���ڑ�");

	SceCtrlData oldpad;
	memset(&oldpad, 0, sizeof(SceCtrlData));
	while(1)
	{
		if(thData.try_netconnect)
		{
			thData.try_netconnect = false;

			GUT_stat->setValue("�ڑ��J�n");
			if(ConnectToAccessPoint(1) == 0)
			{
				conData.net_connected = true;
			}
			else
			{
				GUT_stat->setValue("�ڑ��Ɏ��s���܂���");
			}
		}

		if(thData.try_login)
		{	
			thData.try_login = false;

			if(conData.net_connected)
			{				
				if(userData.mail != NULL && userData.pass != NULL)
				{
					GUT_stat->setValue("�j�R�j�R����Ƀ��O�C�����܂�...");
					FREE(userData.user_session);

					userData.user_session = (char *)malloc(128);
					ret = GetUserSession("unko_king@live.jp", "unkoking", userData.user_session);
					if(ret == 0)
					{
						conData.gotcookie = true;
						GUT_stat->setValue("���O�C���ɐ������܂���");
					}
					else
					{
					//���O�C�����s
						conData.gotcookie = false;
						if(ret == 1)
						{
							GUT_stat->setValue("���[���A�h���X�܂��̓p�X���[�h���Ⴂ�܂�");
						}
						else if(ret == 2)
						{
							GUT_stat->setValue("�A�J�E���g�����b�N����Ă��܂�");
						}
						else
						{
							GUT_stat->setValue("���O�C���ł��܂���ł����B�s���ȃG���[");
						}
					}
				}
			}
			else
			{
				GUT_stat->setValue("�ڑ�����Ă��܂���");
			}
		}


			if(thData.try_liveconnect)
			{
				if(conData.net_connected && conData.gotcookie)
				{
					sceKernelSuspendThread(RenderThreadId);
					sceKernelSuspendThread(DrawThreadId);
					ret = GetOSKText(temp, 1024, PSP_UTILITY_OSK_INPUTTYPE_LATIN_DIGIT, "�ԑgID", "");
					sceKernelResumeThread(DrawThreadId);
					sceKernelResumeThread(RenderThreadId);
					liveData.id = atoi(temp);
					liveData.port = 0;
					liveData.thread = 0;
					ret = GetPlayerStatus(liveData.id, userData.user_session, &liveData);
				
					if(ret != 0 || liveData.port == 0 || liveData.thread == 0)
					{
						GUT_stat->setValue("�ԑg���̎擾�Ɏ��s���܂���");
						conData.live_connected = false;
						if(ret == 1)
						{
							sprintf(temp, "[lv%d]�ԑg���I�����Ă��܂�", liveData.id);
							GUT_stat->setValue(temp);
						}
						else if(ret == 2)
						{
							GUT_stat->setValue("���O�C�����Ă��܂���");
						}
						else
						{
							GUT_stat->setValue("�s���ȃG���[�B�j�R�j�R�������̎d�l�ύX�H");
						}
					}
					else
					{
						GUT_stat->setValue("�ԑg���̎擾�ɐ������܂����A��M�X���b�h�͖�����");
						//if(RecvThreadId >= 0)
						//{
						//	sceKernelDeleteThread(RecvThreadId);
						//}
						//RecvThreadId = sceKernelCreateThread("RecvThread", RecvThread, 0x22, 0x10000, PSP_THREAD_ATTR_USER, NULL);
						//if(RecvThreadId < 0)
						//{
						//	conData.live_connected = false;
						//	GUT_stat->setValue("��M�X���b�h�̍쐬�Ɏ��s���܂���");
						//}
						//else
						//{
						//	conData.live_connected = true;
						//	sceKernelStartThread(RecvThreadId, 0, NULL);
						//	GUT_stat->setValue("��M�X���b�h�쐬����");
						//}
					}
				}
				else
				{
					if(!conData.net_connected)
					{
						GUT_stat->setValue("�A�N�Z�X�|�C���g�ɐڑ�����Ă��܂���");
					}
					else if(conData.net_connected && !conData.gotcookie)
					{
						GUT_stat->setValue("�j�R�j�R����Ƀ��O�C�����Ă��܂���");
					}
				}
			}


		oldpad = currpad;
		sceKernelDelayThread(50 * 1000);
	}
	NicoAPIEnd();
	return 0;
}


int ConnectToAccessPoint(int configid)
{
	int err;
	int state = 0;
	int laststate = -1;
	char stat[128];


	/*
	// ���ɐڑ��ς݂̏ꍇ�͈��ؒf����
	err = sceNetApctlGetState(&state);
	if(err == 0)
	{
		if(state != 0)
		{
			err = sceNetApctlDisconnect();
			if(err != 0)
			{
				GUT_stat->setValue("�A�N�Z�X�|�C���g����ؒf�ł��܂���ł���");
				return -1;
			}
		}
	}
	else
	{
		GUT_stat->setValue("���W���[���̏�Ԃ��擾�ł��܂���ł���");
		return -1;
	}
	*/


	//�w�肳�ꂽ�v���t�@�C���Őڑ�
	err = sceNetApctlConnect(configid);
	if(err != 0)
	{
		sprintf(stat, "�ڑ��G���[: sceNetApctlConnect returns %08X", err);
		GUT_stat->setValue(stat);
		return -1;
	}

	unsigned int start_time = (unsigned int)time(NULL);
	unsigned int now_time = 0;

	SceCtrlData oldpad = currpad;
	while(1)
	{
	/*	now_time = (unsigned int)time(NULL);
		if((now_time - start_time) > 5)
		{
			GUT_stat->setValue("�ڑ����^�C���A�E�g���܂���");
			return -1;
		}
*/
		if(!(oldpad.Buttons & PSP_CTRL_START)
			&& (currpad.Buttons & PSP_CTRL_START))
		{
			// �ڑ����~
			GUT_stat->setValue("�ڑ������~����܂���");
			sceNetApctlDisconnect();
			return -1;
		}

		err = sceNetApctlGetState(&state);
		if(err != 0)
		{
			sprintf(stat, "�ڑ��G���[: sceNetApctlGetState returns %08X", err);
			GUT_stat->setValue(stat);
			return -1;
		}
		if(state > laststate)
		{
			if(state == 2)
			{
				GUT_stat->setValue("�A�N�Z�X�|�C���g�ɐڑ���...");
			}
			else if(state == 3)
			{
				start_time = (unsigned int)time(NULL);
				GUT_stat->setValue("IP�A�h���X���擾��...");
			}		
			laststate = state;
		}
		if(state == 4)
		{
			GUT_stat->setValue("�ڑ�����");
			break;  //�ڑ��ς�
		}

		oldpad = currpad;
		sceKernelDelayThread(50 * 1000);
	}

	return 0;
}


