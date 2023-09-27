#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <iostream>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib") // dynamicly link winsock2 

int main()
{
    //Winsock start up
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
    //Socket creation and initial set up (TCP type , etc..)
     SOCKET serverSocket = INVALID_SOCKET;
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(serverSocket == INVALID_SOCKET)
    {
        std::cout << "Error at socket(): " << WSAGetLastError() << '\n';
        WSACleanup();
        return 0;
    }
    else {
        std::cout << "Socket() is OK!\n"; 
    }

    // We need this socaddr_in obj to finish setting up the socket and bind it to an ip/port
    sockaddr_in service;
    service.sin_family = AF_INET;
    service.sin_port = 9999;  // this is actually converted into 3879 because of big endian - little endian conversion
                              // use of nhlot() or htol() can solve this flipping bytes issue
    InetPton(AF_INET, ("127.0.0.1"), &(service.sin_addr.s_addr));

    // actual binding of the socket
    if(bind(serverSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
    {
        
        std::cout << "bind() failed: " << WSAGetLastError() << '\n';
        closesocket(serverSocket);
        WSACleanup();
        return 0;
    }
    else {
        std::cout << "bind() is OK!\n";   
    }

    //listen function to "open" the socket for listening on the port 

    if (listen(serverSocket , 1) == SOCKET_ERROR)
    {
        std::cout << "listen(): Error listening on socket" << WSAGetLastError() << '\n';
    }
    else 
    {
        socklen_t len = sizeof(service);
        if (getsockname(serverSocket, (struct sockaddr *)&service, &len) == -1)
            perror("getsockname");
        else
            printf("port number %d\n", ntohs(service.sin_port));
        std::cout << "listen() is OK, I am waiting for connections..\n";    
    }

    // accepting function for when a new connection is established.
    // accept is a blocking function so the server will idle untill 
    // a client connects to it
    SOCKET acceptSocket;
    acceptSocket = accept(serverSocket, NULL, NULL);
    if(acceptSocket == INVALID_SOCKET)
    {
        std::cout << "accept failed : " << WSAGetLastError() << '\n';
        WSACleanup();
        return -1;
    }
    else {
        #define DEFAULT_BUFLEN 512

char recvbuf[DEFAULT_BUFLEN];
int iResult, iSendResult;
int recvbuflen = DEFAULT_BUFLEN;

// Receive until the peer shuts down the connection
do {

    iResult = recv(acceptSocket, recvbuf, recvbuflen, 0);
    if (iResult > 0) {
        printf("Bytes received: %d\n", iResult);
        std::cout << "Message from client is : " << recvbuf << '\n'; //this is where you can process the data

        // Echo the buffer back to the sender
        iSendResult = send(acceptSocket,"response from server", iResult, 0);
        if (iSendResult == SOCKET_ERROR) {
            printf("send failed: %d\n", WSAGetLastError());
            closesocket(acceptSocket);
            WSACleanup();
            return 1;
        }
        printf("Bytes sent: %d\n", iSendResult);
    } else if (iResult == 0)
        printf("Connection closing...\n");
    else {
        printf("recv failed: %d\n", WSAGetLastError());
        closesocket(acceptSocket);
        WSACleanup();
        return 1;
    }
    } while (iResult > 0);

    }
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}