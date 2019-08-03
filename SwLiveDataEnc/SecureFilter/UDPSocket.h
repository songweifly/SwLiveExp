

#ifndef _UDPSOCKET_H_
#define _UDPSOCKET_H_


//#include "windows.h"
#include <WinSock2.h>
#include "Ethernet.h"

#define MAX_PACKET_SIZE   1024


WORD InitializeSocket();
int CleanUpSocket();

class CUDPSocket
{
public:
	BOOL IsSocket();
	

	CUDPSocket(); 
	virtual ~CUDPSocket();
	
	BOOL Create(UINT uSocketPort=0,char* pszSockAddr=NULL);
	int Destroy();

	int SendTo(const char *pBuf,int iLen,char* pszRemoteSockAddr,unsigned short uPort) const;
	int SendTo(const char *pBuf,int iLen,SOCKADDR* pRemoteAddr=NULL) const;
	
	int RecvFrom(char *pBuf,int iLen,SOCKADDR* pRemoteAddr=NULL) const;
	
	SOCKADDR_IN GetLocalAddr() const;
	SOCKET GetSocket() const;
	
	SOCKADDR_IN GetRemoteAddr() const;
	SOCKADDR_IN SetRemoteAddr(const SOCKADDR_IN &NewRemoteAddr);

		
	static u_long Name2Inet(LPCSTR szAddress);
	static u_long GetLocal_in_addr();

protected:
	
	
	SOCKET m_Socket;
	SOCKADDR_IN m_LocalAddr;
	SOCKADDR_IN m_RemoteAddr;
	
private:

};


#endif