#pragma  once

#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

#include <MSWSock.h>
#include <process.h>
#include <map>

#include "CommonHeader.h"

_CDH_BEGIN

    //LPFN_ACCEPTEX     m_lpfnAcceptEx;         // AcceptEx����ָ��

#define MAXAcceptEx 150 //��֤����ж��ٸ�socket�ٵȴ�����
#define MAXSpinCount 4000 //��ת����
#define MAXRECEIVEDBUFFLENGTH 1024 * 8 * 20 // 8k�ı����ȽϺ�
    using namespace std;

enum IOCP_OPERATION_TYPE
{
    E_INVALID= 0x11, //��û��ʼ��
    E_ACCEPT, //����˵Ĳ������пͻ���������
    E_RECV, //��������
    E_SEND, //����
    E_CONNECT , //�������˷����� (��Ϊ�ͻ���ʱ�еĲ���)
};

struct PerHandleData
{
    SOCKET selfSocket;
    SOCKET hasListenSocket;//�����׽��֣����������ͻ��������ģ���ô����ֶα�ʾ���صļ����׽��֣� ��������������������ù����ֵ  [12/27/2013 CDH]
};

struct PerIOData
{
    OVERLAPPED overlapped;
    WSABUF dataBuf;
    SOCKET remoteSocket;
    char buffer[MAXRECEIVEDBUFFLENGTH];  //���յ�����
    int bufferLen;
    IOCP_OPERATION_TYPE operationType; //�ο�IOCP_OPERATION_TYPE
};

struct MiddleDataBuffer
{
    char buffer[MAXRECEIVEDBUFFLENGTH]; //������
    unsigned int bufferLength; //��������С
};

class WrapSocket
{
public:
    // ��portΪ0ʱ�� �Ͳ��Ǽ���socket
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
    
    // ��״̬����Ϊ���ڷ�������
    void SendingData(bool isSending)
    {
        mIsSending = isSending;
    }

  
    // ����Ƿ��ܷ�������
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
    unsigned int mPort; //����Ǽ���socket����ô�����ĸ��˿��أ�
    PerIOData mIODataReceive; //��������ʱ�õ� 
    PerIOData mIODataSend;   //��������ʱ�õ�
    volatile bool mIsSending; //�Ƿ���Ͷ������ ����ط��������߳�ͬ���� ��Ϊ��ʼ������manager�����߳������ó�true�� ����ɶ˿��߳������ó�false���������������߳���ȡ��false��ɴ���ô�Ͷ����Ϣ��Ҳ������
    /************************************************************************/
    /* ������ĸ��ֶΣ�Ŀǰû����                                            */
    /************************************************************************/
    char receivedBuffer[MAXRECEIVEDBUFFLENGTH];  //�������ݵĻ���
    unsigned int receivedBufferLength;
    char sentBuffer[MAXRECEIVEDBUFFLENGTH]; //��������ʱ�� �Ƚ����ݻ��浽�⡣�൱��һ�����У� ���ǰ����������������ͣ��ͽ�������ӵ�ĩβ
    unsigned int sentBufferLength;
};

struct DataBufferKey
{
    int listenSocket; //�����׽��֣�����еĻ�
    int selfSocket; //����
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
typedef map<int, WrapSocket*> SocketMap; // SOCKET��ӳ��


#define  IOCPMgr MyIOCP::Instance()

// socketΪ�׽��ֵ�ID, data�ǽ��յ������ݣ� datalength�ǽ��յ������ݳ��ȣ� ����Ϣ���յ�ʱ�� �ʹ�������ע�⣺�������ֻ�������߳��б�����
// ע�� dataָ���Ǹ��ֲ��ģ� ���Ҫ�������ݣ����Լ�����һ���ڴ�
typedef void (*processFun)(int socket, char* data, unsigned int dataLength); 
typedef map<int, processFun> FunMap; //��Ӧsocket�Ĵ�����
class MyIOCP
{
public:
    static MyIOCP* Instance();

    // ��ʼ������, ������ʾÿ������Socket���Ĵ�����׼�����ӵ�remoteSocket����
    BOOL SetInit(unsigned int maxAcceptExSocket = 50); 

    void Update(); //����ÿ֡����

    BOOL Send(int socket, char* message, unsigned int messageLength); //���������Զ������������Ϣ�Ľӿ�

public:// ��Ϊ����˵Ĳ���

    // ���һ������socket, funΪ�䴦������ע�⺯���е��߳�ͬ������
    BOOL AddListenerSocket(WrapSocket* listen, processFun fun); 


public:// ��Ϊ�ͻ���Ӧ�еĲ���
    BOOL ConnectToRemoteServer(WrapSocket* listen, processFun fun); // ��������һ��������

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

    //��ʼ����
    BOOL StartListen(WrapSocket* listenSocket);

    // Ԥ�ȴ��������ӵ�socket
    BOOL PostAcceptEx(SOCKET listenSocket);

    BOOL PostReceive(WrapSocket* wrapSocket);

    //���׽�������ɶ˿ڹ�������
    BOOL AssociateDeviceWithCompletionPort(WrapSocket* socket);

    static DWORD WINAPI WorkerThread(LPVOID lpParam); //�̺߳���

    void Execute(); //ִ����ɶ˿���Ϣ��Ӧ

private:
    // ��������ϢͶ�ݵĸ�������
    void Receive(WrapSocket* socket); //����ɶ˿��л�ȡ����Ϣ
    void DispatchAllReceivedData(); //������Ľ��յ�������ȫ��Ͷ�ݳ�ȥ
    void PostSendAllBufferedData(); //������������͵�����ȫ��send

private: //��������������������Ӧ��map

    enum WRAPESOCKETTYPE 
    {
        E_CONNECTED, //�������ϵ�
        E_LISTENER,  //������
        E_READYTOBECONNECTED, //�ȴ������ӵ�

    };

    //ר��Ϊ�˻��һ��WrapSocket���ͻ������ݣ��˴�����ͬ����
    BOOL GetSocketForSendOrRecvData(int socket, WrapSocket*& wrapSocket);
    
    //��һ��socket���������õ���Ӧ��WrapSocket ��  ȡ���� Ҫ�Լ�����delete //   ע�⣬ ȡ��E_LISTENER���͵��׽���ʱ�� Ҫ�ǵ�ɾ�����Ӧ�Ĵ�����ӳ��processFun
    WrapSocket* GetSocketAndEraseFromTheMap(int socket, WRAPESOCKETTYPE type);

    //��һ��WrapSocket���룬���ڹ���
    void InsertWrapSocketToMap(WrapSocket* socket, WRAPESOCKETTYPE type);
    

    LPFN_ACCEPTEX     mlpfnAcceptEx;         // AcceptEx����ָ��
    LPFN_GETACCEPTEXSOCKADDRS  mlpfnGetAcceptExSockAddrs; //AcceptEx�����׺���ָ��
    unsigned int mMaxAcceptExSocket; //ÿ������socket����м��������õ�socket�ڵȴ�������
    HANDLE mCompletionPort; //��ɶ˿ھ��

    FunMap mFuns; //ÿ��socket����Ϣ������

    /************************************************************************/
    /* ����ÿ�������ķ��ʣ����������߳�ͬ��������                             */
    /************************************************************************/
    CRITICAL_SECTION mCriticalSetionOfReceivedData;
    MIDDLEDATABUFFERMAP mReceivedDataMap; //ÿ��socket���յ�����Ϣ���ݻ�����

    CRITICAL_SECTION mCriticalSetionOfSentData;
    MIDDLEDATABUFFERMAP mSentDataMap; //ÿ��socketҪ���͵���Ϣ���ݻ�����

    CRITICAL_SECTION mCriticalSetionOfConnectedSockets;
    SocketMap mConnectedSockets; // �Ѿ������ϵ�SOCKET

    CRITICAL_SECTION mCriticalSetionOfListenerSockets;
    SocketMap mListenerSockets;  // ����SOCKET

    CRITICAL_SECTION mCriticalSetionOfReadyToBeConnected;
    SocketMap mReadyToBeConnected; // ͨ��AcceptEx�����õĵȴ������ӵ�socket, key��ʾ��Ӧ�ļ���socket��intֵ

    
};


_CDH_END