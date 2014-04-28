#pragma once

#include "Common.h"
_CDH_BEGIN

/*
��дȨ�޵������ߡ��������߶��С����ǵ���ɶ˿���д������ʱ�� ���ж���̣߳�����д������Ҫ�߳�ͬ����������������һ���߳̽��У�����û���ڶ��ĵط���ͬ��
*/
template<typename DATA_>
class ProducerConsumerQUeue
{
	struct Node
	{
		DATA_ data;
		Node *next;
	};

	Node *mpHead; // ͷ���
	Node *mpTail; // β���
	volatile long	mWriteLock; // д��
	volatile long   mReadLock;
public:
	ProducerConsumerQUeue() : mWriteLock(0)
							 ,mReadLock(0)
	{
		mpHead = new Node;

		mpHead->next = NULL;
		mpTail = mpHead;
	}

	bool Push(DATA_& data)
	{
		if (GetWritePermission())
		{
			Node *pNode = new Node;
			pNode->data = data;
			pNode->next = 0;

			mpTail->next= pNode;
			mpTail		= pNode;

			ReleaseWritePermission();
			return true;
		}

		return false;
	}

	bool Pop(DATA_& data)
	{
		bool ret = false;
		if (GetReadPermission())
		{
				
			if (mpHead->next != NULL)
			{
				Node *pNode = mpHead;
				mpHead = mpHead->next;
				data = mpHead->data;
				delete pNode;
				ret = true;
			}

			ReleaseReadPermission();
			return ret;
		}

		return ret;
	}

	bool GetReadPermission()
	{
		int spin = 1;
		while (::_InterlockedCompareExchange(&mReadLock, 1, 0) == 0)
		{
			++spin;
			if (spin >= SpinCount)
			{
				return false;
			}

			Sleep(1);
		}

		return true;
	}

	void ReleaseReadPermission()
	{
		mReadLock = 0;
	}

	bool GetWritePermission()
	{
		//int spin = 1;
		while (::_InterlockedCompareExchange(&mWriteLock, 1, 0) == 0)
		{
// 			++spin;
// 			if (spin >= SpinCount)
// 			{
// 				return false;
// 			}

			Sleep(1);
		}

		return true;
	}

	void ReleaseWritePermission()
	{
		mWriteLock = 0;
	}



	~ProducerConsumerQUeue()
	{
		DATA_ tmp;
		while (Pop(tmp) )
		{
		}

		if (mpHead)
		{
			delete mpHead;

			mpHead = NULL;
			mpTail = NULL;
		}
	}
};




_CDH_END