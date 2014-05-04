#pragma once

#include "Common.h"

_CDH_BEGIN

// ��Ž�������
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

	//append������յ�������
	void ReciveData(char *data, UINT len)
	{
		if (data == nullptr || len <= 0)
		{
			return;
		}
		UINT copyLen = len + mCurrentDataLen > mTotalDataLen ? mTotalDataLen - mCurrentDataLen : len	;

		memcpy(mData + mCurrentDataLen, data, copyLen);
	}

	// �Ƿ�һ���������ݽ�������
	bool IsAllRecived()
	{
		return mCurrentDataLen == mTotalDataLen;
	}

	// �����������ȡ��
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

	// ��ǰ���ݵĴ�С
	UINT  mCurrentDataLen;

	// ����һ�����
	UINT mTotalDataLen;

};


_CDH_END