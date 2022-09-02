// SummerPractice.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "SummerPractice.h"
#include <winsock2.h>
#include <windows.h>
#include <windowsx.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>  
#include <sstream>
#include <stdlib.h>
#include <thread>
#include <mutex>
#include <mysql.h>
#include <atlstr.h>

#pragma comment(lib, "Ws2_32.lib")


#define MAX_LOADSTRING 100
#define PORT 80
#define ServerAddr "127.0.0.1"

using namespace std;

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна
mutex m;

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SUMMERPRACTICE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SUMMERPRACTICE));

    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SUMMERPRACTICE));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_SUMMERPRACTICE);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        660, 360, 600, 500, nullptr, nullptr, hInstance, nullptr);

    HWND ServerButton = CreateWindowW(L"Button", L"Start as a Server", WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE,
        200, 100, 200, 30, hWnd, (HMENU)1001, hInstance, NULL);

    HWND ClientButton = CreateWindowW(L"Button", L"Start as a Client", WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE,
        200, 150, 200, 30, hWnd, (HMENU)100, hInstance, NULL);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

void ServiceThread(SOCKET NewSock)
{
    char buffer[1024];
    wstring Error = L"Error code is ";
    int EndBuff = 0;
    HWND hWnd = FindWindowA((LPCSTR)szWindowClass, (LPCSTR)szTitle);
   
        if ((EndBuff = recv(NewSock, buffer, sizeof(buffer) - 1, 0)) == SOCKET_ERROR)
        {
            Error += to_wstring(WSAGetLastError());
            MessageBox(hWnd, Error.data(), L"Error", MB_OK);
            exit(1);
        }
        string nick, pass;
        nick = CW2A((LPWSTR)buffer); 

        if ((EndBuff = recv(NewSock, buffer, sizeof(buffer) - 1, 0)) == SOCKET_ERROR)
        {
            Error += to_wstring(WSAGetLastError());
            MessageBox(hWnd, Error.data(), L"Error", MB_OK);
            exit(1);
        }

        pass = CW2A((LPWSTR)buffer);

        if ((EndBuff = recv(NewSock, buffer, sizeof(buffer) - 1, 0)) == SOCKET_ERROR)
        {
            Error += to_wstring(WSAGetLastError());
            MessageBox(hWnd, Error.data(), L"Error", MB_OK);
            exit(1);
        }

        if (buffer[0] == 'R')
        {
            MYSQL* conn;
            conn = mysql_init(NULL);
             if (conn == NULL)
                 MessageBox(hWnd, L"Database initialization error", L"Error", MB_OK);

            if (!mysql_real_connect(conn, "localhost", "root", "Kostia09", "summerproject", 3306, NULL, 0))
                MessageBox(hWnd, L"Database connection error", L"Error", MB_OK);
            
            string query = "INSERT INTO user VALUES(";
            query += "0,\""; query += nick; query += "\",\""; query += pass; query += "\");";
            MessageBox(hWnd, wstring(query.begin(), query.end()).data(), L"Check", MB_OK);
            int a = mysql_query(conn, query.c_str());

            Sleep(500);
            if (a == 0) send(NewSock, "Registration is complete!", 26, 0);
            else send(NewSock, "Registration failed. Try other Nickname.", 41, 0);
        }
    
        if (buffer[0] == 'E')
        {
            MYSQL* conn;
            conn = mysql_init(NULL);
            if (conn == NULL)
                MessageBox(hWnd, L"Database initialization error", L"Error", MB_OK);

            if (!mysql_real_connect(conn, "localhost", "root", "Kostia09", "summerproject", 3306, NULL, 0))
                MessageBox(hWnd, L"Database connection error", L"Error", MB_OK);

            mysql_set_character_set(conn, "utf8");
            string query = "SELECT name, password FROM summerproject.user WHERE user.name =\"";
            query += nick; query += "\" AND user.password=\""; query += pass; query += "\";";
            int a = mysql_query(conn, query.c_str());

            query.clear();
            MYSQL_RES* res;
            MYSQL_ROW row;
            if (res = mysql_store_result(conn))
            {
                while (row = mysql_fetch_row(res))
                {
                    for (int i = 0; i < mysql_num_fields(res); i++)
                    {
                        query += row[i];
                    }
                }
            }

            if (!query.empty())
                send(NewSock, "Succesfully entered!", 21, 0);
            else send(NewSock, "Failed. Check name or password.", 32, 0);

            query = "SELECT name FROM summerproject.user;";
            a = mysql_query(conn, query.c_str());
            query.clear();
            if (res = mysql_store_result(conn))
            {
                while (row = mysql_fetch_row(res))
                {
                    for (int i = 0; i < mysql_num_fields(res); i++)
                    {
                        query += row[i];
                        query += ';';
                    }
                }
            }

            send(NewSock, query.c_str(), query.length(), 0);

            while ((EndBuff = recv(NewSock, buffer, sizeof(buffer) - 1, 0)) != SOCKET_ERROR)
            {
                if (buffer[0] == 'S')
                {
                    EndBuff = recv(NewSock, buffer, sizeof(buffer) - 1, 0);
                    string message = buffer;
                    query = "INSERT INTO summerproject.messages VALUES(\""; query += nick; query += "\", \"";
                    for (int i = 0; i < EndBuff; i++)
                    {
                        if (buffer[i] != ';')
                            query += buffer[i];
                        else
                            query += "\", \"";
                    }
                    query += "\");";
                    //MessageBox(hWnd, wstring(query.begin(), query.end()).data(), L"Check", 0);
                    a = mysql_query(conn, query.c_str());
                    memset(buffer, '\0', EndBuff);
                }

                if (buffer[0] == 'U')
                {
                    EndBuff = recv(NewSock, buffer, sizeof(buffer) - 1, 0);
                    query.clear();
                    //SELECT idFrom, Message From summerproject.messages WHERE ((idFrom="Nickname" AND idTo="Test") OR (idFrom="Test" AND idTo="Nickname"));
                    query = "SELECT idFrom, Message From summerproject.messages WHERE ((idFrom=\""; query += nick;
                    query += "\" AND idTo = \""; query += buffer; query += "\") OR(idFrom = \""; query += buffer;
                    query += "\" AND idTo=\""; query += nick; query += "\"));";
                    a = mysql_query(conn, query.c_str());  
                    query.clear();

                    if (res = mysql_store_result(conn))
                    {
                        while (row = mysql_fetch_row(res))
                        {
                            for (int i = 0; i < mysql_num_fields(res); i+=2)
                            {
                                query += row[i];
                                query += ":";
                                query += row[i+1];
                                query += "\r\n";
                            }
                        }
                    }
                    send(NewSock, query.c_str(), query.length(), 0);
                }
            }
        }
}

void Server(HWND hWnd)
{
    
    HWND LogWindow = CreateWindowW(L"Edit", L"History:", WS_CHILD | WS_VISIBLE | WS_BORDER
        | ES_READONLY | ES_MULTILINE | WS_VSCROLL | ES_LEFT,
        50, 50, 500, 350, hWnd, NULL, hInst, NULL);
    UpdateWindow(hWnd);

    SOCKET ServerSock = socket(AF_INET, SOCK_STREAM, 0);
    int EndBuff;
    wstring Error = L"Error code is ";

    if (ServerSock == INVALID_SOCKET)
        MessageBox(hWnd, L"Socket creation Failed", L"Error", MB_OK);

    sockaddr_in ServSockAddr, NewSockAddr;
    ServSockAddr.sin_family = AF_INET;
    ServSockAddr.sin_port = htons(PORT);
    ServSockAddr.sin_addr.s_addr = 0;

    if (bind(ServerSock, (sockaddr*)&ServSockAddr, sizeof(ServSockAddr)) < 0)
    {
        Error += to_wstring(WSAGetLastError());
        MessageBox(hWnd, Error.data(), L"ERROR", MB_OK);
    }

    listen(ServerSock, 10);// 10 - размер очереди

    SOCKET NewSock;
    int NewSockSize = sizeof(NewSockAddr);
    while (NewSock = accept(ServerSock, (sockaddr*)&NewSockAddr, &NewSockSize))
    {
        if (NewSock == INVALID_SOCKET)
        {  
            Error += to_wstring(WSAGetLastError());
            MessageBox(hWnd, Error.data(), L"ERROR", MB_OK);
        }

        thread ST(ServiceThread, NewSock);
        ST.detach();

        char buffer[1024];
        Edit_GetText(LogWindow, (LPWSTR)buffer, 1024);
        string info;
        info = CW2A((LPWSTR)buffer); info += "\n New connection:";
        getnameinfo((sockaddr*)&NewSockAddr, NewSockSize, buffer, sizeof(buffer), NULL, NULL, 0);
        info += buffer; info += ":";
        getnameinfo((sockaddr*)&NewSockAddr, NewSockSize, NULL, NULL, buffer, sizeof(buffer), 0);
        inet_ntop(AF_INET, &NewSockAddr.sin_addr, buffer, sizeof(buffer) - 1);
        info += buffer;
        wchar_t adj[1024];
        mbstowcs(adj, info.c_str(), info.length() + 1);
        LPWSTR message = adj;
        Edit_SetText(LogWindow, message);
        UpdateWindow(hWnd);

    }
}

void Client(HWND hWnd)
{
    HWND LogButton = CreateWindowW(L"Edit", L"Nickname", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,
        200, 100, 200, 30, hWnd, NULL, hInst, NULL);

    HWND PasButton = CreateWindowW(L"Edit", L"Password", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_PASSWORD,
        200, 150, 200, 30, hWnd, NULL, hInst, NULL);

    HWND EnterButton = CreateWindowW(L"Button", L"Sign in", WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE,
        200, 200, 200, 30, hWnd, (HMENU)20, hInst, NULL);

    HWND RegisterButton = CreateWindowW(L"Button", L"Sign up", WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE,
        200, 250, 200, 30, hWnd, (HMENU)201, hInst, NULL);
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//

void Refresher(SOCKET ClientSock, HWND UserList, HWND MessagesField)
{
    while (1)
    {
        char buffer[1024], buffer2[1024];
        memset(buffer, '\0', sizeof(buffer) - 1);
        memset(buffer2, '\0', sizeof(buffer) - 1);
        send(ClientSock, "U", 1, 0);
        ComboBox_GetText(UserList, (LPWSTR)buffer, sizeof(buffer) - 1);
        wcstombs(buffer2, (LPWSTR)buffer, sizeof(buffer) - 1);
        send(ClientSock, buffer2, sizeof(buffer2), 0);
        recv(ClientSock, buffer, sizeof(buffer), 0);
        wchar_t adj[1024];
        mbstowcs(adj, buffer, strlen(buffer) + 1);
        LPWSTR field = adj;
        Edit_SetText(MessagesField, field);
        Sleep(100);
    }
}

SOCKET CrutchSock;
HWND SendField, UserList;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    WSADATA wsad;
    if (WSAStartup(MAKEWORD(2, 2), &wsad) != 0)
        MessageBox(hWnd, L"Initialization Failed", L"Error", MB_OK);

    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);

        // Разобрать выбор в меню:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
        {
            DestroyWindow(hWnd);
        }
            break;
        
        case CBN_SETFOCUS:
        {
            MessageBox(hWnd, L"Initialization Failed", L"Error", MB_OK);
        }
        break;

        case 1001:
        {
            DestroyWindow(FindWindowExA(hWnd, NULL, "Button", "Start as a Server"));
            DestroyWindow(FindWindowExA(hWnd, NULL, "Button", "Start as a Client"));
            Server(hWnd);
        }
            break;

        case 100:
        {
            DestroyWindow(FindWindowExA(hWnd, NULL, "Button", "Start as a Server"));
            DestroyWindow(FindWindowExA(hWnd, NULL, "Button", "Start as a Client"));
            Client(hWnd);
        }
            break;
            
        case 301:
        {
            char buffer[1024];
            string message = "";
            HWND hWnd = FindWindowA((LPCSTR)szWindowClass, (LPCSTR)szTitle);

            //Добавь проверку на пустоту строк, потом, если будет время
            ComboBox_GetText(UserList, (LPWSTR)buffer, sizeof(buffer)-1);
            message += CW2A((LPWSTR)buffer); message += ";";
            Edit_GetText(SendField, (LPWSTR)buffer, sizeof(buffer)-1);
            message += CW2A((LPWSTR)buffer);
            send(CrutchSock, "S", 1, 0);
            send(CrutchSock, message.c_str(), message.length(), 0);
        }
        break;

        case 20:
        {
            int EndBuff = 6;
            int ClientSock = socket(AF_INET, SOCK_STREAM, 0);
            char buffer[1024], buffer2[1024];
            wstring Error = L"Error code is ";

            if (ClientSock == INVALID_SOCKET)
                MessageBox(hWnd, L"Socket creation Failed", L"Error", MB_OK);

            sockaddr_in ServSockAddr;
            ServSockAddr.sin_family = AF_INET;
            ServSockAddr.sin_port = htons(PORT);
            inet_pton(AF_INET, ServerAddr, &ServSockAddr.sin_addr.s_addr);

            if (connect(ClientSock, (sockaddr*)&ServSockAddr, sizeof(ServSockAddr)) != 0)
                MessageBox(hWnd, L"Connection Failed", L"Error", MB_OK);

            CrutchSock = ClientSock;

            Edit_GetText(FindWindowExA(hWnd, NULL, "Edit", "Nickname"), (LPWSTR)buffer, 20);
            EndBuff += 20;
            Sleep(500);
            send(ClientSock, buffer, EndBuff, 0);

            GetWindowText(FindWindowExA(hWnd, NULL, "Edit", "Password"), (LPWSTR)buffer, 20);
            Sleep(500);
            send(ClientSock, buffer, EndBuff, 0);

            Edit_SetText(FindWindowExA(hWnd, NULL, "Edit", "Password"), L"Enter");
            GetWindowText(FindWindowExA(hWnd, NULL, "Edit", "Password"), (LPWSTR)buffer, 20);
            Sleep(500);
            send(ClientSock, buffer, EndBuff, 0);
            Edit_SetText(FindWindowExA(hWnd, NULL, "Edit", "Password"), L"");

            recv(ClientSock, buffer, sizeof(buffer), 0);
            wchar_t adj[1024];
            mbstowcs(adj, buffer, strlen(buffer) + 1);
            LPWSTR message = adj;
            MessageBox(hWnd, message, L"Response", 0);

            if (buffer[0]=='S')
            {
                DestroyWindow(FindWindowExA(hWnd, NULL, "Button", "sign in"));
                DestroyWindow(FindWindowExA(hWnd, NULL, "Button", "sign up"));
                DestroyWindow(FindWindowExA(hWnd, NULL, "Edit", "Nickname"));
                DestroyWindow(FindWindowExA(hWnd, NULL, "Edit", "Password"));

                SendField = CreateWindowW(L"Edit", L"Send", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,
                    20, 400, 400, 30, hWnd, NULL, hInst, NULL);

                HWND SendButton = CreateWindowW(L"Button", L"Send", WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE,
                    440, 400, 130, 30, hWnd, (HMENU)301, hInst, NULL);

                HWND MessagesField = CreateWindowW(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER
                    | ES_READONLY | ES_MULTILINE | WS_VSCROLL | ES_LEFT,
                    20, 20, 400, 370, hWnd, NULL, hInst, NULL);

                UserList = CreateWindowW(L"Combobox", L"ComboName", WS_CHILD | WS_VISIBLE | WS_BORDER | CBS_SIMPLE | CBS_SORT,
                    440, 20, 130, 370, hWnd, (HMENU)401, hInst, NULL);

                ComboBox_SetText(UserList, L"Nickname");

                memset(buffer, '\0', sizeof(buffer) - 1);
                recv(ClientSock, buffer, sizeof(buffer), 0);
                string  exchanger;
                for (int i = 0; i < sizeof(buffer); i++)
                {
                    if (buffer[i]!=';')
                    exchanger += buffer[i];
                    else
                    {
                        ComboBox_AddItemData(UserList, wstring(exchanger.begin(), exchanger.end()).data());
                        exchanger.clear();
                    }
                }
              
                thread RF(Refresher, ClientSock, UserList, MessagesField);
                RF.detach();

            }
        }
            break;

        case 201:
        {
            int EndBuff = 6;
            int ClientSock = socket(AF_INET, SOCK_STREAM, 0);
            char buffer[1024];
            wstring Error = L"Error code is ";

            if (ClientSock == INVALID_SOCKET)
                MessageBox(hWnd, L"Socket creation Failed", L"Error", MB_OK);

            sockaddr_in ServSockAddr;
            ServSockAddr.sin_family = AF_INET;
            ServSockAddr.sin_port = htons(PORT);
            inet_pton(AF_INET, ServerAddr, &ServSockAddr.sin_addr.s_addr);

            if (connect(ClientSock, (sockaddr*)&ServSockAddr, sizeof(ServSockAddr)) != 0)
                MessageBox(hWnd, L"Connection Failed", L"Error", MB_OK);

            Edit_GetText(FindWindowExA(hWnd, NULL, "Edit", "Nickname"), (LPWSTR)buffer, 20);
            EndBuff += 20;
            Sleep(500);
            send(ClientSock, buffer, EndBuff, 0);

            GetWindowText(FindWindowExA(hWnd, NULL, "Edit", "Password"), (LPWSTR)buffer, 20);
            Sleep(500);
            send(ClientSock, buffer, EndBuff, 0);

            Edit_SetText(FindWindowExA(hWnd, NULL, "Edit", "Password"), L"Register");
            GetWindowText(FindWindowExA(hWnd, NULL, "Edit", "Password"), (LPWSTR)buffer, 20);
            Sleep(500);
            send(ClientSock, buffer, EndBuff, 0);
            Edit_SetText(FindWindowExA(hWnd, NULL, "Edit", "Password"), L"");

            recv(ClientSock, buffer, sizeof(buffer), 0);
            wchar_t adj[1024];
            mbstowcs(adj, buffer, strlen(buffer) + 1);
            LPWSTR message = adj;
            MessageBox(hWnd, message, L"Response", 0);

            
        }
        break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
