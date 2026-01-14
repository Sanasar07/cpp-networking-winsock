#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <vector>
#pragma comment(lib, "Ws2_32.lib")

#define PORT "5555"
#define BUF_SIZE 512

using namespace std;

void broadcast(const char* buffer, int bytes, SOCKET sender, const vector<SOCKET>& clients);

int main() {
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        cout << "WSAStartup failed: " << iResult << endl;
        return 1;
    }

    addrinfo hints{}, * result;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(NULL, PORT, &hints, &result);

    SOCKET listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (listenSocket == INVALID_SOCKET) {
        cout << "Socket creation failed: " << WSAGetLastError() << endl;
        return 1;
    }

    bind(listenSocket, result->ai_addr, (int)result->ai_addrlen);
    listen(listenSocket, SOMAXCONN);
    
    // ��������� ����� � ������������� �����
    u_long mode = 1;
    ioctlsocket(listenSocket, FIONBIO, &mode);

    vector<SOCKET> clients;
    char buffer[BUF_SIZE];

    cout << "Server started. Waiting for clients..." << endl;

    timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 100000; // 0.1 �������

    while (true) {
        fd_set readfds;
        FD_ZERO(&readfds);

        // ��������� listenSocket � �����
        FD_SET(listenSocket, &readfds);

        // ��������� ���� �������� � �����
        for (SOCKET c : clients) {
            FD_SET(c, &readfds);
        }

        // ���������, ����� ������ ������ � ������
        int ready = select(0, &readfds, NULL, NULL, &timeout);
        if (ready == SOCKET_ERROR) {
            cout << "select error: " << WSAGetLastError() << endl;
            break;
        }

        // ����� �����������
        if (FD_ISSET(listenSocket, &readfds)) {
            SOCKET newClient = accept(listenSocket, NULL, NULL);
            if (newClient != INVALID_SOCKET) {
                ioctlsocket(newClient, FIONBIO, &mode);
                clients.push_back(newClient);
                cout << "New client connected" << endl;
            }
        }

        // ������ ������ �� ��������
        for (int i = 0; i < clients.size(); i++) {
            SOCKET c = clients[i];
            if (FD_ISSET(c, &readfds)) {
                int bytes = recv(c, buffer, BUF_SIZE - 1, 0);
                if (bytes > 0) {
                    buffer[bytes] = '\0';
                    cout << "Client: " << buffer << endl;

                    // �������� ������ ��������
                    broadcast(buffer, bytes, c, clients);
                }
                else if (bytes == 0) {
                    cout << "Client disconnected" << endl;
                    closesocket(c);
                    clients.erase(clients.begin() + i);
                    i--; // ��������� ������ ����� ��������
                }
                else {
                    if (WSAGetLastError() != WSAEWOULDBLOCK) {
                        cout << "recv error: " << WSAGetLastError() << endl;
                        closesocket(c);
                        clients.erase(clients.begin() + i);
                        i--;
                    }
                }
            }
        }
    }

    // ��������� ��� ������
    for (SOCKET c : clients) closesocket(c);
    closesocket(listenSocket);
    WSACleanup();

    return 0;
}

// ������� broadcast
void broadcast(const char* buffer, int bytes, SOCKET sender, const vector<SOCKET>& clients) {
    for (SOCKET c : clients) {
        if (c != sender) {
            send(c, buffer, bytes, 0);
        }
    }
}
