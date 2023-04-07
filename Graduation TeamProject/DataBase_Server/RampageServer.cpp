#include <iostream>
#include <string>
#include <jdbc/mysql_connection.h>
#include <jdbc/mysql_driver.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/prepared_statement.h>
#include <jdbc/cppconn/datatype.h>
#include <jdbc/cppconn/variant.h>
#include <NetworkDevice.h>

#define BUFSIZE 4096
#define SERVERPORT 9000
char* SERVERIP = (char*)"127.0.0.1";

sql::Driver* driver;
sql::Connection* connection;
sql::Statement* statement;
sql::PreparedStatement* PreStatement;

std::string SELECT{ "SELECT" };
std::string FROM{ "FROM" };
std::string WHERE{ "WHERE" };

void SearchDataFromQuery(SearchData& searchData);

DWORD WINAPI ProcessClient(LPVOID arg)
{
	CNetworkDevice Network_Device;

	//sql::ResultSet* result;

	SOCKET Arg;
	memcpy(&Arg, arg, sizeof(arg));

	Network_Device.init(Arg);

	std::cout << "connect client" << std::endl;

	SearchData searchData;
	Network_Device.ReceiveRequest(searchData);
	SearchDataFromQuery(searchData);
	return 0;
}

void SearchDataFromQuery(SearchData& searchData)
{
	sql::ResultSet* result;

	try
	{
		driver = get_driver_instance();
		connection = driver->connect("tcp://127.0.0.1:3306", "root", "wlfjd36796001!");
	}
	catch (const std::exception& e)
	{
		std::cout << "Could not connect to server. Error message: " << e.what() << std::endl;
		system("pause");
		exit(1);
	}

	connection->setSchema("sakila");

	std::string query;
	query = SELECT + " " + "*" + " " + FROM + "sakila";
	PreStatement = connection->prepareStatement(query.c_str());
	result = PreStatement->executeQuery();


}

void TestDataBaseConnect() {
	sql::Driver* driver;
	sql::Connection* connection;
	sql::Statement* statement;
	sql::PreparedStatement* PreStatement;
	sql::ResultSet* result;

	try
	{
		driver = get_driver_instance();
		connection = driver->connect("tcp://127.0.0.1:3306", "root", "wlfjd36796001!");
	}
	catch (const std::exception& e)
	{
		std::cout << "Could not connect to server. Error message: " << e.what() << std::endl;
		system("pause");
		exit(1);
	}

	connection->setSchema("sakila");

	PreStatement = connection->prepareStatement("SELECT actor_id FROM sakila.actor;");
	result = PreStatement->executeQuery();

	while (result->next())
	{
		int actorID = result->getInt(1);
		/*std::string firstName = result->getString(2).c_str();
		std::string lastName = result->getString(3).c_str();
		sql::SQLString timeStampString = result->getString(4);*/
		printf("Reading from table=(%d)\n", actorID);
		//printf("Reading from table=(%d, %s, %s, %s)\n", actorID, firstName.c_str(), lastName.c_str(), timeStampString.c_str());
	}

	delete result;
	delete PreStatement;
	delete connection;
	system("pause");
	return;
}

int main(int argc, char* argv[])
{
	int retval;

	if (argc > 1) {
		SERVERIP = argv[1];
	}

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	//if (listen_sock == INVALID_SOCKET) c_err_quit("socket()");

	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	//inet_pton(AF_INET, SERVERIP, &serveraddr.sin_addr);

	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	//if (retval == SOCKET_ERROR) c_err_quit("bind()");

	retval = listen(listen_sock, SOMAXCONN);
	//if (retval == SOCKET_ERROR) c_err_quit("listen()");

	SOCKET client_sock;
	struct sockaddr_in clientaddr;
	int addrlen;
	int len;
	char buf[BUFSIZE + 1];

	while (true)
	{
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			//c_err_display("accept()");
			break;
		}

		HANDLE hThread = CreateThread(NULL, 0, ProcessClient, &client_sock, 0, NULL);

		if (hThread == NULL) { closesocket(client_sock); }
		else {
			CloseHandle(hThread);
		}
	}

	closesocket(listen_sock);

	WSACleanup();
	return 0;
}