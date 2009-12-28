
typedef struct _ConData
{
	bool net_connected;
	bool live_connected;
	bool gotcookie;
}ConData;


int NetThread(SceSize args, void *argp);

int ConnectToAccessPoint(int configid);


