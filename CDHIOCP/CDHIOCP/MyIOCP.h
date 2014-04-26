#pragma  once

#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

#include <MSWSock.h>
#include <process.h>
#include <map>

#include "CommonHeader.h"

_CDH_BEGIN

    //LPFN_ACCEPTEX     m_lpfnAcceptEx;         // AcceptEx函数指针

#define MAXAcceptEx 150 //保证最多有多少个socket再等待连接
#define MAXSpinCount 4000 //旋转次数
#define MAXRECEIVEDBUFFLENGTH 1024 * 8 * 20 // 8k的倍数比较好
    using namespace std;

enum IOCP_OPERATION_TYPE
{
    E_INVALID= 0x11, //还没开始用
    E_ACCEPT, //服务端的操作，有客户端连上了
    E_RECV, //接收数据
    E_SEND, //发送
    E_CONNECT , //连接上了服务器 (作为客户端时有的操作)
};

struct PerHandleData
{
    SOCKET selfSocket;
    SOCKET hasListenSocket;//监听套接字，如果这是与客户端相连的，那么这个字段表示本地的监听套接字， 如果是与服务端相连，不用管这个值  [12/27/2013 CDH]
};

struct PerIOData
{
    OVERLAPPED overlapped;
    WSABUF dataBuf;
    SOCKET remoteSocket;
    char buffer[MAXRECEIVEDBUFFLENGTH];  //接收的数据
    int bufferLen;
    IOCP_OPERATION_TYPE operationType; //参考IOCP_OPERATION_TYPE
};

struct MiddleDataBuffer
{
    char buffer[MAXRECEIVEDBUFFLENGTH]; //缓冲区
    unsigned int bufferLength; //缓冲区大小
};

class WrapSocket
{
public:
    // 当port为0时， 就不是监听socket
    WrapSocket(unsigned int listenPort = 0) : mSocket(WSASocket(AF_INET,SOCK_STREAM,0,NULL,0,WSA_FLAG_OVERLAPPED))
        , mPort(listenPort)
        , sentBufferLength(0)
        , receivedBufferLength(0)
        , mIsSending(FALSE)
    {
        memset(&mHandleData, 0, sizeof(mHandleData));

        memset(&mIODataReceive, 0, sizeof(mIODataReceive));
        memset(&mIODataSend, 0, sizeof(mIODataSend));


        memset(sentBuffer, 0, sizeof(sentBuffer));
        memset(receivedBuffer, 0, sizeof(receivedBuffer));

    }

    PerIOData& GetPerIODataReceive()
    {
        return mIODataReceive;
    }

    PerIOData& GetPerIODataSend()
    {
        return mIODataSend;
    }

    PerHandleData& GetPerHandleData()
    {
        return mHandleData;
    }

    SOCKET& GetSocket()
    {
        return mSocket;
    }

    BOOL GetListenPort(unsigned int& port)
    {
        if (mPort != 0)
        {
            port = mPort;
            return TRUE;
        }
        return FALSE;
    }
    
    // 将状态设置为正在发送数据
    void SendingData(bool isSending)
    {
        mIsSending = isSending;
    }

  
    // 检测是否能发送数据
    BOOL CanSendData()
    {
        return !mIsSending;
    }
    ~WrapSocket()
    {
        closesocket(mSocket);
    }


private:
    PerHandleData mHandleData;
    SOCKET mSocket;
    unsigned int mPort; //如果是监听socket，那么监听哪个端口呢？
    PerIOData mIODataReceive; //接收数据时用的 
    PerIOData mIODataSend;   //发送数据时用的
    volatile bool mIsSending; //是否在投递数据 这个地方不用做线程同步， 因为他始终是在manager的主线程中设置成true， 在完成端口线程中设置成false，所以哪怕在主线程中取到false造成错过该次投递消息，也不碍事
    /************************************************************************/
    /* 下面的四个字段，目前没用上                                            */
    /************************************************************************/
    char receivedBuffer[MAXRECEIVEDBUFFLENGTH];  //接收数据的缓存
    unsigned int receivedBufferLength;
    char sentBuffer[MAXRECEIVEDBUFFLENGTH]; //发送数据时， 先将数据缓存到这。相当于一个队列， 如果前面的数据来不及发送，就将数据添加到末尾
    unsigned int sentBufferLength;
};

struct DataBufferKey
{
    int listenSocket; //监听套接字，如果有的话
    int selfSocket; //自身，
    DataBufferKey(int listen, int self)
    {
        listenSocket = listen;
        selfSocket = self;
    }
    bool operator==(const DataBufferKey& other)
    {
        if (selfSocket == other.selfSocket && listenSocket == other.listenSocket)
        {
            return true;
        }

        return false;
    }
    bool operator!=(const DataBufferKey& other)
    {
        return !(*this == other);
    }

    bool operator>(const DataBufferKey& other)
    {
        if (listenSocket > other.listenSocket)
        {
            return TRUE;
        }
        else if(listenSocket == other.selfSocket)
        {
            if (selfSocket > other.selfSocket)
            {
                return TRUE;
            }
            else if(selfSocket == other.selfSocket)
            {
                return FALSE;
            }
            else
            {
                return FALSE;
            }
        }
        else
        {
            return FALSE;
        }
    }

    bool operator<(const DataBufferKey& other)
    {
        if (*this != other)
        {
            if (!(*this > other))
            {
                return TRUE;
            }
        }
        return FALSE;
    }
};



typedef map<DataBufferKey, MiddleDataBuffer*> MIDDLEDATABUFFERMAP;
typedef map<int, WrapSocket*> SocketMap; // SOCKET的映射


#define  IOCPMgr MyIOCP::Instance()

// socket为套接字的ID, data是接收到的数据， datalength是接收到的数据长度， 有消息接收到时， 就触发它，注意：这个函数只会在主线程中被调用
// 注意 data指针是个局部的， 如果要保存数据，请自己申请一块内存
typedef void (*processFun)(int socket, char* data, unsigned int dataLength); 
typedef map<int, processFun> FunMap; //相应socket的处理函数
class MyIOCP
{
public:
    static MyIOCP* Instance();

    // 初始化操作, 参数表示每个监听Socket最大的创建好准备连接的remoteSocket数量
    BOOL SetInit(unsigned int maxAcceptExSocket = 50); 

    void Update(); //必须每帧操作

    BOOL Send(int socket, char* message, unsigned int messageLength); //给外面的向远程主机发送消息的接口

public:// 作为服务端的操作

    // 添加一个监听socket, fun为其处理函数，注意函数中的线程同步问题
    BOOL AddListenerSocket(WrapSocket* listen, processFun fun); 


public:// 作为客户端应有的操作
    BOOL ConnectToRemoteServer(WrapSocket* listen, processFun fun); // 主动连接一个服务器

private:
    MyIOCP() : mMaxAcceptExSocket(0)
        , mCompletionPort(nullptr)
        , mlpfnAcceptEx(nullptr)
        , mlpfnGetAcceptExSockAddrs(nullptr)
    {
        InitializeCriticalSectionAndSpinCount(&mCriticalSetionOfConnectedSockets, MAXSpinCount);
        InitializeCriticalSectionAndSpinCount(&mCriticalSetionOfListenerSockets, MAXSpinCount);
        InitializeCriticalSectionAndSpinCount(&mCriticalSetionOfReadyToBeConnected, MAXSpinCount);
        InitializeCriticalSectionAndSpinCount(&mCriticalSetionOfReceivedData, MAXSpinCount);
        InitializeCriticalSectionAndSpinCount(&mCriticalSetionOfSentData, MAXSpinCount);
    }
    MyIOCP(const MyIOCP&);
    MyIOCP& operator=(const MyIOCP&);

    //开始监听
    BOOL StartListen(WrapSocket* listenSocket);

    // 预先创建好连接的socket
    BOOL PostAcceptEx(SOCKET listenSocket);

    BOOL PostReceive(WrapSocket* wrapSocket);

    //将套接字与完成端口关联起来
    BOOL AssociateDeviceWithCompletionPort(WrapSocket* socket);

    static DWORD WINAPI WorkerThread(LPVOID lpParam); //线程函数

    void Execute(); //执行完成端口消息响应

private:
    // 缓冲区消息投递的辅助函数
    void Receive(WrapSocket* socket); //从完成端口中获取的消息
    void DispatchAllReceivedData(); //将缓冲的接收到的数据全都投递出去
    void PostSendAllBufferedData(); //将缓冲的欲发送的数据全部send

private: //辅助函数，用来操作相应的map

    enum WRAPESOCKETTYPE 
    {
        E_CONNECTED, //已连接上的
        E_LISTENER,  //监听的
        E_READYTOBECONNECTED, //等待被连接的

    };

    //专门为了获得一个WrapSocket发送或者数据，此处不必同步了
    BOOL GetSocketForSendOrRecvData(int socket, WrapSocket*& wrapSocket);
    
    //将一个socket传进来，得到相应的WrapSocket ，  取出后， 要自己负责delete //   注意， 取出E_LISTENER类型的套接字时， 要记得删除相对应的处理函数映射processFun
    WrapSocket* GetSocketAndEraseFromTheMap(int socket, WRAPESOCKETTYPE type);

    //将一个WrapSocket插入，便于管理
    void InsertWrapSocketToMap(WrapSocket* socket, WRAPESOCKETTYPE type);
    

    LPFN_ACCEPTEX     mlpfnAcceptEx;         // AcceptEx函数指针
    LPFN_GETACCEPTEXSOCKADDRS  mlpfnGetAcceptExSockAddrs; //AcceptEx的配套函数指针
    unsigned int mMaxAcceptExSocket; //每个监听socket最多有几个创建好的socket在等待被连接
    HANDLE mCompletionPort; //完成端口句柄

    FunMap mFuns; //每个socket的消息处理函数

    /************************************************************************/
    /* 下面每个变量的访问，都必须做线程同步！！！                             */
    /************************************************************************/
    CRITICAL_SECTION mCriticalSetionOfReceivedData;
    MIDDLEDATABUFFERMAP mReceivedDataMap; //每个socket接收到的消息数据缓冲区

    CRITICAL_SECTION mCriticalSetionOfSentData;
    MIDDLEDATABUFFERMAP mSentDataMap; //每个socket要发送的消息数据缓冲区

    CRITICAL_SECTION mCriticalSetionOfConnectedSockets;
    SocketMap mConnectedSockets; // 已经连接上的SOCKET

    CRITICAL_SECTION mCriticalSetionOfListenerSockets;
    SocketMap mListenerSockets;  // 监听SOCKET

    CRITICAL_SECTION mCriticalSetionOfReadyToBeConnected;
    SocketMap mReadyToBeConnected; // 通过AcceptEx创建好的等待被连接的socket, key表示相应的监听socket的int值

    
};


_CDH_END