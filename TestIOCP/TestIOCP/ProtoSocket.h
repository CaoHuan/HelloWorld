#pragma once


#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#include "Common.h"
_CDH_BEGIN

// ԭʼsocket���Եײ��socket������������ݽṹ����һ���򵥵ķ�װ
struct ProtoSocket
{
	OVERLAPPED		mOverlapped;
	SOCKET			mSocket;
	WSABUF			mDataBufPtr;
};


_CDH_END