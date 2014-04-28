#pragma once


#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#include "Common.h"
_CDH_BEGIN

// 原始socket，对底层的socket，及其相关数据结构进行一个简单的封装
struct ProtoSocket
{
	OVERLAPPED		mOverlapped;
	SOCKET			mSocket;
	WSABUF			mDataBufPtr;
};


_CDH_END