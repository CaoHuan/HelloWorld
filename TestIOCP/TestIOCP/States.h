
#pragma once
#include "Common.h"
#include "ProtoSocket.h"
_CDH_BEGIN

template <typename T>
struct State
{
public:
	virtual ~State();

	// 进入该状态
	virtual void Enter(T*) = 0;

	// 该状态， socket有消息时，如何执行
	virtual void Execute(T*) = 0;

	// 改变状态之前的操作
	virtual void Exit(T*) = 0;
};
/*
服务端socket状态转变
	Ready ----> Listen ----> Dead

客户端socket状态转变
	
c --> s		Accept ----> Recive ----> Dead
					|
s---> c				---> Send   ----> Dead

作为客户端时socket状态转变
	Ready -----> connect ----> Recive ----> Dead
						   |
						   --> Send   ----> Dead
*/

// sokcet初始状态
class StateReady : public State<ProtoSocket>
{
public:
	virtual void Enter(ProtoSocket* socket)
	{
	}

	virtual void Execute(ProtoSocket* socket)
	{
	}

	virtual void Exit(ProtoSocket* socket)
	{
	}
};

// 监听状态， 服务端监听socket独有
class StateListen : public State<ProtoSocket>
{
public:

	virtual void Enter(ProtoSocket* socket)
	{

	}

	virtual void Execute(ProtoSocket* socket)
	{

	}

	virtual void Exit( ProtoSocket* socket)
	{
	}
};

// 接收数据状态
class StateRecive : public State<ProtoSocket>
{
public:


	virtual void Enter(ProtoSocket* socket)
	{
		
	}

	virtual void Execute(ProtoSocket* socket)
	{
			
	}

	virtual void Exit( ProtoSocket* socket)
	{
	}
};

// 发送数据状态， 客户端socket独有
class StateSend : public State<ProtoSocket>
{
public:
	virtual void Enter(ProtoSocket* socket)
	{
	}

	virtual void Execute(ProtoSocket* socket)
	{
	}

	virtual void Exit(ProtoSocket* socket)
	{
	}
};

// 待连接状态， 初始socket时， 
class StateAccept : public State<ProtoSocket>
{
public:

	virtual void Enter(ProtoSocket* socket)
	{
	}

	virtual void Execute(ProtoSocket* socket)
	{
	}

	virtual void Exit(ProtoSocket* socket)
	{
	}
};

// 连接客户端状态， 客户端socket独有
class StateConnect : public State<ProtoSocket>
{
public:
	virtual void Enter(ProtoSocket* socket)
	{
	}

	virtual void Execute(ProtoSocket* socket)
	{
	}

	virtual void Exit(ProtoSocket* socket)
	{
	}
};

// 已无效的连接
class StateDead : public State<ProtoSocket>
{
public:
	virtual void Enter(ProtoSocket* socket)
	{
	}

	virtual void Execute(ProtoSocket* socket)
	{
	}

	virtual void Exit(ProtoSocket* socket)
	{
	}
};


_CDH_END