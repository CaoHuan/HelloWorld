#pragma once


#include <list>

#include "ProtoSocket.h"
#include "StateMachine.h"
#include "MessageInterface.h"
_CDH_BEGIN



// 封装监听socket
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

	
	// 服务器socket只是负责监听， 单工， 只有一个状态机
	SocketStateMachine<ProtoSocket> mFSMS2C;

	StateReady  mStateReady;

	StateListen mStateListen;

	StateDead	mStateDead;

	int mPort; 
};

// 封装连接socket
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

	// socket是双工的， 所以在这里有两个状态机，
	// 一个对应C --> S
	// 一个对应S --> C

	// C --> S 状态机
	SocketStateMachine<ProtoSocket> mFSMC2S;

	// S --> C 状态机
	SocketStateMachine<ProtoSocket> mFSMS2C;

	StateReady		mStateReady;

	StateRecive		mStateRecive;
	StateAccept		mStateAccept;

	StateSend		mStateSend;
	StateConnect	mStateConnect;

	StateDead		mStateDead;
};


_CDH_END