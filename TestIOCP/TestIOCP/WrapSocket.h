#pragma once


#include <list>

#include "ProtoSocket.h"
#include "StateMachine.h"
#include "MessageInterface.h"
_CDH_BEGIN



// ��װ����socket
class ServerSocket : public ProtoSocket
{
public:
	ServerSocket(int port, IServerSocketMessageHandler *handle) 
		: ProtoSocket()
		 ,mPort(port)
	{
	}

	void StartListen()
	{

	}

	void CloseSocket();

private:

	//ProtoSocket mSocket;//mSelfSocket;
	IServerSocketMessageHandler *mMessageHandle;
	//std::list<ProtoSocket*> mConnectedSocket;

	
	// ������socketֻ�Ǹ�������� ������ ֻ��һ��״̬��
	SocketStateMachine<ProtoSocket> mFSMS2C;

	StateReady  mStateReady;

	StateListen mStateListen;

	StateDead	mStateDead;

	int mPort; 
};

// ��װ����socket
class ClientSocket : public ProtoSocket
{
public:
	ClientSocket(IMessageHandler *handler);
	ClientSocket(int addr, int port, IMessageHandler *handler);

	void OnRecive(char *data, UINT len);

	void OnSend(char *data, UINT len);

private:

	//ProtoSocket mSocket;//mSelfSocket;
	IMessageHandler *mMessageHandler;

	// socket��˫���ģ� ����������������״̬����
	// һ����ӦC --> S
	// һ����ӦS --> C

	// C --> S ״̬��
	SocketStateMachine<ProtoSocket> mFSMC2S;

	// S --> C ״̬��
	SocketStateMachine<ProtoSocket> mFSMS2C;

	StateReady		mStateReady;

	StateRecive		mStateRecive;
	StateAccept		mStateAccept;

	StateSend		mStateSend;
	StateConnect	mStateConnect;

	StateDead		mStateDead;
};


_CDH_END