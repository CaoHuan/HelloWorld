#pragma once
#include "Common.h"

_CDH_BEGIN
// ����socket��Ϣ����
struct IServerSocketMessageHandler 
{
	virtual void OnMessage(int socketID, char *data, unsigned int len) = 0;
};

// ����Ϣ����
struct IMessageHandler
{
	virtual void OnMessage(char *data, unsigned int len) = 0;
};

_CDH_END