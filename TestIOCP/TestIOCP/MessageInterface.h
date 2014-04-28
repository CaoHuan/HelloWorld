#pragma once
#include "Common.h"

_CDH_BEGIN
// 监听socket消息触发
struct IServerSocketMessageHandler 
{
	virtual void OnMessage(int socketID, char *data, unsigned int len) = 0;
};

// 有消息触发
struct IMessageHandler
{
	virtual void OnMessage(char *data, unsigned int len) = 0;
};

_CDH_END