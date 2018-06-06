#include "NetworkThread.h"

#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <Windows.h>
#pragma comment(lib, "ws2_32.lib")

CNetworkThread::CNetworkThread()
{
}


CNetworkThread::~CNetworkThread()
{
	Terminate();
}

void CNetworkThread::Start()
{
	if (m_pThread == nullptr)
	{
		m_pThread = new std::thread(&CNetworkThread::Work, this);
	}
}

void CNetworkThread::Work()
{
	WSADATA wsaData;
	SOCKET hSocket;
	SOCKADDR_IN servAdr;

	if (argc != 3)
	{
		printf("usage : %s ", argv[0]);
		exit(1);
	}

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		ErrorHandling("WSAStartup() error!");
	}

	hSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (hSocket == INVALID_SOCKET)
	{
		ErrorHandling("socket() error!");
	}

	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
	servAdr.sin_addr.s_addr = inet_addr(argv[1]);
	servAdr.sin_port = htons(atoi(argv[2]));

	if (connect(hSocket, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
	{
		ErrorHandling("connect() error!");

	}
	else
	{
		puts("connected.......");
	}
	int strLen = 0;
	int readLen = 0;
	char message[BUF_SIZE];
	while ((readLen = recv(hSocket, &message[readLen], BUF_SIZE - 1, 0)) == 0)
	{
	}

	message[readLen] = '\n';
	message[readLen + 1] = 0;
	//cout << "From : " << message << endl;
	fputs("FromServer : ", stdout);
	fputs(message, stdout);
	fputs("\n", stdout);

	fputs("InputMessage : ", stdout);


	closesocket(hSocket);
	WSACleanup();
}

void CNetworkThread::Terminate()
{
	if (m_pThread != nullptr)
	{
		m_pThread->join();
		delete m_pThread;
		m_pThread = nullptr;
	}
}