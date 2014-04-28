
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

// 准备好被连接
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