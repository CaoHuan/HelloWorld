#include <functional>
#include <iostream>

#include "MyIOCP.h"

using namespace std;

_CDH_BEGIN

    bool operator<(const DataBufferKey& left, const DataBufferKey& right)
{
    if (left.listenSocket < right.listenSocket)
    {
        return true;
    }
    else if(left.listenSocket > right.listenSocket)
    {
        return false;
    }
    else
    {
        if (left.selfSocket < right.selfSocket)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}

MyIOCP* MyIOCP::Instance()
{
    static MyIOCP iocp;

    return &iocp;
}

BOOL MyIOCP::AddListenerSocket(WrapSocket* listen, processFun fun)
{
    if (listen != nullptr && listen->GetSocket() != INVALID_SOCKET)
    {
        if (StartListen(listen))
        {
            //SOCKET ��ʵ�Ǹ�int�� ���Կ�����Ϊ��ֱ�Ӳ��뵽map��
            mListenerSockets.insert(make_pair(listen->GetSocket(), listen));
            mFuns.insert(make_pair(listen->GetSocket(), fun));

            return TRUE;
        }
    }

    return FALSE;
}

BOOL MyIOCP::StartListen(WrapSocket* listenSocket)
{
    if (listenSocket == nullptr)
    {
        return FALSE;
    }
    if (mlpfnAcceptEx == nullptr || mlpfnGetAcceptExSockAddrs == nullptr) //�õ�AcceptEx�����ĵ�ַ,�������׵ĺ�����ַ
    {
        GUID GuidAcceptEx = WSAID_ACCEPTEX;        // GUID�������ʶ��AcceptEx���������
        DWORD dwBytes = 0;  

        WSAIoctl(
            listenSocket->GetSocket(), 
            SIO_GET_EXTENSION_FUNCTION_POINTER, 
            &GuidAcceptEx, 
            sizeof(GuidAcceptEx), 
            &mlpfnAcceptEx, 
            sizeof(mlpfnAcceptEx), 
            &dwBytes, 
            NULL, 
            NULL);

        GUID GuidGetAcceptExSockAddrs = WSAID_GETACCEPTEXSOCKADDRS;
        WSAIoctl(
            listenSocket->GetSocket(), 
            SIO_GET_EXTENSION_FUNCTION_POINTER, 
            &GuidGetAcceptExSockAddrs,
            sizeof(GuidGetAcceptExSockAddrs), 
            &mlpfnGetAcceptExSockAddrs, 
            sizeof(mlpfnGetAcceptExSockAddrs),   
            &dwBytes, 
            NULL, 
            NULL);
    }
    if (mlpfnAcceptEx == nullptr || mlpfnGetAcceptExSockAddrs == nullptr)
    {
        cout << "Cant get AcceptEx address" << endl;
        return FALSE;
    }

    unsigned int port = 0;
    if (listenSocket->GetListenPort(port))
    {
        SOCKADDR_IN internetAddr = {0};
        internetAddr.sin_family = PF_INET;
        internetAddr.sin_port = htons(port);
        internetAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        if (::bind(listenSocket->GetSocket(), (sockaddr*)&internetAddr, sizeof(internetAddr)) < 0)
        {
            cout << "Bind Error" << endl;
            return false;
        }

        // ����
        if (listen(listenSocket->GetSocket(), SOMAXCONN) < 0)
        {
            cout << "listen failed !!!!" << endl;
            return FALSE;
        }
        // �˴�Ҫ��ͬ������
        InsertWrapSocketToMap(listenSocket, WRAPESOCKETTYPE::E_LISTENER);

        listenSocket->GetPerHandleData().selfSocket = listenSocket->GetSocket();
        if (!AssociateDeviceWithCompletionPort(listenSocket))
        {
            cout << "Associate IOCP failed������" << endl;
            return FALSE;
        }

        //����mMaxAcceptExSocket��socket�Ա��ͻ�������
        unsigned int acceptExSockets = 0;
        while (acceptExSockets < mMaxAcceptExSocket)
        {
            if (!PostAcceptEx(listenSocket->GetSocket()))
            {
                cout << "AcceptEX failed!!" << endl;
                return FALSE;
            }
            ++acceptExSockets;
        }
    }

    return TRUE;
}

BOOL MyIOCP::PostAcceptEx(SOCKET listenSocket)
{
    //     if (listenSocket == nullptr)
    //     {
    //         return FALSE;
    //     }
    WrapSocket* socket = new WrapSocket();
    if (socket == nullptr)
    {
        cout << "out of memory !!!" << endl;
        return FALSE;
    }

    socket->GetPerIODataReceive().operationType = IOCP_OPERATION_TYPE::E_ACCEPT;
    socket->GetPerIODataReceive().remoteSocket = socket->GetSocket(); //����ɶ˿���Ϣ�У����Լ��ӽ�ȥ���Ա������ж����ĸ�socket
    socket->GetPerHandleData().selfSocket = socket->GetSocket();
    socket->GetPerHandleData().hasListenSocket = listenSocket;
    mlpfnAcceptEx(listenSocket, socket->GetSocket(), socket->GetPerIODataReceive().buffer, 0,
        sizeof(SOCKADDR_IN)+ 16, sizeof(SOCKADDR_IN) +16, nullptr, (LPOVERLAPPED)&(socket->GetPerIODataReceive()));

    // ���봴���õ��б��������������������˴�Ҫ���߳�ͬ��
    InsertWrapSocketToMap(socket, MyIOCP::WRAPESOCKETTYPE::E_READYTOBECONNECTED);
    return TRUE;

}

BOOL MyIOCP::PostReceive(WrapSocket* wrapSocket)
{
    if (wrapSocket == nullptr)
    {
        return FALSE;
    }


    memset(&wrapSocket->GetPerIODataReceive(), 0, sizeof(wrapSocket->GetPerIODataReceive()));
    wrapSocket->GetPerIODataReceive().remoteSocket = wrapSocket->GetSocket();


    DWORD flags = 0;
    DWORD recBytes = 0;

    wrapSocket->GetPerIODataReceive().dataBuf.buf = wrapSocket->GetPerIODataReceive().buffer;
    wrapSocket->GetPerIODataReceive().dataBuf.len = sizeof(wrapSocket->GetPerIODataReceive().buffer);
    wrapSocket->GetPerIODataReceive().operationType = IOCP_OPERATION_TYPE::E_RECV;

    WSARecv(wrapSocket->GetSocket(), &(wrapSocket->GetPerIODataReceive().dataBuf), 1, &recBytes, &flags, &(wrapSocket->GetPerIODataReceive().overlapped), NULL);

    return TRUE;
}


void MyIOCP::Receive(WrapSocket* socket)
{
    EnterCriticalSection(&mCriticalSetionOfReceivedData);
    MIDDLEDATABUFFERMAP::iterator iter(mReceivedDataMap.find(DataBufferKey(socket->GetPerHandleData().hasListenSocket, socket->GetPerHandleData().selfSocket)));
    if (iter != mReceivedDataMap.end())
    {
        MiddleDataBuffer* midBuff = iter->second;
        if (socket->GetPerIODataReceive().bufferLen > 0 && midBuff->bufferLength + socket->GetPerIODataReceive().bufferLen < MAXRECEIVEDBUFFLENGTH)
        {
            memcpy(&midBuff->buffer[midBuff->bufferLength], socket->GetPerIODataReceive().buffer, socket->GetPerIODataReceive().bufferLen);
            midBuff->bufferLength += socket->GetPerIODataReceive().bufferLen;
        }
        if (midBuff->bufferLength + socket->GetPerIODataReceive().bufferLen > MAXRECEIVEDBUFFLENGTH)
        {
            cout << "Out of buff" << endl;
        }
    }
    LeaveCriticalSection(&mCriticalSetionOfReceivedData);

    PostReceive(socket);

}

BOOL MyIOCP::AssociateDeviceWithCompletionPort(WrapSocket* socket)
{
    HANDLE h = CreateIoCompletionPort((HANDLE)socket->GetSocket(), mCompletionPort, (DWORD)&(socket->GetPerHandleData()), 0);

    return (h == mCompletionPort);
}

DWORD WINAPI MyIOCP::WorkerThread(LPVOID lpParam)
{
    MyIOCP* pMyIOCP =(MyIOCP*)lpParam;

    pMyIOCP->Execute();  //��ʼ������ɶ˿ڶ��еĺ���

    return 0;
}
void MyIOCP::Execute()
{
    PerHandleData* pPerHandleData = nullptr;
    PerIOData* pPerIOData = nullptr;
    LPOVERLAPPED lpOverLapped = nullptr;
    DWORD byteTransferred;
    BOOL bRet = FALSE;

    while (true)
    {
        bRet = GetQueuedCompletionStatus(mCompletionPort,
            &byteTransferred,
            (PULONG_PTR)&pPerHandleData,
            /*(LPOVERLAPPED*)&pPerIOData*/&lpOverLapped,
            INFINITE);
        pPerIOData = (PerIOData*)CONTAINING_RECORD(lpOverLapped, PerIOData, overlapped);
        if (bRet == FALSE)
        {
            if (pPerIOData == nullptr)
            {
                // �˳��̡߳�������Ҫ���ˡ���
                break;
            }
            // �Է������Ͽ���

            continue;
        }

        // ����ɹ�����ɶ˿�����
        switch (pPerIOData->operationType)
        {
        case CDH::E_INVALID:
            {
            }
            break;
        case CDH::E_ACCEPT:
            {
                /************************************************************************/
                /* 
                inet_ntoa(ClientAddr->sin_addr) �ǿͻ���IP��ַ

                ntohs(ClientAddr->sin_port) �ǿͻ�������Ķ˿�

                inet_ntoa(LocalAddr ->sin_addr) �Ǳ���IP��ַ

                ntohs(LocalAddr ->sin_port) �Ǳ���ͨѶ�Ķ˿�

                pIoContext->m_wsaBuf.buf �Ǵ洢�ͻ��˷�����һ�����ݵĻ�����
                */
                /************************************************************************/
                SOCKADDR_IN* ClientAddr = NULL;
                SOCKADDR_IN* LocalAddr = NULL; 
                int remoteLen = sizeof(SOCKADDR_IN), localLen = sizeof(SOCKADDR_IN); 
                mlpfnGetAcceptExSockAddrs(pPerIOData->buffer, 0,  sizeof(SOCKADDR_IN)+16, sizeof(SOCKADDR_IN)+16, (LPSOCKADDR*)&LocalAddr, &localLen, (LPSOCKADDR*)&ClientAddr, &remoteLen);

                WrapSocket* connectedSocket = GetSocketAndEraseFromTheMap(pPerIOData->remoteSocket, WRAPESOCKETTYPE::E_READYTOBECONNECTED);
                if (connectedSocket == nullptr)
                {
                    closesocket(pPerIOData->remoteSocket);
                    continue;
                }
                if (!AssociateDeviceWithCompletionPort(connectedSocket))
                {
                    closesocket(pPerIOData->remoteSocket);
                    continue;
                }
                // ��ӵ��Ѿ������б�
                InsertWrapSocketToMap(connectedSocket, WRAPESOCKETTYPE::E_CONNECTED);

                PostReceive(connectedSocket);

                // ���������һ��׼�����˵�socket, ����������һ��socket�����ӡ�
                PostAcceptEx(connectedSocket->GetPerHandleData().hasListenSocket);
            }
            break;
        case CDH::E_RECV:
            {
                int socket = pPerHandleData->selfSocket;
                WrapSocket* wrapSocket = nullptr;
                GetSocketForSendOrRecvData(socket, wrapSocket);

                if (wrapSocket != nullptr)
                {
                    wrapSocket->GetPerIODataReceive().bufferLen = byteTransferred;
                    Receive(wrapSocket);
                }
            }
            break;
        case CDH::E_SEND:
            {
                int socket = pPerHandleData->selfSocket;
                WrapSocket* wrapSocket = nullptr;
                GetSocketForSendOrRecvData(socket, wrapSocket);
                if (wrapSocket != nullptr)
                {
                    wrapSocket->SendingData(false);
                }

            }
            break;
        case CDH::E_CONNECT:
            break;
        default:
            break;
        }

    }
}

BOOL MyIOCP::SetInit(unsigned int maxAcceptExSocket)
{
    mMaxAcceptExSocket = maxAcceptExSocket;

    WSADATA wsd;
    SYSTEM_INFO SystemInfo;

    // ����WinSock2.2
    WSAStartup(MAKEWORD(2,2), &wsd);

    // ����һ��I/O ��ɶ˿�
    mCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    if (mCompletionPort == nullptr)
    {
        return FALSE;
    }

    // ȷ��ϵͳ���ж��ٸ�������
    GetSystemInfo(&SystemInfo);
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    // ����ϵͳ�п��õĴ��������������������߳�
    for (int i = 0; i < int(SystemInfo.dwNumberOfProcessors * 2); ++i)
    {
        // ����һ���������Ĺ������̣߳�������ɶ˿ڴ��ݵ����߳�
        UINT dw = 0;
        CloseHandle(chBEGINTHREADEX(0, 0, MyIOCP::WorkerThread, 
            (void*)this, 0, &dw));
    }

    return true;
}

void MyIOCP::Update()
{
    DispatchAllReceivedData();
    PostSendAllBufferedData();

    //��Ҫ�Ͽ�������ȫ�������
}

BOOL MyIOCP::Send(int socket, char* message, unsigned int messageLength)
{
    BOOL result = FALSE;
    WrapSocket* wrapSocket = nullptr;
    if (!GetSocketForSendOrRecvData(socket, wrapSocket) || wrapSocket == nullptr)
    {
        return FALSE;
    }

    EnterCriticalSection(&mCriticalSetionOfSentData);
    MIDDLEDATABUFFERMAP::iterator iter(mSentDataMap.find(DataBufferKey(wrapSocket->GetPerHandleData().hasListenSocket, wrapSocket->GetPerHandleData().selfSocket)));
    if (iter != mSentDataMap.end())
    {
        MiddleDataBuffer* midBuff = iter->second;
        if (midBuff->bufferLength + messageLength < MAXRECEIVEDBUFFLENGTH)
        {
            memcpy(midBuff->buffer + midBuff->bufferLength, message, messageLength);
            midBuff->bufferLength += messageLength;
            result = TRUE;
        }
        else
        {
            //����������������
            result = FALSE;
        }
       
    }
    LeaveCriticalSection(&mCriticalSetionOfSentData);

    return result;
}

void MyIOCP::PostSendAllBufferedData()
{
    //Ϊ�˲������̣߳� ���������е�����һ����ȫ��ȡ����Ȼ����һ�ݸ����� ���в����� ע�⣬ ��dataMap�����е������������ڶ����ڴ˺����ڵ�
    MIDDLEDATABUFFERMAP dataMap;
    EnterCriticalSection(&mCriticalSetionOfSentData);
    MIDDLEDATABUFFERMAP::iterator dataIter(mSentDataMap.begin());
    while (dataIter != mSentDataMap.end())
    {
        MiddleDataBuffer buffer = {0};
        memcpy(&buffer, dataIter->second, sizeof(MiddleDataBuffer));
        dataMap.insert(make_pair(dataIter->first, &buffer));
        memset(dataIter->second, 0, sizeof(MiddleDataBuffer));
        ++dataIter;
    }
    LeaveCriticalSection(&mCriticalSetionOfSentData);

    //��������
    MIDDLEDATABUFFERMAP::iterator iter(dataMap.begin());
    WrapSocket* socket = nullptr;
    while (iter != dataMap.end())
    { 
        if (iter->second->bufferLength > 0)
        {
            if (GetSocketForSendOrRecvData(iter->first.selfSocket, socket))
            {
                // ������ܷ��ͣ� ��������һ�ε���
                if (socket->CanSendData())
                {
                    socket->SendingData(true);
                    ZeroMemory(&(socket->GetPerIODataSend()), sizeof(PerIOData));

                    memcpy(socket->GetPerIODataSend().buffer, iter->second->buffer, iter->second->bufferLength);
                    socket->GetPerIODataSend().bufferLen = iter->second->bufferLength;
                    memset(iter->second, 0, sizeof(MiddleDataBuffer));

                    socket->GetPerIODataSend().remoteSocket = socket->GetSocket();
                    socket->GetPerIODataSend().operationType = IOCP_OPERATION_TYPE::E_SEND;
                    socket->GetPerIODataSend().dataBuf.buf = socket->GetPerIODataSend().buffer;
                    socket->GetPerIODataSend().dataBuf.len = socket->GetPerIODataSend().bufferLen;
                    DWORD numBytesTransferd = 0;
                    WSASend(socket->GetSocket(), &(socket->GetPerIODataSend().dataBuf), 1, &numBytesTransferd, 0, &(socket->GetPerIODataSend().overlapped), nullptr );
                }
            }
        }

        ++iter;
    }

}

void MyIOCP::DispatchAllReceivedData()
{
    // Ϊ�˲������̣߳� ���������е�����һ����ȫ��ȡ����Ȼ����һ�ݸ����� ���в����� ע�⣬ ��dataMap�����е������������ڶ����ڴ˺����ڵ�
    MIDDLEDATABUFFERMAP dataMap;
    EnterCriticalSection(&mCriticalSetionOfReceivedData);
    MIDDLEDATABUFFERMAP::iterator dataIter(mReceivedDataMap.begin());
    while (dataIter != mReceivedDataMap.end())
    {
        if (dataIter->second->bufferLength > 0)
        {
            MiddleDataBuffer buffer = {0};
            memcpy(&buffer, dataIter->second, sizeof(MiddleDataBuffer));
            dataMap.insert(make_pair(dataIter->first, &buffer));
            memset(dataIter->second, 0, sizeof(MiddleDataBuffer));
        }

        ++dataIter;
    }
    LeaveCriticalSection(&mCriticalSetionOfReceivedData);

    // ����Ϣȫ���ɷ���ȥ
    MIDDLEDATABUFFERMAP::iterator iter(dataMap.begin());
    while (iter != dataMap.end())
    {
        FunMap::iterator iterFun = mFuns.find(iter->first.listenSocket); //�ҵ���������Ǹ��׽��ֵĺ���
        if (iterFun != mFuns.end())
        {
            iterFun->second(iter->first.selfSocket, iter->second->buffer, iter->second->bufferLength); //����
        }
        ++iter;
    }
}


#pragma region private method

BOOL MyIOCP::GetSocketForSendOrRecvData(int socket, WrapSocket*& wrapSocket)
{
    //Ϊ�˷��ͻ������ݣ� ����һ�������Ӻõ�
    SocketMap::iterator finder = mConnectedSockets.find(socket);
    if (finder != mConnectedSockets.end())
    {
        wrapSocket = finder->second;
        return TRUE;
    }
    return FALSE;
}

WrapSocket* MyIOCP::GetSocketAndEraseFromTheMap(int socket, WRAPESOCKETTYPE type)
{
    WrapSocket* result = nullptr;
    SocketMap::iterator finder;
    switch (type)
    {
    case CDH::MyIOCP::E_CONNECTED:
        EnterCriticalSection(&mCriticalSetionOfConnectedSockets);
        finder = mConnectedSockets.find(socket);
        if (finder != mConnectedSockets.end())
        {
            result = finder->second;
            mConnectedSockets.erase(finder);

            //�������Ӷ�����ȡ��������ֱ�ӽ�δ���������ɾ��
            EnterCriticalSection(&mCriticalSetionOfReceivedData);
            MIDDLEDATABUFFERMAP::iterator iter(mReceivedDataMap.find(DataBufferKey(result->GetPerHandleData().hasListenSocket, result->GetPerHandleData().selfSocket)));
            if (iter != mReceivedDataMap.end())
            {
                delete iter->second;
                mReceivedDataMap.erase(iter);
            }
            LeaveCriticalSection(&mCriticalSetionOfReceivedData);

            EnterCriticalSection(&mCriticalSetionOfSentData);
            MIDDLEDATABUFFERMAP::iterator iterSent = mSentDataMap.find(DataBufferKey(result->GetPerHandleData().hasListenSocket, result->GetPerHandleData().selfSocket));
            if (iterSent != mSentDataMap.end())
            {
                delete iterSent->second;
                mSentDataMap.erase(iterSent);
            }
            LeaveCriticalSection(&mCriticalSetionOfSentData);

        }
        LeaveCriticalSection(&mCriticalSetionOfConnectedSockets);
        break;
    case CDH::MyIOCP::E_LISTENER:
        EnterCriticalSection(&mCriticalSetionOfListenerSockets);
        finder = mListenerSockets.find(socket);
        if (finder != mListenerSockets.end())
        {
            result = finder->second;
            mListenerSockets.erase(finder);
        }
        LeaveCriticalSection(&mCriticalSetionOfListenerSockets);
        break;
    case CDH::MyIOCP::E_READYTOBECONNECTED:
        EnterCriticalSection(&mCriticalSetionOfReadyToBeConnected);
        finder = mReadyToBeConnected.find(socket);
        if (finder != mReadyToBeConnected.end())
        {
            result = finder->second;
            mReadyToBeConnected.erase(finder);
        }
        LeaveCriticalSection(&mCriticalSetionOfReadyToBeConnected);
        break;
    default:
        break;
    }
    return result;
}

void MyIOCP::InsertWrapSocketToMap(WrapSocket* socket, WRAPESOCKETTYPE type)
{ //TODO:���ʱ��Ҫ�������ɶ˿���ȥ��
    if (socket == nullptr)
    {
        return;
    }
    switch (type)
    {
    case CDH::MyIOCP::E_CONNECTED:
        {

            EnterCriticalSection(&mCriticalSetionOfConnectedSockets);

            mConnectedSockets.insert(make_pair(socket->GetSocket(), socket));

            //Ϊ�����ϵ�socket�����Ϣ������

            EnterCriticalSection(&mCriticalSetionOfReceivedData);
            MiddleDataBuffer* receivedBuff = new MiddleDataBuffer;
            memset(receivedBuff, 0, sizeof(MiddleDataBuffer));
            mReceivedDataMap.insert(make_pair(DataBufferKey(socket->GetPerHandleData().hasListenSocket, socket->GetPerHandleData().selfSocket), receivedBuff));
            LeaveCriticalSection(&mCriticalSetionOfReceivedData);

            EnterCriticalSection(&mCriticalSetionOfSentData);
            MiddleDataBuffer* sentBuff = new MiddleDataBuffer;
            memset(sentBuff, 0, sizeof(MiddleDataBuffer));
            mSentDataMap.insert(make_pair(DataBufferKey(socket->GetPerHandleData().hasListenSocket, socket->GetPerHandleData().selfSocket), sentBuff));
            LeaveCriticalSection(&mCriticalSetionOfSentData);

            LeaveCriticalSection(&mCriticalSetionOfConnectedSockets);
        }

        break;
    case CDH::MyIOCP::E_LISTENER:
        {

            EnterCriticalSection(&mCriticalSetionOfListenerSockets);

            mListenerSockets.insert(make_pair(socket->GetSocket(), socket));

            LeaveCriticalSection(&mCriticalSetionOfListenerSockets);
        }
        break;
    case CDH::MyIOCP::E_READYTOBECONNECTED:
        {

            EnterCriticalSection(&mCriticalSetionOfReadyToBeConnected);

            mReadyToBeConnected.insert(make_pair(socket->GetSocket(), socket));

            LeaveCriticalSection(&mCriticalSetionOfReadyToBeConnected);
        }
        break;
    default:
        break;
    }
}

#pragma endregion private method


_CDH_END