#include <Debugging.h>
#if DEFINED(USE_OHMCARSIMULATOR)
#include <OhmCarSimulatorConnector.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")


    OhmCarSimulatorConnector::OhmCarSimulatorConnector(){}

    OhmCarSimulatorConnector* OhmCarSimulatorConnector::getInstance()
    {
        static OhmCarSimulatorConnector ohmCarSimulatorConnector;
        return &ohmCarSimulatorConnector;
    }

    void OhmCarSimulatorConnector::floatToBytes(float value, uint8_t* buffer, int offset)
    {
        buffer[offset] = *((uint8_t*)&value+3);
        buffer[offset+1] = *((uint8_t*)&value+2);
        buffer[offset+2] = *((uint8_t*)&value+1);
        buffer[offset+3] = *((uint8_t*)&value);
    }

    int OhmCarSimulatorConnector::readInt(uint8_t* data, int offset)
    {
        return (int)data[offset] << 24 | (int)data[offset+1] << 16 |(int)data[offset+2] << 8 | (int)data[offset+3];
    }

    void OhmCarSimulatorConnector::intToBytes(int value, uint8_t* data, int offset)
    {
        data[offset] = (uint8_t)(value >> 24);
        data[offset+1] = (uint8_t)(value >> 16);
        data[offset+2] = (uint8_t)(value >> 8);
        data[offset+3] = (uint8_t)(value);
    }

    void OhmCarSimulatorConnector::startConnection()
    {
    isSocketConnected = false;

        WSADATA wsaData;

        int iResult;

        // Initialize Winsock
        iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
        if (iResult != 0) {
            printf("WSAStartup failed: %d\n", iResult);
            return;
        }

        struct addrinfo* result = NULL;
        struct addrinfo* ptr = NULL;
        struct addrinfo  hints;

        ZeroMemory( &hints, sizeof(hints) );
        hints.ai_family   = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;

        // Resolve the server address and port
        iResult = getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result);
        if (iResult != 0) {
            printf("getaddrinfo failed: %d\n", iResult);
            WSACleanup();
            return;
        }

        ConnectSocket = INVALID_SOCKET;

        // Attempt to connect to the first address returned by
        // the call to getaddrinfo
        ptr=result;

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

        if (ConnectSocket == INVALID_SOCKET) {
            printf("Error at socket(): %ld\n", WSAGetLastError());
            freeaddrinfo(result);
            WSACleanup();
            return;
        }


        // Connect to server.
        iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
        }


        // Should really try the next address returned by getaddrinfo
        // if the connect call failed
        // But for this simple example we just free the resources
        // returned by getaddrinfo and print an error message

        freeaddrinfo(result);

        if (ConnectSocket == INVALID_SOCKET) {
            printf("Unable to connect to server!\n");
            WSACleanup();
            return;
        }

        isSocketConnected = true;
    }

    void OhmCarSimulatorConnector::closeConnection()
    {
        if(!isSocketConnected)
            return;

        isSocketConnected = false;

        int iResult;

        iResult = shutdown(ConnectSocket, SD_SEND);
        if (iResult == SOCKET_ERROR) {
            printf("shutdown failed: %d\n", WSAGetLastError());
            closesocket(ConnectSocket);
            WSACleanup();
            return;
        }

        iResult = shutdown(ConnectSocket, SD_SEND);
        if (iResult == SOCKET_ERROR) {
            printf("shutdown failed: %d\n", WSAGetLastError());
            closesocket(ConnectSocket);
            WSACleanup();
            return;
        }

        // cleanup
        closesocket(ConnectSocket);
        WSACleanup();
    }

    void OhmCarSimulatorConnector::sendData(uint8_t* data, int length)
    {
        int iResult = send(ConnectSocket, (char*)data, length, 0);
        if (iResult == SOCKET_ERROR) {
            isSocketConnected = false;
            printf("send failed: %d\n", WSAGetLastError());
            closesocket(ConnectSocket);
            WSACleanup();
            return;
        }
    }
#endif