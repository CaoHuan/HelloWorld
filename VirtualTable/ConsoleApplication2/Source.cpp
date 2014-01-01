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
LPFN_ACCEPTEX     m_lpfnAcceptEx;         // AcceptEx����ָ��
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

// ��I/O��������
typedef struct tagPER_IO_DATA
{
    OVERLAPPED Overlapped;
    WSABUF DataBuf;
    SOCKET newSocket;
    unsigned int listener;
    char buffer[1024];
    int BufferLen;
    int OperationType; // ������Ϊ��д��ʶ
}PER_IO_DATA, *LPPER_IO_DATA;

//�̺߳���
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


        // ���ɹ��ķ��أ� ���Ҫע��ʹ�������CONTAINING_RECORD
        PerIoData = (LPPER_IO_DATA)CONTAINING_RECORD(lpOverlapped, PER_IO_DATA, Overlapped);
        cout << PerIoData->DataBuf.buf << ::GetCurrentThreadId() << endl;
        // �ȼ��һ�£��������׽������Ƿ��д�����
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

            //PER_IO_CONTEXT* pIoContext = ����ͨ���õ�I/O Context

                SOCKADDR_IN* ClientAddr = NULL;
            SOCKADDR_IN* LocalAddr = NULL;  
            int remoteLen = sizeof(SOCKADDR_IN), localLen = sizeof(SOCKADDR_IN);  

            m_lpfnGetAcceptExSockAddrs(PerIoData->buffer, 0,  sizeof(SOCKADDR_IN)+16, sizeof(SOCKADDR_IN)+16, (LPSOCKADDR*)&LocalAddr, &localLen, (LPSOCKADDR*)&ClientAddr, &remoteLen);

            /************************************************************************/
            /* 
            inet_ntoa(ClientAddr->sin_addr) �ǿͻ���IP��ַ

            ntohs(ClientAddr->sin_port) �ǿͻ�������Ķ˿�

            inet_ntoa(LocalAddr ->sin_addr) �Ǳ���IP��ַ

            ntohs(LocalAddr ->sin_port) �Ǳ���ͨѶ�Ķ˿�

            pIoContext->m_wsaBuf.buf �Ǵ洢�ͻ��˷�����һ�����ݵĻ�����
            
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
            // ��������
            char sendBuf[1000];
            sprintf_s(sendBuf, "Welcome %s to %d %d \n", PerIoData->DataBuf.buf, PerHandleData->Socket, (PerHandleData->Socket), ::GetCurrentThreadId());

            send(PerHandleData->Socket, sendBuf, strlen(sendBuf) + 1, 0);

            // WSASend()

            // �ɹ��ˣ����� �յ������Կͻ��˵�����
            cout << PerIoData->DataBuf.buf << ::GetCurrentThreadId() << endl;
            Flags = 0;

            // Ϊ��һ���ص����ü�����I/O��������
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


    // ����WinSock2.2
    WSAStartup(MAKEWORD(2,2), &wsd);

    // 1.����һ��I/O ��ɶ˿�
    CompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

    // 2.ȷ��ϵͳ���ж��ٸ�������
    GetSystemInfo(&SystemInfo);
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    // 3.����ϵͳ�п��õĴ��������������������߳�
    for (int i = 0; i < int(SystemInfo.dwNumberOfProcessors * 2); ++i)
    {
        HANDLE ThreadHandle;

        // ����һ���������Ĺ������̣߳�������ɶ˿ڴ��ݵ����߳�
        UINT dw = 0;
        CloseHandle((HANDLE)_beginthreadex(0, 0, (PTHREAD_START)ServerWorkerThread, 
            CompletionPort, 0, &dw));
    }

    // 4.����һ�������׽��֣�һ�µ���·���ǹ̶���
    Listen = WSASocket(AF_INET,SOCK_STREAM,0,NULL,0,WSA_FLAG_OVERLAPPED);

    //�󶨺ͼ���
    InternetAddr.sin_family = PF_INET;
    InternetAddr.sin_port = htons(6000);
    InternetAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(Listen, (SOCKADDR*)&InternetAddr, sizeof(InternetAddr));

    listen(Listen, 5);
    setsockopt(Listen,SOL_SOCKET,SO_UPDATE_ACCEPT_CONTEXT,NULL, 0 );
    //����ѭ��
    bool b = true;



    GUID GuidAcceptEx = WSAID_ACCEPTEX;        // GUID�������ʶ��AcceptEx���������
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
    // 5.�������ӣ���������ɶ˿ڣ����������AcceptEx�����棬 �Դ�����������WinsockӦ�ó���
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
    //         //6.�����������׽��ֹ����ĵ����������Ϣ�ṹ
    PerHandleData = (LPPER_HANDLE_DATA)GlobalAlloc(GPTR, sizeof(PER_HANDLE_DATA));
    PerIoData->listener = Listen;
    PerIoData->newSocket = Accept;
    bool ret = m_lpfnAcceptEx(Listen,  PerIoData->newSocket, PerIoData->buffer, 0, sizeof (SOCKADDR_IN) + 16, sizeof (SOCKADDR_IN) + 16,
        &dw, &(PerIoData->Overlapped));

   
    PerHandleData->Socket = Accept;
    memcpy(&PerHandleData->ClientAddr, &saRemote, RemoteLen);

    //7.�������׽��ֺ���ɶ˿ڹ�������
    CreateIoCompletionPort((HANDLE)Listen, CompletionPort, (DWORD)PerHandleData, 0);

    //CreateIoCompletionPort((HANDLE)Accept, CompletionPort, (DWORD)PerHandleData, 0);
    if (CompletionPort == nullptr)
    {
    }
    // ��ʼ�ڽ����׽����ϴ���I/O
    // ʹ���ص�I/O���ƣ����½����׽�����Ͷ��һ�������첽
    // WSARecv �� WSASend������ЩI/O ������ɺ󣬹������̻߳�ΪI/O�����ṩ���� ֮��Ϳ����������
    static int const DATA_BUFSIZE = 4096;

    DWORD RecBytes = 0;

    DWORD Flags = 0;


    Sleep(INFINITE);
    return 0;

}
