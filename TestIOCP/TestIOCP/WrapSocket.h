#pragma once

#include <memory>
#include <list>

#include "ProtoSocket.h"
#include "StateMachine.h"
#include "MessageInterface.h"
_CDH_BEGIN

// 存放接收数据
class BuffData
{
public:
	BuffData(UINT totalLen, UINT currentLen, char* data) 
		: mData(new char[totalLen])
		 ,mCurrentDataLen(currentLen)
		 ,mTotalDataLen(totalLen)
	{
		ZeroMemory(mData, totalLen);

		memcpy(mData, data, currentLen);
	}

	//append后面接收到的数据
	void ReciveData(char *data, UINT len)
	{
		if (data == nullptr || len <= 0)
		{
			return;
		}
		UINT copyLen = len + mCurrentDataLen > mTotalDataLen ? mTotalDataLen - mCurrentDataLen : len	;

		memcpy(mData + mCurrentDataLen, data, copyLen);
	}

	// 是否将一个包的数据接收完整
	bool IsAllRecived()
	{
		return mCurrentDataLen == mTotalDataLen;
	}

	// 将包里的数据取出
	bool GetData(char **pData, int& len)
	{
		if (!IsAllRecived())
		{
			return false;
		}

		*pData = mData;
		mData = nullptr;

		len	 = mTotalDataLen;

		return true;
	}
	
	int GetTotallLen()
	{
		return mTotalDataLen;
	}
	
	int GetCurrentLen()
	{
		return mCurrentDataLen;
	}

	~BuffData()
	{
		if (mData)
		{
			delete[] mData;
		}
	}
private:

	char* mData;

	// 当前数据的大小
	UINT  mCurrentDataLen;

	// 数据一共多大
	UINT mTotalDataLen;

};

class ServerSocket
{
public:
	ServerSocket(int port, IServerSocketMessageHandler *handle)
	{
	}
private:

	ProtoSocket mSocket;//mSelfSocket;
	IServerSocketMessageHandler *mMessageHandle;
	//std::list<ProtoSocket*> mConnectedSocket;

	SocketStateMachine<ProtoSocket> *mSM;

	StateRecive mStateRecive;
	StateAccept mStateAccept;
	StateSend   mStateSend;
	StateReady	mStateReady;
	StateDead	mStateDead;
};

class ClientSocket
{
public:
	ClientSocket(IMessageHandler *handler);
	ClientSocket(int addr, int port, IMessageHandler *handler);

private:

	ProtoSocket mSocket;//mSelfSocket;
	IMessageHandler *mMessageHandler;

	SocketStateMachine<ProtoSocket> *mSM;

	StateRecive mStateRecive;
	StateAccept mStateAccept;
	StateSend   mStateSend;
	StateReady	mStateReady;
	StateDead	mStateDead;
};


_CDH_END