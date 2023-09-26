#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <iostream>
#include <ws2tcpip.h>
  
  
#pragma comment(lib, "ws2_32.lib")

int main()
{
    //winsock linking
    WSADATA wsadata;
    int wsaerr;
    WORD wVersionRequested = MAKEWORD(2, 2);
    wsaerr = WSAStartup(wVersionRequested,  &wsadata);
    if(wsaerr != 0)
    {
        std::cout << "Winsock dll not found..\n" ;
        return 0;
    }
    else {
        std::cout << "Winsock dll found \n";
        std::cout << "Status: " << wsadata.szSystemStatus << '\n';
    }
    //socket creation
    SOCKET clientScoket = INVALID_SOCKET;
    clientScoket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(clientScoket == INVALID_SOCKET)
    {
        std::cout << "Error at socket(): " << WSAGetLastError() << '\n';
        WSACleanup();
        return 0;
    }
    else {
        std::cout << "Socket() is OK!\n"; 
    }
    //socket setup continuation
    sockaddr_in clientService;
    clientService.sin_family = AF_INET;
    InetPton(AF_INET, ("127.0.0.1"), &clientService.sin_addr.s_addr);
    clientService.sin_port = 9999; // this is actually converted into 3879 because of big endian - little endian conversion
    // socket binding / connection
    if(connect(clientScoket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR)
    {
        std::cout << "Client connect() - Failed to connect\n"  << "err:"<< WSAGetLastError() << '\n';;
        WSACleanup();
        return 0;
    }
    else {
        std::cout << "Client connect() is OK! \n";
        std::cout << "Can start sending and recieveing data...\n";
        
        #define DEFAULT_BUFLEN 512

int recvbuflen = DEFAULT_BUFLEN;

const char *sendbuf = "this is a long test";
char recvbuf[DEFAULT_BUFLEN];

int iResult;

// Send an initial buffer
iResult = send(clientScoket, sendbuf, (int) strlen(sendbuf), 0);
if (iResult == SOCKET_ERROR) {
    printf("send failed: %d\n", WSAGetLastError());
    closesocket(clientScoket);
    WSACleanup();
    return 1;
}

printf("Bytes Sent: %ld\n", (long)iResult);

// shutdown the connection for sending since no more data will be sent
// the client can still use the ConnectSocket for receiving data
iResult = shutdown(clientScoket, SD_SEND);
if (iResult == SOCKET_ERROR) {
    printf("shutdown failed: %d\n", WSAGetLastError());
    closesocket(clientScoket);
    WSACleanup();
    return 1;
}

// Receive data until the server closes the connection
do {
    iResult = recv(clientScoket, recvbuf, recvbuflen, 0);
    if (iResult > 0)
    {
        printf("Bytes received: %d\n", iResult);
        std::cout << "Message from server : " << recvbuf << '\n';
    }
    else if (iResult == 0)
        printf("Connection closed\n");
    else
        printf("recv failed: %d\n", WSAGetLastError());
} while (iResult > 0);
    }
    closesocket(clientScoket);
    WSACleanup();
return 0;
}
