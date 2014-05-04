
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
/*
�����socket״̬ת��
	Ready ----> Listen ----> Dead

�ͻ���socket״̬ת��
	
c --> s		Accept ----> Recive ----> Dead
					|
s---> c				---> Send   ----> Dead

��Ϊ�ͻ���ʱsocket״̬ת��
	Ready -----> connect ----> Recive ----> Dead
						   |
						   --> Send   ----> Dead
*/

// sokcet��ʼ״̬
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

// ����״̬�� ����˼���socket����
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

// ��������״̬
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

// ��������״̬�� �ͻ���socket����
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

// ������״̬�� ��ʼsocketʱ�� 
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

// ���ӿͻ���״̬�� �ͻ���socket����
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