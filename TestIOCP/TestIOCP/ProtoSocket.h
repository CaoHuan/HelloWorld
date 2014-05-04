#pragma once


#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#include "Common.h"
_CDH_BEGIN

// 原始socket，对底层的socket，及其相关数据结构进行一个简单的封装
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