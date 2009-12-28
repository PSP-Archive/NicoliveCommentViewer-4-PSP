#ifndef _NICOAPIDECL
#define _NICOAPIDECL

#define FREE(_Memory) if(_Memory!=NULL){free(_Memory);_Memory=NULL;}

typedef struct _UserData
{
	char *mail;
	char *pass;

	char *user_session;
}UserData;

typedef struct _LiveData
{
	int		id;
	int		watch_count;
	int		comment_count;
	
	int		start_time;
	char	room_label[16]; 
	int		room_seetno;

	char	addr[64];
	int		port;
	int		thread;
}LiveData;

extern UserData userData;
extern LiveData liveData;

void NicoAPIStart(void);
void NicoAPIEnd(void);


/*
 *		GetUserSession
 *	https://secure.nicovideo.jp/secure/login?site=niconico
 *
 *@param mail: mail address
 *@param password: password
 *@param usersession: pointer to a buf to save user_session in
 *
 *@return 0: success
 *@return 1: ID or PASS is wrong
 *@return 2: account is locked(too many re-trys?)
 *
 */
int GetUserSession(char *mail, char *password, char *usersession);


/*
 *		GetLiveAntenna
 *	https://secure.nicovideo.jp/secure/login?site=niconico
 *
 *@param mail: mail address
 *@param password: password
 *@param ticket: pointer to a buf to save ticket in
 *
 *@return 0: success
 *@return -1: failed	//there are more info, but it is utf-8 encoded so...
 *
 */
int GetLiveAntenna(char *mail, char *password, char *ticket);

/*
 *		GetPlayerStatus
 *	http://live.nicovideo.jp/api/getplayerstatus?v=lvXXXXXXX
 *
 *@param liveid: live's id (lvXXXXXX)
 *@param usersession: string given as Cookie
 *@param livedata: structure to save addr and port
 *
 *@return 0: success
 *@return -1: unknown error
 *@return 1: live is closed
 *@return 2: not logged in
 *
 */
int GetPlayerStatus(int liveid, char *usersession, LiveData *livedata);

/*
 *		Heartbeat
 *	http://live.nicovideo.jp/api/heartbeat?v=lvXXXXXXX
 *
 *@param liveid: live's id (lvXXXXXX)
 *@param usersession: string given as Cookie
 *@param livedata: structure to save addr and port
 *
 *@return 0: success
 *@return 1: live is closed or not found
 *@return 2: not logged in
 *
 */
int Heartbeat(int liveid, char *usersession, LiveData *livedata);

#endif