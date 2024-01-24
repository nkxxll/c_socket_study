#ifndef UNICODE
#define UNICODE 1
#endif

// link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

#include <stdio.h>
#include <stdlib.h>  // Needed for _wtoi
#include <winsock2.h>
#include <ws2tcpip.h>

int __cdecl wmain(int argc, wchar_t **argv) {
    //-----------------------------------------
    // Declare and initialize variables
    WSADATA wsaData = { 0 };
    int iResult = 0;

    //    int i = 1;

    SOCKET sock = INVALID_SOCKET;
    int iFamily = AF_INET;
    int iType = SOCK_STREAM;
    int iProtocol = IPPROTO_TCP;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        wprintf(L"WSAStartup failed: %d\n", iResult);
        return 1;
    }

    wprintf(L"Calling socket with following parameters:\n");
    wprintf(L"  Address Family = ");
    switch (iFamily) {
        case AF_UNSPEC:
            wprintf(L"Unspecified");
            break;
        case AF_INET:
            wprintf(L"AF_INET (IPv4)");
            break;
        case AF_INET6:
            wprintf(L"AF_INET6 (IPv6)");
            break;
        case AF_NETBIOS:
            wprintf(L"AF_NETBIOS (NetBIOS)");
            break;
        case AF_BTH:
            wprintf(L"AF_BTH (Bluetooth)");
            break;
        default:
            wprintf(L"Other");
            break;
    }
    wprintf(L" (%d)\n", iFamily);

    wprintf(L"  Socket type = ");
    switch (iType) {
        case 0:
            wprintf(L"Unspecified");
            break;
        case SOCK_STREAM:
            wprintf(L"SOCK_STREAM (stream)");
            break;
        case SOCK_DGRAM:
            wprintf(L"SOCK_DGRAM (datagram)");
            break;
        case SOCK_RAW:
            wprintf(L"SOCK_RAW (raw)");
            break;
        case SOCK_RDM:
            wprintf(L"SOCK_RDM (reliable message datagram)");
            break;
        case SOCK_SEQPACKET:
            wprintf(L"SOCK_SEQPACKET (pseudo-stream packet)");
            break;
        default:
            wprintf(L"Other");
            break;
    }
    wprintf(L" (%d)\n", iType);

    wprintf(L"  Protocol = %d = ", iProtocol);
    switch (iProtocol) {
        case 0:
            wprintf(L"Unspecified");
            break;
        case IPPROTO_ICMP:
            wprintf(L"IPPROTO_ICMP (ICMP)");
            break;
        case IPPROTO_IGMP:
            wprintf(L"IPPROTO_IGMP (IGMP)");
            break;
        case IPPROTO_TCP:
            wprintf(L"IPPROTO_TCP (TCP)");
            break;
        case IPPROTO_UDP:
            wprintf(L"IPPROTO_UDP (UDP)");
            break;
        case IPPROTO_ICMPV6:
            wprintf(L"IPPROTO_ICMPV6 (ICMP Version 6)");
            break;
        default:
            wprintf(L"Other");
            break;
    }
    wprintf(L" (%d)\n", iProtocol);

    sock = socket(iFamily, iType, iProtocol);
    if (sock == INVALID_SOCKET)
        wprintf(L"socket function failed with error = %d\n", WSAGetLastError());
    else {
        wprintf(L"socket function succeeded\n");

        // here comes the socket code
        struct sockaddr_in name;
        name.sin_family = AF_INET;
        name.sin_addr.s_addr = inet_addr("localhost");
        name.sin_port = htons(5150);
        int res = bind(sock, (SOCKADDR *)&name, sizeof(name));
        if (res != 0) {
            wprintf(L"bind failed: %d\n", res);
            return 1;
        }
        SOCKET clientfd = accept(sock, 0, 0);
        // https://learn.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-wsapoll
        LPWSAPOLLFD fdArray = { {
                                    .fd = 0,
                                    .events = POLLRDNORM,
                                },
                                {
                                    .fd = clientfd,
                                    .evnets = POLLRDNORM,
                                } };

        for (;;) {
            char buf[1024];
            WSAPoll(fdArray, 2, 500000);
            if (fdArray[0].revents & POLLRDNORM) {
                int n = read(0, buf, sizeof(buf));
                if (n > 1) {
                    send(clientfd, buf, n, 0);
                } else {
                    break;
                }
            } else if (fdArray[1].revents & POLLRDNORM) {
                int n = recv(clientfd, buf, sizeof(buf), 0);
                if (n > 0) {
                    printf("Received: %s", buf);
                }
            }
        }

        // Close the socket to release the resources associated
        // Normally an application calls shutdown() before closesocket
        //   to  disables sends or receives on a socket first
        // This isn't needed in this simple sample
        iResult = closesocket(sock);
        // poll for stdin and the client socket
        if (iResult == SOCKET_ERROR) {
            wprintf(L"closesocket failed with error = %d\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }
    }

    WSACleanup();

    return 0;
}
