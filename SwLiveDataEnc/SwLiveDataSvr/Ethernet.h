
#ifndef _ZZW_ETHERNET_
#define _ZZW_ETHERNET_
#pragma once

#define SVR_NODE_PORT    7789
#define CLI_NODE_PORT    7799
//
//#define MNGR_NODE         0x0001
//#define CALC_NODE         0x0002  //sw
////cmd:
//#define REMOTE_START      0x0001
//#define REMOTE_EXIT       0x000A

typedef struct 
{
	CString sAccount;
	CString sPwd;
} PACKET;

#endif