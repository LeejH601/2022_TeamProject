#include <iostream>
#include <NetworkDevice.h>

#define BUFSIZE 4096
#define SERVERPORT 9000
char* address = (char*)"127.0.0.1";

int main()
{
	CNetworkDevice NetworkDevice;

	SOCKET sock;
	int retval = 0;

	// 扩加 檬扁拳
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// 家南 积己
	sock = socket(AF_INET, SOCK_STREAM, 0);
	//if (sock == INVALID_SOCKET) err_quit("socket()");

	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	inet_pton(AF_INET, address, &serveraddr.sin_addr);

	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	//if (retval == SOCKET_ERROR) err_quit("connect()");

	NetworkDevice.init(sock);

	NetworkDevice.RequestDataTable();
	NetworkDevice.RecvDataTable();
}