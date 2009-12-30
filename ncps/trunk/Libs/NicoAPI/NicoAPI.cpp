//Warning: this library depends on 'NetworkClass'.

#include <stdlib.h>
#include <string.h>

#include "../NetworkClass.h"
#include "../NicoAPI.h"
#include "../intraFont/intraFont.h"

#include "../../types.h"


#define tostr(n) tostr_(n)
#define tostr_(n) #n
#define TRACEF {sprintf(trace_buf, "TRACE: (%s:%s)\r\n", __FILE__, tostr(__LINE__));fp=fopen("ms0:/debug.txt", "a");fputs(trace_buf, fp);fclose(fp);}


bool strrep(char *buf, const char *before, const char *after)
{
	const char *find = before;	//string to find
	const char *rep = after;		//string to replace
	int shift =(int)(strlen(rep) - strlen(find));  //amount to shift

	//search
	char *found;
	found = strstr(buf, find);

	if(found != NULL) 
	{
		//shift the string
		if (shift > 0)
		{
			memmove(found+shift, found, strlen(found)+1);	//righter
		} 
		else if(shift < 0)
		{
			memmove(found, found-shift, strlen(found)+shift+1);	//lefter
		}
		
		//replace
		memmove(found, rep, strlen(rep));
		return 0;
	}

	return 1;
}

void NicoAPIStart(void)
{
	userData.mail = (char *)malloc(128);
	userData.pass = (char *)malloc(128);
	userData.user_session = NULL;

	liveData.id = 0;
	liveData.watch_count = 0;
	liveData.comment_count = 0;
	liveData.start_time = 0;
	liveData.room_seetno = 0;
	liveData.port = 0;
	liveData.thread = 0;
}

void NicoAPIEnd(void)
{
	FREE(userData.mail);
	FREE(userData.pass);
	FREE(userData.user_session);
}

int GetUserSession(char *mail, char *password, char *usersession)
{
	//============================================
	//https://secure.nicovideo.jp/secure/login?site=niconico
	//============================================
	char *param = NULL;
	char *found = NULL;

	char header[4096];
	char buf[2048];


	param = (char *)malloc(strlen(mail) + strlen(password) + 32);
	sprintf(param, "mail=%s&password=%s", mail, password);

	//TODO: percent-encode
	strrep(param, "@", "%40");

	sprintf(header, 
		"Host: secure.nicovideo.jp\r\n"
		"Content-Type: application/x-www-form-urlencoded\r\n"
		"Content-Length: %d"
		, strlen(param));

	HTTPSSocket https;
	https.init();
	https.setHost("secure.nicovideo.jp");
	https.setObject("/secure/login?site=niconico");
	https.setRequestMethod("POST");
	https.setRequestProperty(header);
	https.open();
	https.send(param);
	https.recv(buf);
	https.close();

	FREE(param);

	//error check
	found = strstr(buf, "login_form?message=cant_login");
	if(found != NULL)
	{
		return 1;	//ID or PASS is wrong
	}
	found = strstr(buf, "login_lock_release");
	if(found != NULL)
	{
		return 2;	//account locked
	}
	

	//login succeeded
	found = strstr(buf, "Set-Cookie: user_session=u");

	//copy user_session
	for(int i=12; *(found+i)!='\r'; i++)
	{
		usersession[i-12] = *(found+i);
		usersession[i-12+1] = '\0';
	}

	//default
	return 0;
}

int GetLiveAntenna(char *mail, char *password, char *ticket)
{
	//============================================
	//https://secure.nicovideo.jp/secure/login?site=nicolive_antenna
	//============================================

	char *param = NULL;
	char *found = NULL;

	char header[4096];
	char buf[65536];

	param = (char *)malloc(strlen(mail) + strlen(password) + 1024);
	sprintf(param, "mail=%s&password=%s", mail, password);

	//TODO: percent-encode
	strrep(param, "@", "%40");

	sprintf(header, 
		"Host: secure.nicovideo.jp\r\n"
		"Content-Type: application/x-www-form-urlencoded\r\n"
		"Content-Length: %d"
		, strlen(param));

	HTTPSSocket https;
	https.init();
	https.setHost("secure.nicovideo.jp");
	https.setObject("/secure/login?site=nicolive_antenna");
	https.setRequestMethod("POST");
	https.setRequestProperty(header);
	https.open();
	https.send(param);
	https.recv(buf);
	https.recv(buf);
	https.close();

	FREE(param);

	//error check
	found = strstr(buf, "status=\"fail\"");
	if(found == NULL)
	{
		//login succeeded
		GetXMLTagNode(buf, "ticket", ticket);
	}
	else
	{
		return -1;	//failed
	}

	//default
	return 0;
}


int GetPlayerStatus(int liveid, char *usersession, LiveData *livedata)
{
	//============================================
	//http://live.nicovideo.jp/api/getplayerstatus?v=lvXXXXXXX
	//============================================
	char buf[4096];
	char temp[1024];
	char header[128];
	sprintf(header,
		"Host: live.nicovideo.jp\r\n"
		"Cookie: %s"
		, usersession);

	char object[128];
	sprintf(object,
		"/api/getplayerstatus?v=lv%d"
		, liveid);

	HTTPSocket http;
	http.setHost("live.nicovideo.jp");
	http.setObject(object);
	http.setRequestMethod("GET");
	http.setRequestProperty(header);
	http.open();
	http.send(NULL);
	http.recv(buf);
	http.close();

	char *found = NULL;
	found = strstr(buf, "status=\"fail\"");
	if(found == NULL)
	{
		GetXMLTagNode(buf, "addr",								livedata->addr);
		GetXMLTagNode(buf, "port", temp);						livedata->port = atoi(temp);
		GetXMLTagNode(buf, "thread", temp);						livedata->thread = atoi(temp);
	}
	else
	{
		found = strstr(buf, "<code>closed</code>");
		if(found != NULL)
		{
			return 1;
		}
		found = strstr(buf, "<code>notlogin</code>");
		if(found != NULL)
		{
			return 2;
		}

		//unknown error
		return -1;
	}

	//default
	return 0;
}

int Heartbeat(int liveid, char *usersession, LiveData *livedata)
{
	//============================================
	//http://live.nicovideo.jp/api/heartbeat?v=lvXXXXXXX
	//============================================
	char buf[4096];
	char temp[1024];
	char header[128];
	sprintf(header,
		"Host: live.nicovideo.jp\r\n"
		"Cookie: %s"
		, usersession);

	char object[128];
	sprintf(object,
		"/api/heartbeat?v=lv%d"
		, liveid);

	HTTPSocket http;
	http.setHost("live.nicovideo.jp");
	http.setObject(object);
	http.setRequestMethod("GET");
	http.setRequestProperty(header);
	http.open();
	http.send(NULL);
	http.recv(buf);
	http.close();

	char *found = NULL;
	found = strstr(buf, "status=\"fail\"");
	if(found == NULL)
	{
		//number of visitors
		GetXMLTagNode(buf, "watchCount", temp);
		livedata->watch_count = atoi(temp);

		//number of comments
		GetXMLTagNode(buf, "commentCount", temp);					
		livedata->comment_count = atoi(temp);
	}
	else
	{
		found = strstr(buf, "<code>NOTFOUND_STREAM</code>");
		if(found != NULL)
		{
			return 1;	//live is closed or not found
		}
		found = strstr(buf, "<code>NOTLOGIN</code>");
		if(found != NULL)
		{
			return 2;	//not logged in
		}

		//unknown error
		return -1;
	}

	//default
	return 0;
}
