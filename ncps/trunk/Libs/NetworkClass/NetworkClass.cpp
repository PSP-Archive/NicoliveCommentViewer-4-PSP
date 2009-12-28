#include <pspkernel.h>
#include <unistd.h>
#include <pspnet.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>

#include "../NetworkClass.h"
#define INVALID_SOCKET (~0)
#define FREE(_Memory) if(_Memory!=NULL){free(_Memory);_Memory=NULL;}

XMLSocket::XMLSocket()
{
	sock = INVALID_SOCKET;
	tmpport = -1;
	tmphost = NULL;
	tmpobject = NULL;
}

XMLSocket::~XMLSocket()
{
	FREE(tmphost);
	FREE(tmpobject);
}


bool XMLSocket::setHost(const char *hostname)
{
	if(hostname == NULL)
	{
		return 1;
	}

	FREE(tmphost);

	tmphost = (char *)malloc(strlen(hostname) + 16);
	strcpy(tmphost, hostname);
	
	//default
	return 0;
}

bool XMLSocket::setPort(int port)
{
	if(port < 0 || port > 65535)
	{
		return 1;
	}

	tmpport = port;

	//default
	return 0;
}


bool XMLSocket::open()
{
	if(	tmphost == NULL ||
		tmpport < 0 || 
		tmpport > 65535)
	{
		return 1;
	}

	int ret = 0;

	//if already connected,
	//shutdown the connection and
	//overwrite with new connection
	if(sock != INVALID_SOCKET)
	{
		shutdown(sock, 2);
		::close(sock);
		sock = INVALID_SOCKET;
	}

	
	server.sin_family = AF_INET;
	server.sin_port = htons(tmpport);

	host = gethostbyname(tmphost);
	memcpy(&hostip, *(host->h_addr_list), sizeof(struct in_addr));
	server.sin_addr = hostip;
	

	//TCP
	ret = socket(AF_INET, SOCK_STREAM, 0);
	if(ret < 0)
	{
		return 1;
	}
	else
	{
		sock = ret;
	}

	ret = connect(sock, (struct sockaddr *)&server, sizeof(server));
	if(ret != 0)
	{
		shutdown(sock, 2);
		::close(sock);
		sock = INVALID_SOCKET;

		return 1;
	}


	//default
	return 0;		
}

bool XMLSocket::close()
{
	if(sock == INVALID_SOCKET)
	{
		return 1;	//connection is closed
	}

	if(sock != INVALID_SOCKET)
	{
		shutdown(sock, 2);
		::close(sock);
		sock = INVALID_SOCKET;
	}

	//default
	return 0;
}


bool XMLSocket::send(char *buftosend)
{
	if(	sock == INVALID_SOCKET || 
		buftosend == NULL)
	{
		return 1;
	}

	int ret = 0;


	ret = ::send(sock, buftosend, strlen(buftosend) + 1, 0);
	if(ret < 0)
	{
		return 1;
	}


	//default
	return 0;
}


int XMLSocket::recv(char *buftosave, int max_len)
{
	if(	sock == INVALID_SOCKET || 
		buftosave == NULL)
	{
		return 1;
	}


	int RecvLen = 0;
	int LineBufLen = 4096;
	int TotalBufLen = 0;

	char *LineBuf = (char *)malloc(LineBufLen);
	char *TotalBuf = (char *)malloc(LineBufLen);

	if(LineBuf == NULL || TotalBuf == NULL)
	{
		printf("No more memory...\n");
		return 1;
	}

	memset(LineBuf, '\0', LineBufLen);
	memset(TotalBuf, '\0', LineBufLen);


	int i = 0;
	while(1)
	{
		//for safety
		if(i > 1000)
		{
			break;
		}
		else
		{
			i++;
		}

		memset(LineBuf, '\0', LineBufLen);
		RecvLen = ::recv(sock, LineBuf, LineBufLen - 1, 0);
		if(RecvLen <= 0) break;

		
		//since the separator of
		//XMLSocket is NULL
		char *found = strstr(LineBuf, "\0");
		if(found != NULL)
		{
			//found
			TotalBufLen += RecvLen;

			if(TotalBufLen >= max_len)
			{
				//buf is greater than max_len
				break;
			}

			char *tmp = (char *)realloc(TotalBuf, TotalBufLen + 16);
			if(tmp != NULL)
			{
				TotalBuf = tmp;
			}
			else
			{
				free(tmp);

				free(LineBuf);
				free(TotalBuf);
				return 1;
			}
			strcat(TotalBuf, LineBuf);
			break;
		}
		else
		{
			//not found;continue to recv
			*(LineBuf + RecvLen) = '\0';
			TotalBufLen += RecvLen;

			if(TotalBufLen >= max_len)
			{
				//buf is greater than max_len
				break;
			}

			char *tmp = (char *)realloc(TotalBuf, TotalBufLen + 16);
			if(tmp != NULL)
			{
				TotalBuf = tmp;
			}
			else
			{
				free(tmp);

				free(LineBuf);
				free(TotalBuf);
				return 1;
			}

			strcat(TotalBuf, LineBuf);
		}
	}

	strcpy(buftosave, TotalBuf);
	free(LineBuf);
	free(TotalBuf);


	//default
	return TotalBufLen;
}

			
int XMLSocket::recvEx(std::vector<std::string> *data)
{
	if(	sock == INVALID_SOCKET || 
		data == NULL)
	{
		return 1;
	}


	int RecvLen = 0;
	int LineBufLen = 4096;
	int TotalBufLen = 0;

	char *LineBuf = NULL;

	std::string one_comment;
	int processed_size = 0;

	char *TempBuf = NULL;
	char *RemainBuf = NULL;
	char *ptr = NULL;

	bool connection_end = false;

	int i = 0;
	while(1)
	{
		//for safety
		if(i > 1000)
		{
			break;
		}
		else
		{
			i++;
		}

		LineBuf = (char *)malloc(LineBufLen);
		memset(LineBuf, '\0', LineBufLen);

		ptr = LineBuf;
		processed_size = 0;

		RecvLen = ::recv(sock, LineBuf, LineBufLen - 1, 0);
		if(RecvLen <= 0) break;
		

		for(int p=0; p<RecvLen; p++)
		{
			if(*(LineBuf + p) == '\n')
			{
				*(LineBuf + p) = '\0';
				RecvLen = p;

				connection_end = true;
				break;
			}
		}
		

		if(*(LineBuf + RecvLen) != '\0')
		{
			//data still continues to next recv
			*(LineBuf + RecvLen + 1) = '\0';

			while(1)
			{
				processed_size += strlen(ptr) + 1;
				if(processed_size == RecvLen)
				{
					//finished processing, but still remaining data
					RemainBuf = (char *)malloc(strlen(ptr) + 1);
					strcpy(RemainBuf, ptr);

					break;
				}

				if(RemainBuf != NULL)
				{
					TempBuf = (char *)malloc(strlen(ptr) + strlen(RemainBuf) + 1);
					strcpy(TempBuf, RemainBuf);
					strcat(TempBuf, ptr);

					free(RemainBuf);
					RemainBuf = NULL;
				}
				else
				{
					TempBuf = (char *)malloc(strlen(ptr) + 1);	//include \0
					strcpy(TempBuf, ptr);
				}

				one_comment = TempBuf;
				// EnterCriticalSection(cs);
				data->push_back(one_comment);
				// LeaveCriticalSection(cs);
				free(TempBuf);
				TempBuf = NULL;

				ptr += strlen(ptr) + 1;
			}
		}
		else
		{
			while(1)
			{
				if(RemainBuf != NULL)
				{
					TempBuf = (char *)malloc(strlen(ptr) + strlen(RemainBuf) + 1);
					strcpy(TempBuf, RemainBuf);
					strcat(TempBuf, ptr);

					free(RemainBuf);
					RemainBuf = NULL;
				}
				else
				{
					TempBuf = (char *)malloc(strlen(ptr) + 1024);	//include \0
					strcpy(TempBuf, ptr);
				}

				one_comment = TempBuf;
				// EnterCriticalSection(cs);
				data->push_back(one_comment);
				// LeaveCriticalSection(cs);
				free(TempBuf);
				TempBuf = NULL;

				processed_size += strlen(ptr) + 1;
				if(processed_size >= RecvLen)
				{
					break;
				}

				ptr += strlen(ptr) + 1;
			}
		}

		TotalBufLen += processed_size;
		free(LineBuf);
		LineBuf = NULL;

		if(connection_end)
		{
			break;
		}
	}

	//default
	return TotalBufLen;
}



HTTPSocket::HTTPSocket()
{
	sock = INVALID_SOCKET;

	tmphost = NULL;
	tmpobject = NULL;
	tmpmethod = NULL;
	tmpprop = NULL;
}

HTTPSocket::~HTTPSocket()
{
	FREE(tmphost);
	FREE(tmpobject);
	FREE(tmpmethod);
	FREE(tmpprop);
}



bool HTTPSocket::setHost(const char *hostname)
{
	if(hostname == NULL)
	{
		return 1;
	}


	FREE(tmphost);
	tmphost = (char *)malloc(strlen(hostname) + 16);
	strcpy(tmphost, hostname);

	//default
	return 0;
}

bool HTTPSocket::setObject(const char *object)
{
	if(object == NULL)
	{
		return 1;
	}


	FREE(tmpobject);
	tmpobject = (char *)malloc(strlen(object) + 16);
	strcpy(tmpobject, object);

	//default
	return 0;
}

bool HTTPSocket::setRequestMethod(const char *method)
{
	if(method == NULL)
	{
		return 1;
	}


	FREE(tmpmethod);
	tmpmethod = (char *)malloc(strlen(method) + 16);
	strcpy(tmpmethod, method);

	//default
	return 0;
}

bool HTTPSocket::setRequestProperty(const char *prop)
{
	if(prop == NULL)
	{
		return 1;
	}


	FREE(tmpprop);
	tmpprop = (char *)malloc(strlen(prop) + 16);
	strcpy(tmpprop, prop);

	//default
	return 0;
}


bool HTTPSocket::open()
{
	if(tmphost == NULL)
	{
		return 1;
	}

	int ret = 0;

	//if already connected,
	//shutdown the connection and
	//overwrite with new connection
	if(sock != INVALID_SOCKET)
	{
		shutdown(sock, 2);
		::close(sock);
		sock = INVALID_SOCKET;
	}


	server.sin_family = AF_INET;
	server.sin_port = htons(80);

	struct hostent *host;

	host = gethostbyname(tmphost);
	if(host == NULL)
	{
		return 1;
	}
	memcpy(&hostip, *(host->h_addr_list), sizeof(struct in_addr));
	server.sin_addr = hostip;


	//TCP
	ret = socket(AF_INET, SOCK_STREAM, 0);
	if(ret < 0)
	{
		return 1;
	}
	else
	{
		sock = ret;
	}

	ret = connect(sock, (struct sockaddr *)&server, sizeof(server));
	if(ret != 0)
	{
		if(sock != INVALID_SOCKET)
		{
			shutdown(sock, 2);
			::close(sock);
			sock = INVALID_SOCKET;
		}

		return 1;
	}


	//default
	return 0;		
}

bool HTTPSocket::close()
{
	if(sock == INVALID_SOCKET)
	{
		return 1;	//connection is closed
	}

	if(sock != INVALID_SOCKET)
	{
		shutdown(sock, 2);
		::close(sock);
		sock = INVALID_SOCKET;
	}

	//default
	return 0;
}

bool HTTPSocket::send(char *buftosend)
{
	if(sock == INVALID_SOCKET || tmphost == NULL) 
	{
		return 1;
	}

	if(tmpmethod == NULL) 
	{
		tmpmethod = (char *)malloc(16);
		strcpy(tmpmethod, "GET");
	}

	char *tmprequest;

	//if 'buftosend' is NULL, usually in POST
	if(buftosend == NULL)
	{
		tmprequest = (char *)malloc(
			strlen(tmpmethod) +
			strlen(tmpobject) +
			strlen(tmpprop) +
			128);	

		sprintf(tmprequest,
			"%s "			//method
			"%s "			//object
			"HTTP/1.1\r\n"	//version
			"%s\r\n\r\n"	//properties
			, tmpmethod, tmpobject, tmpprop);
	}
	else
	{
		tmprequest = (char *)malloc(
			strlen(tmpmethod) +
			strlen(tmpobject) +
			strlen(tmpprop) +
			strlen(buftosend) +
			128);	

		sprintf(tmprequest,
			"%s "			//method
			"%s "			//object
			"HTTP/1.1\r\n"	//version
			"%s\r\n\r\n"	//properties
			"%s\r\n"		//buf to send
			, tmpmethod, tmpobject, tmpprop, buftosend);
	}

	if(::send(sock, tmprequest, strlen(tmprequest), 0) < 0)
	{
		return 1;
	}

	FREE(tmprequest);

	//default
	return 0;
}

int HTTPSocket::recv(char *buftosave)
{
	if(	sock == INVALID_SOCKET || 
		buftosave == NULL)
	{
		return 1;
	}


	int RecvLen = 0;
	int LineBufLen = 4096;
	int TotalBufLen = 0;

	char *LineBuf = (char *)malloc(LineBufLen);
	char *TotalBuf = (char *)malloc(LineBufLen);

	if(LineBuf == NULL || TotalBuf == NULL)
	{
		printf("No more memory...\n");
		return 1;
	}

	memset(LineBuf, '\0', LineBufLen);
	memset(TotalBuf, '\0', LineBufLen);


	int i = 0;
	while(1)
	{
		//for safety
		if(i > 1000)
		{
			break;
		}
		else
		{
			i++;
		}

		memset(LineBuf, '\0', LineBufLen);
		RecvLen = ::recv(sock, LineBuf, LineBufLen - 1, 0);
		if(RecvLen == 0) break;

		
		//since the separator of
		//XMLSocket is NULL
		char *found = strstr(LineBuf, "\0");
		if(found != NULL)
		{
			//found
			TotalBufLen += RecvLen;
			char *tmp = (char *)realloc(TotalBuf, TotalBufLen + 16);
			if(tmp != NULL)
			{
				TotalBuf = tmp;
			}
			else
			{
				free(tmp);

				free(LineBuf);
				free(TotalBuf);
				return 1;
			}
			strcat(TotalBuf, LineBuf);
			break;
		}
		else
		{
			//not found;continue to recv
			*(LineBuf + RecvLen) = '\0';
			TotalBufLen += RecvLen;
			char *tmp = (char *)realloc(TotalBuf, TotalBufLen + 16);
			if(tmp != NULL)
			{
				TotalBuf = tmp;
			}
			else
			{
				free(tmp);

				free(LineBuf);
				free(TotalBuf);
				return 1;
			}

			strcat(TotalBuf, LineBuf);
		}
	}

	strcpy(buftosave, TotalBuf);
	free(LineBuf);
	free(TotalBuf);


	//default
	return TotalBufLen;
}




/***************************************************************
 *	HTTP'S' Socket
 ***************************************************************/

HTTPSSocket::HTTPSSocket()
{
	sock = INVALID_SOCKET;
	ssl = NULL;
	ctx = NULL;

	tmphost = NULL;
	tmpobject = NULL;
	tmpmethod = NULL;
	tmpprop = NULL;

	SSL_load_error_strings();
	SSL_library_init();
}


HTTPSSocket::~HTTPSSocket()
{
	FREE(tmphost);
	FREE(tmpobject);
	FREE(tmpmethod);
	FREE(tmpprop);

	if(ctx != NULL)
	{
		SSL_CTX_free(ctx);
		ctx = NULL;
	}
	ERR_free_strings();
}

//create a new instance
bool HTTPSSocket::init()
{
	ctx = SSL_CTX_new(SSLv3_client_method());
	if(ctx == NULL)
	{
		return 1;
	}

	return 0;
}

bool HTTPSSocket::setHost(const char *hostname)
{
	if(hostname == NULL)
	{
		return 1;
	}


	FREE(tmphost);
	tmphost = (char *)malloc(strlen(hostname) + 16);
	strcpy(tmphost, hostname);

	//default
	return 0;
}

bool HTTPSSocket::setObject(const char *object)
{
	if(object == NULL)
	{
		return 1;
	}


	FREE(tmpobject);
	tmpobject = (char *)malloc(strlen(object) + 16);
	strcpy(tmpobject, object);

	//default
	return 0;
}

bool HTTPSSocket::setRequestMethod(const char *method)
{
	if(method == NULL)
	{
		return 1;
	}


	FREE(tmpmethod);
	tmpmethod = (char *)malloc(strlen(method) + 16);
	strcpy(tmpmethod, method);

	//default
	return 0;
}

bool HTTPSSocket::setRequestProperty(const char *prop)
{
	if(prop == NULL)
	{
		return 1;
	}


	FREE(tmpprop);
	tmpprop = (char *)malloc(strlen(prop) + 16);
	strcpy(tmpprop, prop);

	//default
	return 0;
}


bool HTTPSSocket::open()
{
	if(tmphost == NULL)
	{
		return 1;
	}

	int ret = 0;

	//if already connected,
	//shutdown the connection and
	//overwrite with new connection
	if(sock != INVALID_SOCKET)
	{
		shutdown(sock, 2);
		::close(sock);
		sock = INVALID_SOCKET;
	}


	server.sin_family = AF_INET;
	server.sin_port = htons(443);

	struct hostent *host;

	host = gethostbyname(tmphost);
	if(host == NULL)
	{
		return 1;
	}

	memcpy(&hostip, *(host->h_addr_list), sizeof(struct in_addr));
	server.sin_addr = hostip;


	//TCP
	ret = socket(AF_INET, SOCK_STREAM, 0);
	if(ret < 0)
	{
		return 1;
	}
	else
	{
		sock = ret;
	}

	ret = connect(sock, (struct sockaddr *)&server, sizeof(server));
	if(ret != 0)
	{
		if(sock != INVALID_SOCKET)
		{
			shutdown(sock, 2);
			::close(sock);
			sock = INVALID_SOCKET;
		}

		return 1;
	}


	//SSL
	RAND_poll();
	while(RAND_status() == 0)
	{
		unsigned short rand_ret = rand() % 65536;
		RAND_seed(&rand_ret, sizeof(rand_ret));
	}	

	//make a new SSL connection
	ssl = SSL_new(ctx);
	if(ssl == NULL)
	{
		return 1;
	}
	
	//bind a sock to ssl connection
	ret = SSL_set_fd(ssl, sock);
	if(ret == 0)
	{
		ssl = NULL;
		return 1;
	}

	//connect with SSL connection
	ret = SSL_connect(ssl);
	if(ret != 1)
	{
		ssl = NULL;
		return 1;
	}
	

	//default
	return 0;		
}

bool HTTPSSocket::close()
{
	if(sock == INVALID_SOCKET)
	{
		return 1;	//connection is closed
	}

	if(ssl != NULL)
	{
		SSL_shutdown(ssl);
		SSL_free(ssl);
		ssl = NULL;
	}

	if(sock != INVALID_SOCKET)
	{
		shutdown(sock, 2);
		::close(sock);
		sock = INVALID_SOCKET;
	}


	//default
	return 0;
}

bool HTTPSSocket::send(char *buftosend)
{
	if(sock == INVALID_SOCKET || tmphost == NULL) 
	{
		return 1;
	}

	if(tmpmethod == NULL) 
	{
		tmpmethod = (char *)malloc(16);
		strcpy(tmpmethod, "GET");
	}


	if(buftosend == NULL)
	{
		buftosend = NULL;
	}


	char *tmprequest = (char *)malloc(
		strlen(tmphost) + strlen(tmpobject) + strlen(tmpmethod)	//HTTP header
		+ strlen(buftosend)	//buf to send
		+ 128);	//'HTTP/1.1'

	sprintf(tmprequest,
		"%s "			//method
		"%s "			//object
		"HTTP/1.1\r\n"	//version
		"%s\r\n\r\n"	//properties
		"%s\r\n"		//buf to send
		, tmpmethod, tmpobject, tmpprop, buftosend);

	if(SSL_write(ssl, tmprequest, strlen(tmprequest)) < 0)
	{
		return 1;
	}

	FREE(tmprequest);

	//default
	return 0;
}

int HTTPSSocket::recv(char *buftosave)
{
	if(	sock == INVALID_SOCKET || 
		buftosave == NULL)
	{
		return 1;
	}


	int RecvLen = 0;
	int LineBufLen = 4096;
	int TotalBufLen = 0;

	char *LineBuf = (char *)malloc(LineBufLen);
	char *TotalBuf = (char *)malloc(LineBufLen);

	if(LineBuf == NULL || TotalBuf == NULL)
	{
		printf("No more memory...\n");
		return 1;
	}

	memset(LineBuf, '\0', LineBufLen);
	memset(TotalBuf, '\0', LineBufLen);


	int i = 0;
	while(1)
	{
		//for safety
		if(i > 1000)
		{
			break;
		}
		else
		{
			i++;
		}

		memset(LineBuf, '\0', LineBufLen);
		RecvLen = SSL_read(ssl, LineBuf, LineBufLen - 1);
		if(RecvLen == 0) break;

		
		//find a separator
		char *found = strstr(LineBuf, "\0");
		if(found != NULL)
		{
			//found
			TotalBufLen += RecvLen;
			char *tmp = (char *)realloc(TotalBuf, TotalBufLen + 16);
			if(tmp != NULL)
			{
				TotalBuf = tmp;
			}
			else
			{
				free(tmp);

				free(LineBuf);
				free(TotalBuf);
				return 1;
			}
			strcat(TotalBuf, LineBuf);
			break;
		}
		else
		{
			//not found;continue to recv
			*(LineBuf + RecvLen) = '\0';
			TotalBufLen += RecvLen;
			char *tmp = (char *)realloc(TotalBuf, TotalBufLen + 16);
			if(tmp != NULL)
			{
				TotalBuf = tmp;
			}
			else
			{
				free(tmp);

				free(LineBuf);
				free(TotalBuf);
				return 1;
			}

			strcat(TotalBuf, LineBuf);
		}
	}

	strcpy(buftosave, TotalBuf);
	free(LineBuf);
	free(TotalBuf);


	//default
	return TotalBufLen;
}



/*
 *	GetXMLTagNode
 *	-something like tag.getInnerText();
 *	
 *@return
 *	0: success
 *	-1: failed to alloc memory
 *	1: tag not found
 *	2: end of start-tag not found
 *	3: beginning of end-tag not found
 *@params
 *	buf: pointer to a buffer to search in
 *	tagname: name of a tag to search the node in
 *	string: pointer to save the string of node
 *
 */
int GetXMLTagNode(char *buf, const char *tagname, char *string)
{
	//this func can only search the FIRST tag in buffer.
	char *found = NULL;
	char *tmptag = NULL;
	char *temp = NULL;
	int pos = 0;

	if(buf == NULL || tagname == NULL || string == NULL)
	{
		return -1;
	}

	//there are two patterns to match a tag:
	//'<tagname>' or '<tagname '
	tmptag = (char *)malloc((strlen(tagname) + 32));	//'<' + '>' or '<' + ' '
	if(tmptag == NULL)
	{
		printf("fuck! no more memory!\n");
		return -1;
	}
	sprintf(tmptag, "<%s>", tagname);

	found = strstr(buf, tmptag);
	if(found == NULL) 
	{
		sprintf(tmptag, "<%s ", tagname);
		found = strstr(buf, tmptag);
		if(found == NULL)
		{
			free(tmptag);
			tmptag = NULL;
			return 1;	//tag not found
		}
	}
	pos = (int)(found-buf) - 1;
	pos += strlen(tmptag);
	temp = buf+pos;		

	found = strstr(temp, ">");		//find '>' of the start-tag
	if(found == NULL) 
	{
		free(tmptag);
		tmptag = NULL;
		return 2;	//end of start-tag not found(ex. <chat data="aaa" )
	}
	pos = (int)(found-buf) + 1;	//pointer to the first character of node


	//find the begginning of end-tag
	found = strstr(buf+pos, "<");
	if(found == NULL)
	{
		free(tmptag);
		tmptag = NULL;
		return 3;	//not found
	}

	//copy to buffer until the beginning of end-tag
	for(int i=pos; buf[i]!='<'; i++)
	{
		string[i-pos] = buf[i];
		string[i-pos+1] = '\0';
	}

	free(tmptag);
	tmptag = NULL;
	return 0;
}



/*
 *	GetXMLTagAttribute
 *	
 *@return
 *	0: success
 *	1: tag not found
 *	2: end of tag not found(for safety)
 *	3: attribute not found
 *	4: end of attribute not found
 *@params
 *	buf: pointer to a buffer to search in
 *	tagname: name of a tag to search the attribute in
 *	attr: pointer to the name of attribute
 *	value: pointer to save the value of attribute
 *
 */
int GetXMLTagAttribute(char *buf, const char *tagname, const char *attr, char *string)
{
	char *found = NULL;
	int pos = 0;
	int start = 0;
	int end = 0;
	char *tmptag;
	char *tmpprop;
	char *temp;

	tmptag = (char *)malloc((strlen(tagname) + 16));	//'<' + '>'
	if(tmptag == NULL)
	{
		printf("fuck! no more memory!");
		return -1;
	}
	sprintf(tmptag, "<%s>", tagname);

	found = strstr(buf, tmptag);
	if(found == NULL) 
	{
		sprintf(tmptag, "<%s ", tagname);
		found = strstr(buf, tmptag);

		if(found == NULL)
		{
			free(tmptag);
			tmptag = NULL;
			return 1;	//tag not found
		}
	}
	pos = (int)(found-buf);
	start = pos;
	pos += strlen(tmptag);

	temp = buf+pos;
	found = strstr(temp, ">");	//find the end of tag
	if(found == NULL) 
	{
		free(tmptag);
		tmptag = NULL;
		return 2;	//end of tag not found
	}
	end = (int)(found-buf);



	//find a attribute in the tag found
	tmpprop = (char *)malloc((strlen(attr) + 16));	//'="'
	sprintf(tmpprop, "%s=\"", attr);

	found = strstr(buf, tmpprop);
	if(found == NULL) 
	{
		free(tmptag);
		tmptag = NULL;
		free(tmpprop);
		tmpprop = NULL;
		return 3;
	}
	pos = (int)(found-buf);
	pos += strlen(tmpprop);

	//for safety, find the end 
	//of the attribute
	found = strstr(buf+pos, "\"");
	if(found == NULL)
	{
		free(tmptag);
		tmptag = NULL;
		free(tmpprop);
		tmpprop = NULL;
		return 4;
	}
	else
	{
		if(!((int)(found-buf) < end))
		{
			free(tmptag);
			tmptag = NULL;
			free(tmpprop);
			tmpprop = NULL;
			return 4;
		}
	}
	
	//copy
	if(pos > start && pos < end)
	{
		for(int i=pos; buf[i]!='\"'; i++)
		{
			string[i-pos] = buf[i];
			string[i-pos+1] = '\0';
		}
	}
	else
	{
		free(tmptag);
		tmptag = NULL;
		free(tmpprop);
		tmpprop = NULL;
		return 3;
	}

	free(tmptag);
	tmptag = NULL;
	free(tmpprop);
	tmpprop = NULL;
	return 0;
}

