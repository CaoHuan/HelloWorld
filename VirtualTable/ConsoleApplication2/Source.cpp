// #include <windows.h>
// #include <process.h>
// #include <iostream>
// using namespace std;
// 
// unsigned WINAPI ThreadFunc(PVOID pvParam) {
//     while (1)
//     {
//         cout << " This is a test" << endl;
//     }
// 
//     return 0;
// }
// 
// 
// int main()
// {
//     UINT dword = 0;
//     HANDLE thread =(HANDLE) _beginthreadex(nullptr, 0, ThreadFunc, 0, 0, &dword);
// 
//     return 0;
// }


#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

#include <mswsock.h>
#include <iostream>
#include <process.h>
#include <fstream>
using namespace std;
LPFN_ACCEPTEX     m_lpfnAcceptEx;         // AcceptEx函数指针
LPFN_GETACCEPTEXSOCKADDRS  m_lpfnGetAcceptExSockAddrs;
SOCKET Listen;
typedef unsigned (__stdcall *PTHREAD_START) (void *);

#define chBEGINTHREADEX(psa, cbStackSize, pfnStartAddr, \
    pvParam, dwCreateFlags, pdwThreadId)                 \
    ((HANDLE)_beginthreadex(                          \
    (void *)        (psa),                         \
    (unsigned)      (cbStackSize),                 \
    (PTHREAD_START) (pfnStartAddr),                \
    (void *)        (pvParam),                     \
    (unsigned)      (dwCreateFlags),               \
    (unsigned *)    (pdwThreadId)))



typedef struct tagPER_HANDLE_DATA
{
    SOCKET Socket;
    SOCKADDR_STORAGE ClientAddr;
}PER_HANDLE_DATA, *LPPER_HANDLE_DATA;

// 单I/O操作数据
typedef struct tagPER_IO_DATA
{
    OVERLAPPED Overlapped;
    WSABUF DataBuf;
    SOCKET newSocket;
    unsigned int listener;
    char buffer[1024];
    int BufferLen;
    int OperationType; // 可以作为读写标识
}PER_IO_DATA, *LPPER_IO_DATA;

//线程函数
DWORD WINAPI ServerWorkerThread(LPVOID lpParam);

DWORD WINAPI ServerWorkerThread(LPVOID lpParam)
{
    HANDLE CompletionPort = (HANDLE)lpParam;
    DWORD BytesTransferred;
    LPOVERLAPPED lpOverlapped;
    LPPER_HANDLE_DATA PerHandleData = NULL;
    LPPER_IO_DATA PerIoData = NULL;
    DWORD RecvBytes;
    DWORD Flags;
    bool bRet = FALSE;

    while (TRUE)
    {
        bRet = GetQueuedCompletionStatus(CompletionPort, 
            &BytesTransferred, 
            (PULONG_PTR)&PerHandleData,
            (LPOVERLAPPED*)&lpOverlapped, 
            INFINITE);


        // 检查成功的返回， 这儿要注意使用这个宏CONTAINING_RECORD
        PerIoData = (LPPER_IO_DATA)CONTAINING_RECORD(lpOverlapped, PER_IO_DATA, Overlapped);
        cout << PerIoData->DataBuf.buf << ::GetCurrentThreadId() << endl;
        // 先检查一下，看看在套接字上是否有错误发生
        if (0 == BytesTransferred && lpOverlapped == NULL)
        {
            closesocket(PerHandleData->Socket);
            GlobalFree(PerHandleData);
            GlobalFree(PerIoData);
            continue;
        }
        SOCKET newSock = NULL;
        if (PerIoData->OperationType == 1)
        {

            //PER_IO_CONTEXT* pIoContext = 本次通信用的I/O Context

                SOCKADDR_IN* ClientAddr = NULL;
            SOCKADDR_IN* LocalAddr = NULL;  
            int remoteLen = sizeof(SOCKADDR_IN), localLen = sizeof(SOCKADDR_IN);  

            m_lpfnGetAcceptExSockAddrs(PerIoData->buffer, 0,  sizeof(SOCKADDR_IN)+16, sizeof(SOCKADDR_IN)+16, (LPSOCKADDR*)&LocalAddr, &localLen, (LPSOCKADDR*)&ClientAddr, &remoteLen);

            /************************************************************************/
            /* 
            inet_ntoa(ClientAddr->sin_addr) 是客户端IP地址

            ntohs(ClientAddr->sin_port) 是客户端连入的端口

            inet_ntoa(LocalAddr ->sin_addr) 是本地IP地址

            ntohs(LocalAddr ->sin_port) 是本地通讯的端口

            pIoContext->m_wsaBuf.buf 是存储客户端发来第一组数据的缓冲区
            
            */
            /************************************************************************/
            PerIoData->OperationType = 0; // read

            CreateIoCompletionPort((HANDLE)  PerHandleData->Socket, CompletionPort, (u_long) PerHandleData, 0);

            char sendBuf[1000];
            SOCKADDR address = {0};
            memcpy(&(address), PerIoData->buffer , sizeof(SOCKADDR_IN));
            sprintf_s(sendBuf, "Welcome %s to %d %d \n", inet_ntoa(ClientAddr->sin_addr), PerHandleData->Socket, (PerHandleData->Socket), ::GetCurrentThreadId());

            send(PerHandleData->Socket, sendBuf, strlen(sendBuf) + 1, 0);

            DWORD RecBytes = 0;

            DWORD Flags = 0;
             ZeroMemory(&(PerIoData->Overlapped), sizeof(OVERLAPPED));
            if (WSARecv(PerHandleData->Socket, &(PerIoData->DataBuf), 1, &RecvBytes, &Flags, &(PerIoData->Overlapped), NULL))
            {
                cout << "Success WSARecv" << endl;
            }
            else
            {
                cout << "Failed WSARECV" << endl;
            }


        }
        else
        {
            // 处理数据
            char sendBuf[1000];
            sprintf_s(sendBuf, "Welcome %s to %d %d \n", PerIoData->DataBuf.buf, PerHandleData->Socket, (PerHandleData->Socket), ::GetCurrentThreadId());

            send(PerHandleData->Socket, sendBuf, strlen(sendBuf) + 1, 0);

            // WSASend()

            // 成功了！！！ 收到了来自客户端的数据
            cout << PerIoData->DataBuf.buf << ::GetCurrentThreadId() << endl;
            Flags = 0;

            // 为下一个重叠调用简历单I/O操作数据
            ZeroMemory(PerIoData, sizeof(PER_IO_DATA));

            PerIoData->DataBuf.len = 1024;
            PerIoData->DataBuf.buf = PerIoData->buffer;
            PerIoData->OperationType = 0; // read



            WSARecv(PerHandleData->Socket, &(PerIoData->DataBuf), 1, &RecvBytes, &Flags, &(PerIoData->Overlapped), NULL);
        }
    }

    return 0;
}

typedef unsigned (__stdcall * PTHREAD_START) (void *);

int main()
{
    HANDLE CompletionPort;
    WSADATA wsd;
    SYSTEM_INFO SystemInfo;
    SOCKADDR_IN InternetAddr;


    // 加载WinSock2.2
    WSAStartup(MAKEWORD(2,2), &wsd);

    // 1.创建一个I/O 完成端口
    CompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

    // 2.确定系统中有多少个处理器
    GetSystemInfo(&SystemInfo);
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    // 3.基于系统中可用的处理器数量创建工作器线程
    for (int i = 0; i < int(SystemInfo.dwNumberOfProcessors * 2); ++i)
    {
        HANDLE ThreadHandle;

        // 创建一个服务器的工作器线程，并将完成端口传递到该线程
        UINT dw = 0;
        CloseHandle((HANDLE)_beginthreadex(0, 0, (PTHREAD_START)ServerWorkerThread, 
            CompletionPort, 0, &dw));
    }

    // 4.创建一个监听套接字，一下的套路都是固定的
    Listen = WSASocket(AF_INET,SOCK_STREAM,0,NULL,0,WSA_FLAG_OVERLAPPED);

    //绑定和监听
    InternetAddr.sin_family = PF_INET;
    InternetAddr.sin_port = htons(6000);
    InternetAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(Listen, (SOCKADDR*)&InternetAddr, sizeof(InternetAddr));

    listen(Listen, 5);
    setsockopt(Listen,SOL_SOCKET,SO_UPDATE_ACCEPT_CONTEXT,NULL, 0 );
    //无线循环
    bool b = true;



    GUID GuidAcceptEx = WSAID_ACCEPTEX;        // GUID，这个是识别AcceptEx函数必须的
    GUID GuidGetAcceptExSockAddrs = WSAID_GETACCEPTEXSOCKADDRS;
    DWORD dwBytes = 0;  

    WSAIoctl(
        Listen, 
        SIO_GET_EXTENSION_FUNCTION_POINTER, 
        &GuidAcceptEx, 
        sizeof(GuidAcceptEx), 
        &m_lpfnAcceptEx, 
        sizeof(m_lpfnAcceptEx), 
        &dwBytes, 
        NULL, 
        NULL);
    WSAIoctl(
        Listen, 
        SIO_GET_EXTENSION_FUNCTION_POINTER, 
        &GuidGetAcceptExSockAddrs,
        sizeof(GuidGetAcceptExSockAddrs), 
        &m_lpfnGetAcceptExSockAddrs, 
        sizeof(m_lpfnGetAcceptExSockAddrs),   
        &dwBytes, 
        NULL, 
        NULL);


    PER_HANDLE_DATA* PerHandleData = nullptr;
    SOCKADDR_IN saRemote;
    
    DWORD dw = 1;
    int RemoteLen;
    // 5.接收链接，并分配完成端口，这儿可以用AcceptEx来代替， 以创建可伸缩的Winsock应用程序
    RemoteLen = sizeof(saRemote);
    RemoteLen = sizeof(saRemote);
    //Accept = accept(Listen, (SOCKADDR*)&saRemote, &RemoteLen);

    SOCKET Accept;
   
    
     Accept = WSASocket(AF_INET,SOCK_STREAM,0,NULL,0,WSA_FLAG_OVERLAPPED);
    LPPER_IO_DATA PerIoData = nullptr;
    PerIoData = (LPPER_IO_DATA)GlobalAlloc(GPTR, sizeof(PER_IO_DATA));
    ZeroMemory(&(PerIoData->Overlapped), sizeof(OVERLAPPED));
    PerIoData->DataBuf.len = 1024;
    PerIoData->DataBuf.buf = PerIoData->buffer;
    PerIoData->OperationType = 1;
    //         //6.创建用来和套接字关联的单句柄数据信息结构
    PerHandleData = (LPPER_HANDLE_DATA)GlobalAlloc(GPTR, sizeof(PER_HANDLE_DATA));
    PerIoData->listener = Listen;
    PerIoData->newSocket = Accept;
    bool ret = m_lpfnAcceptEx(Listen,  PerIoData->newSocket, PerIoData->buffer, 0, sizeof (SOCKADDR_IN) + 16, sizeof (SOCKADDR_IN) + 16,
        &dw, &(PerIoData->Overlapped));

   
    PerHandleData->Socket = Accept;
    memcpy(&PerHandleData->ClientAddr, &saRemote, RemoteLen);

    //7.将接收套接字和完成端口关联起来
    CreateIoCompletionPort((HANDLE)Listen, CompletionPort, (DWORD)PerHandleData, 0);

    //CreateIoCompletionPort((HANDLE)Accept, CompletionPort, (DWORD)PerHandleData, 0);
    if (CompletionPort == nullptr)
    {
    }
    // 开始在接收套接字上处理I/O
    // 使用重叠I/O机制，在新建的套接字上投递一个或多个异步
    // WSARecv 或 WSASend请求。这些I/O 请求完成后，工作者线程会为I/O请求提供服务， 之后就可以坐享其成
    static int const DATA_BUFSIZE = 4096;

    DWORD RecBytes = 0;

    DWORD Flags = 0;


    Sleep(INFINITE);
    return 0;

}
