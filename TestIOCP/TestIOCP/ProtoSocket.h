#pragma once


#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#include "Common.h"
_CDH_BEGIN

// ԭʼsocket���Եײ��socket������������ݽṹ����һ���򵥵ķ�װ
struct ProtoSocket
{
	ProtoSocket(): mSocket(WSASocket(AF_INET,SOCK_STREAM,0,NULL,0,WSA_FLAG_OVERLAPPED) )
	{
	}

	virtual ~ProtoSocket(){};
	
	virtual void OnExcute() = 0;

	UINT GetID()
	{
		return mSocket;
	}

	OVERLAPPED		mOverlapped;
	SOCKET			mSocket;
	WSABUF			mDataBufPtr;
};


_CDH_END