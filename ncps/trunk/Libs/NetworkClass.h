#include <sys/socket.h>
#include <netinet/in.h> 

#include <openssl/crypto.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>

#include <string>
#include <vector>

#ifndef _XMLCLSDECL
#define _XMLCLSDECL
class XMLSocket
{
private:
	int sock;
	int semaid;
	struct sockaddr_in server;
	struct hostent *host;
	struct in_addr hostip;


	int tmpport;
	char *tmphost;
	char *tmpobject;
	unsigned int intc;
	
public:
	bool setHost(const char *hostname);
	bool setPort(int port);
	bool open();
	bool close();
	bool send(char *buftosend);
	int recv(char *buftosave, int max_len);
	int recvEx(std::vector<std::string> *data);

	XMLSocket();
	~XMLSocket();
};
#endif

#ifndef _HTTPCLSDECL
#define _HTTPCLSDECL
class HTTPSocket
{
private:
	int sock;
	struct sockaddr_in server;
	struct in_addr hostip;

	char *tmphost;
	char *tmpobject;
	char *tmpmethod;
	char *tmpprop;


public:
	bool setHost(const char *hostname);
	bool setObject(const char *object);
	bool setRequestMethod(const char *method);
	bool setRequestProperty(const char *prop);
	bool open();
	bool close();
	bool send(char *buftosend);
	int recv(char *buftosave);

	HTTPSocket();
	~HTTPSocket();
};
#endif


#ifndef _HTTPSCLSDECL
#define _HTTPSCLSDECL
class HTTPSSocket
{
private:
	int sock;
	struct sockaddr_in server;
	struct in_addr hostip;

	char *tmphost;
	char *tmpobject;
	char *tmpmethod;
	char *tmpprop;

	SSL_CTX *ctx;
	SSL *ssl;

public:
	bool setHost(const char *hostname);
	bool setObject(const char *object);
	bool setRequestMethod(const char *method);
	bool setRequestProperty(const char *prop);
	bool init();
	bool open();
	bool close();
	bool send(char *buftosend);
	int recv(char *buftosave);

	HTTPSSocket();
	~HTTPSSocket();
};
#endif



int GetXMLTagAttribute(char *buf, const char *tagname, const char *attr, char *string);
int GetXMLTagNode(char *buf, const char *tagname, char *string);