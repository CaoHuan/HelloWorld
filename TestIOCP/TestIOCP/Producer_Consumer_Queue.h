#pragma once

#include "Common.h"
_CDH_BEGIN

/*
带写权限的生产者——消费者队列。考虑到完成端口在写入数据时， 会有多个线程，所以写操作需要线程同步。而读操作是由一个线程进行，所以没有在读的地方做同步
*/
template<typename DATA_>
class ProducerConsumerQUeue
{
	struct Node
	{
		DATA_ data;
		Node *next;
	};

	Node *mpHead; // 头结点
	Node *mpTail; // 尾结点
	volatile long	mWriteLock; // 写锁
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