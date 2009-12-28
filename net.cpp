#include <pspkernel.h>
#include <pspctrl.h>
#include <psputility.h>
#include <pspgu.h>
#include <pspdisplay.h>

#include <unistd.h>
#include <pspnet.h>
#include <pspnet_apctl.h>

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


ConData conData;

int NetThread(SceSize args, void *argp)
{
	int ret = 0;

	conData.net_connected = false;
	conData.live_connected = false;
	conData.gotcookie = false;
	NicoAPIStart();


	GUT_stat->setValue("未接続");
	while(1)
	{
		if(!(oldpad.Buttons & PSP_CTRL_START)
			&& (currpad.Buttons & PSP_CTRL_START))
		{
			if(userData.mail != NULL && userData.pass != NULL)
			{
				if(!conData.net_connected)
				{
					GUT_stat->setValue("接続開始");
					if(ConnectToAccessPoint(1) == 0)
					{
						conData.net_connected = true;
					}
					else
					{
						// 接続失敗
						GUT_stat->setValue("接続に失敗しました");
						continue;
					}
				}
				FREE(userData.user_session);
				ret = GetUserSession(userData.mail, userData.pass, userData.user_session);
				if(ret == 0)
				{
					GUT_stat->setValue("ログインに成功しました");
				}
				else
				{
				//ログイン失敗
					if(ret == 1)
					{
						GUT_stat->setValue("メールアドレスまたはパスワードが違います");
					}

					if(ret == 2)
					{
						GUT_stat->setValue("アカウントがロックされています");
					}

					continue;
				}

				conData.gotcookie = true;
			}
		}
	
		/*
		//その放送のアドレス、ポート、スレッド番号を取得
		ret = GetPlayerStatus(livedata.id, usersession, &livedata);

		char request[128];
		char buf[4096];

		//最新50件のコメントを取得するように設定
		sprintf(request,
			"<thread thread=\"%d\" version=\"20061206\" res_from=\"-%d\" />"
			, livedata.thread, 50);

		XMLSocket xml;
		xml.setPort(livedata.port);
		xml.setHost(livedata.addr);

		xml.send(request);

		xml.recv(buf, 4000);
		*/

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

	//指定されたプロファイルで接続
	err = sceNetApctlConnect(configid);
	if (err != 0)
	{
		sprintf(stat, "接続エラー: sceNetApctlConnect returns %08X", err);
		return 0;
	}

	int count = 0;
	while(1)
	{
		if(count > 200000)
		{
			GUT_stat->setValue("接続がタイムアウトしました");
			return -1;
		}
		err = sceNetApctlGetState(&state);
		if (err != 0)
		{
			sprintf(stat, "接続エラー: sceNetApctlGetState returns %08X", err);
			GUT_stat->setValue(stat);
			return 1;
		}
		if (state > laststate)
		{
			if(state == 2)
			{
				GUT_stat->setValue("アクセスポイントに接続中...");
			}
			else if(state == 3)
			{
				GUT_stat->setValue("IPアドレスを取得中...");
			}
			
			laststate = state;
		}
		if(state == 4)
		{
			GUT_stat->setValue("接続完了");
			break;  //接続済み
		}

		count++;
		sceKernelDelayThread(50*1000);
	}

	return 0;
}


