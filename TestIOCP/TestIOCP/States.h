
#pragma once
#include "Common.h"
#include "ProtoSocket.h"
_CDH_BEGIN

template <typename T>
struct State
{
public:
	virtual ~State();

	// �����״̬
	virtual void Enter(T*) = 0;

	// ��״̬�� socket����Ϣʱ�����ִ��
	virtual void Execute(T*) = 0;

	// �ı�״̬֮ǰ�Ĳ���
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

// ׼���ñ�����
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

// ����Ч������
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