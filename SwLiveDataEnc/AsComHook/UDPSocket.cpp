// UDPSocket.cpp: implementation of the CUDPSocket class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "UDPSocket.h"

#pragma comment(lib,"Ws2_32")

/*------------------------------------------------------------------------------*/
//初始化
WORD InitializeSocket()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	WORD wVerGet=0;
	wVersionRequested=MAKEWORD(2,2);
	while(1)
	{
		int err;
		
		err = WSAStartup(wVersionRequested, &wsaData);
		if ( err != 0 ) 
		{
			BYTE* p=(BYTE*)&wVersionRequested;
			if(p[1]>0)
				p[1]--;
			else
			if(p[1]==0)
			{
				p[1]=1;
				if(p[0]>1)
					p[0]--;
				else
				if(p[0]==0)
				{
					break;
				}
			}
			continue;
		}
		else
		{
			wVerGet=wsaData.wVersion;
			break;
		}
	}
	return wVerGet;
	
}
/*------------------------------------------------------------------------------*/
int CleanUpSocket()
{
	return WSACleanup();
}
/*------------------------------------------------------------------------------*/
CUDPSocket::CUDPSocket()
{
	memset((void*)&m_LocalAddr,0,sizeof(m_LocalAddr));
	memset((void*)&m_RemoteAddr,0,sizeof(m_RemoteAddr));
	m_Socket=0;

}
/*------------------------------------------------------------------------------*/
CUDPSocket::~CUDPSocket()
{
	Destroy();
}



/*------------------------------------------------------------------------------*/



BOOL CUDPSocket::Create(UINT uSocketPort,char* pszSockAddr)
{

	BOOL bRet=FALSE;
	if(!m_Socket)
	{
		
		m_LocalAddr.sin_family=AF_INET;
		if(pszSockAddr)
			m_LocalAddr.sin_addr.s_addr=Name2Inet(pszSockAddr);
		else
			m_LocalAddr.sin_addr.s_addr=htonl(INADDR_ANY);
		
		if((m_Socket=socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP))!=INVALID_SOCKET)
		{  //创建无连接数据报套接口
			m_LocalAddr.sin_port=htons(uSocketPort);
			if(bind(m_Socket,(SOCKADDR*)&m_LocalAddr,sizeof(SOCKADDR_IN))==SOCKET_ERROR)
			{   
				m_Socket=0;
				memset((void*)&m_LocalAddr,0,sizeof(m_LocalAddr));
			}
			else
			{
				bRet=TRUE;
			}
		}
	}
	return bRet;

}
/*------------------------------------------------------------------------------*/
int CUDPSocket::Destroy()
{
	int iRet;
//	shutdown(m_Socket,SD_SEND);
	iRet=closesocket(m_Socket);
	memset((void*)&m_LocalAddr,0,sizeof(m_LocalAddr)); 
	memset((void*)&m_RemoteAddr,0,sizeof(m_RemoteAddr));
	m_Socket=0;
	return iRet;
}
/*------------------------------------------------------------------------------*/
int CUDPSocket::SendTo(const char *pBuf, int iLen, char *pszRemoteSockAddr,unsigned short uPort) const
{
	SOCKADDR_IN RemoteAddr;
	RemoteAddr.sin_family=AF_INET;
	RemoteAddr.sin_addr.s_addr=Name2Inet(pszRemoteSockAddr);
	RemoteAddr.sin_port=htons(uPort);
	return sendto(m_Socket,pBuf,iLen,0,(SOCKADDR*)&RemoteAddr,sizeof(SOCKADDR_IN));
}
/*------------------------------------------------------------------------------*/
int CUDPSocket::SendTo(const char *pBuf,int iLen,SOCKADDR* pRemoteAddr) const
{
	if(!pRemoteAddr)
		pRemoteAddr=(SOCKADDR*)&m_RemoteAddr;
	return sendto(m_Socket,pBuf,iLen,0,pRemoteAddr,sizeof(SOCKADDR_IN));
}
/*------------------------------------------------------------------------------*/
int CUDPSocket::RecvFrom(char *pBuf, int iLen,SOCKADDR* pRemoteAddr) const
{
	int addrlen=sizeof(SOCKADDR_IN);
	SOCKADDR_IN RemoteAddr;
	if(!pRemoteAddr)
	{
		pRemoteAddr=(SOCKADDR*)&RemoteAddr;
	}	
	return recvfrom(m_Socket,pBuf,iLen,0,pRemoteAddr,&addrlen);
}
/*------------------------------------------------------------------------------*/
SOCKET CUDPSocket::GetSocket() const
{
	return m_Socket;
}
/*------------------------------------------------------------------------------*/
SOCKADDR_IN CUDPSocket::GetLocalAddr() const
{
	return m_LocalAddr;
}
/*------------------------------------------------------------------------------*/
SOCKADDR_IN CUDPSocket::SetRemoteAddr(const SOCKADDR_IN &NewRemoteAddr)
{
	SOCKADDR_IN OldAddr;
	OldAddr=m_RemoteAddr;
	m_RemoteAddr=NewRemoteAddr;
	return OldAddr;
}
/*------------------------------------------------------------------------------*/


SOCKADDR_IN CUDPSocket::GetRemoteAddr() const
{
	return m_RemoteAddr;
}


/*------------------------------------------------------------------------------*/
u_long CUDPSocket::GetLocal_in_addr()
{
	char szHostname[256];
	if (gethostname(szHostname, sizeof(szHostname)))
	{
		return htonl(INADDR_ANY) ;
	}
	//得到主机信息
	HOSTENT* pHostEnt = gethostbyname(szHostname);
	if (pHostEnt == NULL)
	{
		return htonl(INADDR_ANY);
	}
	
	//检查地址长度
	if (pHostEnt->h_length != 4)
	{
		return htonl(INADDR_ANY);
	}
	
	return   ((struct in_addr *)*pHostEnt->h_addr_list)->s_addr;
	
}
/*------------------------------------------------------------------------------*/

u_long CUDPSocket::Name2Inet(LPCSTR szAddress)
{
	if(szAddress==NULL||strlen(szAddress)<1)
	{
		return htonl(INADDR_ANY);
	}
	BOOL name=FALSE;
	for (int i=0;szAddress[i];i++)
	{
		if((!isdigit(szAddress[i]))&&szAddress[i]!='.')
		{
			name=TRUE;break;
		}
	}
	//不是计算机名，是IP
	if(!name)
		return inet_addr(szAddress); 
	//是计算机名
	else 
	{
		struct hostent*host=gethostbyname(szAddress);
		
		if(host==NULL)
		{
			return htonl(INADDR_ANY);
		}
		return *(long*)host->h_addr;
	}
	
	
}
/*------------------------------------------------------------------------------*/


BOOL CUDPSocket::IsSocket()
{
	return (BOOL)m_Socket;
}
