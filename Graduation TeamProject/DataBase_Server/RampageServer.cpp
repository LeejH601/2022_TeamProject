#pragma warning(disable:4996)
#include <iostream>
#include <string>
#include <jdbc/mysql_connection.h>
#include <jdbc/mysql_driver.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/prepared_statement.h>
#include <jdbc/cppconn/datatype.h>
#include <jdbc/cppconn/variant.h>
#include <NetworkDevice.h>
#include <chrono>

#define BUFSIZE 4096
#define SERVERPORT 9000
#define UPDATE_LIKE 0x01
#define UPDATE_HATE 0x02
#define UPDATE_DOWN 0x04
char* SERVERIP = (char*)"127.0.0.1";

sql::Driver* driver;
sql::Connection* connection;
sql::Statement* statement;

std::string SELECT{ "SELECT" };
std::string FROM{ "FROM" };
std::string WHERE{ "WHERE" };
std::string REGEXP{ "REGEXP" };
std::string ORDERBY{ "ORDER BY" };
std::string INSERT{ "INSERT " };
std::string INTO{ "INTO " };
std::string member_table{ "member_table " };
std::string VALUE{ "VALUE" };

sql::ResultSet* SearchDataFromQuery(SearchData& searchData, int currentPage);
sql::ResultSet* FindComponentSet(int RecordID);
void IncreaseWorkshopParams(int RecordID, int updateParams);
void UploadWorkShop(UploadData& uploadData);

//class IRecord {
//public:
//	
//};

//#define TEST_RETURN_TABLE

DWORD WINAPI ProcessClient(LPVOID arg)
{
	CNetworkDevice Network_Device;
	UploadData uploadData;
	int TablePage = 0;

	sql::ResultSet* result;
	sql::PreparedStatement* PreStatement;

	SOCKET Arg;
	memcpy(&Arg, arg, sizeof(arg));

	Network_Device.init(Arg);

	std::cout << "connect client" << std::endl;

	// login Test
	/*while (true)
	{

	}*/

	eSERVICE_TYPE serviceType;
	SearchData searchData;

	while (true)
	{
		if (!Network_Device.RecvServiceType(serviceType))
			break;

		switch (serviceType)
		{
		case eSERVICE_TYPE::UPLOAD_RECORD:
			Network_Device.RecvUploadData(uploadData);
			UploadWorkShop(uploadData);

			/*for (int i = 0; i < 40; ++i) {
				uploadData.RecordTitle = "test" + std::to_string(i);
				UploadWorkShop(uploadData);
			}*/
			break;
		case eSERVICE_TYPE::DOWNLOAD_RECORD:
		{
			Download_Info info;
			Network_Device.RecvUploadRecordInfo(info);
			std::cout << info.RecordID << "    " << info.RecordTitle << std::endl;

			std::vector<std::vector<char>> Blobs;
			Blobs.resize(3);
			//Blobs.resize(3);
			result = FindComponentSet(info.RecordID);
			result->next();
			int Component_Size = result->getInt("ComponentSize_One");
			std::stringstream* stream = (std::stringstream*)( result->getBlob("ComponentBlob_One"));

			Blobs[0].resize(Component_Size);
			stream->read(Blobs[0].data(), Component_Size);

			Component_Size = result->getInt("ComponentSize_Two");
			stream = (std::stringstream*)(result->getBlob("ComponentBlob_Two"));

			Blobs[1].resize(Component_Size);
			stream->read(Blobs[1].data(), Component_Size);

			Component_Size = result->getInt("ComponentSize_Three");
			stream = (std::stringstream*)(result->getBlob("ComponentBlob_Three"));

			Blobs[2].resize(Component_Size);
			stream->read(Blobs[2].data(), Component_Size);

			Network_Device.SendComponentDataSet(Blobs);

			int UpdateParams = 0;
			UpdateParams |= UPDATE_DOWN;
			IncreaseWorkshopParams(info.RecordID, UpdateParams);
		}
			break;
		case eSERVICE_TYPE::UPDATE_TABLE:
		{
			std::vector<WorkShop_Record> records;
			Network_Device.ReceiveRequest(searchData);

#ifdef TEST_RETURN_TABLE
			for (int i = 0; i < 20; ++i) {
				WorkShop_Record record;
				record.RecordID = i + 1;
				std::string testDataStr = "2023-04-1";
				testDataStr += std::to_string(i);
				memcpy( record.LastUploadDate, testDataStr.c_str(), testDataStr.length());
				record.DownloadNum = i * 10;
				std::string testStr = "testtesttesttesttesttesttesttest";
				testStr += std::to_string(i + 1);
				memcpy(record.RecordTitle, testStr.c_str(), testStr.length());
				record.nLike = 0;
				record.nHate = 0;
				records.push_back(record);
			}
#else
			result = SearchDataFromQuery(searchData, TablePage);
			while (result->next())
			{
				WorkShop_Record record;
				record.RecordID = result->getInt("Record_ID");
				sql::SQLString LUDStr = result->getString("Last_Upload_Date");
				ZeroMemory(record.LastUploadDate, sizeof(record.LastUploadDate));
				memcpy(record.LastUploadDate, LUDStr.c_str(), LUDStr.length());
				record.DownloadNum = result->getInt("Download_Num");
				sql::SQLString RcTitle = result->getString("Record_Title");
				ZeroMemory(record.RecordTitle, sizeof(record.RecordTitle));
				memcpy(record.RecordTitle, RcTitle.c_str(), RcTitle.length());
				record.nLike = result->getInt("nlike");
				record.nHate = result->getInt("Hate");
				records.push_back(record);
			}
			if (result)
				delete result;
#endif
			
			Network_Device.ReturnDataTable(records);
			records.clear();
			
		}
			break;
		case eSERVICE_TYPE::NEXT_TABLE:
		{
			std::vector<WorkShop_Record> records;
			std::string query;
			query = "SELECT COUNT(*) as cnt FROM record;";
			PreStatement = connection->prepareStatement(query.c_str());
			result = PreStatement->executeQuery();

			result->next();
			int TableCnt = result->getInt("cnt");
			if(TableCnt > TablePage + 20)
				TablePage += 20;

			delete PreStatement;

			result = SearchDataFromQuery(searchData, TablePage);
			while (result->next())
			{
				WorkShop_Record record;
				record.RecordID = result->getInt("Record_ID");
				sql::SQLString LUDStr = result->getString("Last_Upload_Date");
				ZeroMemory(record.LastUploadDate, sizeof(record.LastUploadDate));
				memcpy(record.LastUploadDate, LUDStr.c_str(), LUDStr.length());
				record.DownloadNum = result->getInt("Download_Num");
				sql::SQLString RcTitle = result->getString("Record_Title");
				ZeroMemory(record.RecordTitle, sizeof(record.RecordTitle));
				memcpy(record.RecordTitle, RcTitle.c_str(), RcTitle.length());
				record.nLike = result->getInt("nlike");
				record.nHate = result->getInt("Hate");
				records.push_back(record);
			}
			if (result)
				delete result;

			Network_Device.ReturnDataTable(records);
			records.clear();
		}
			break;
		case eSERVICE_TYPE::PREV_TABLE:
		{
			std::vector<WorkShop_Record> records;
			if (TablePage >= 20)
				TablePage -= 20;
			else
				TablePage = 0;
			result = SearchDataFromQuery(searchData, TablePage);
			while (result->next())
			{
				WorkShop_Record record;
				record.RecordID = result->getInt("Record_ID");
				sql::SQLString LUDStr = result->getString("Last_Upload_Date");
				ZeroMemory(record.LastUploadDate, sizeof(record.LastUploadDate));
				memcpy(record.LastUploadDate, LUDStr.c_str(), LUDStr.length());
				record.DownloadNum = result->getInt("Download_Num");
				sql::SQLString RcTitle = result->getString("Record_Title");
				ZeroMemory(record.RecordTitle, sizeof(record.RecordTitle));
				memcpy(record.RecordTitle, RcTitle.c_str(), RcTitle.length());
				record.nLike = result->getInt("nlike");
				record.nHate = result->getInt("Hate");
				records.push_back(record);
			}
			if (result)
				delete result;

			Network_Device.ReturnDataTable(records);
			records.clear();
		}
			break;
		case eSERVICE_TYPE::INCREASE_LIKE:
		{
			Download_Info info;
			Network_Device.RecvUploadRecordInfo(info);

			int UpdateParams = 0;
			UpdateParams |= UPDATE_LIKE;
			IncreaseWorkshopParams(info.RecordID, UpdateParams);
		}
			break;
		case eSERVICE_TYPE::INCREASE_HATE:
		{
			Download_Info info;
			Network_Device.RecvUploadRecordInfo(info);

			int UpdateParams = 0;
			UpdateParams |= UPDATE_HATE;
			IncreaseWorkshopParams(info.RecordID, UpdateParams);
		}
			break;
		default:
			break;
		}
	}

	//Network_Device.ReceiveRequest(searchData);
	//result = SearchDataFromQuery(searchData);
	/*while (result->next())
	{
		Test_Record record;
		record.WID = result->getInt("WID");
		record.WName = result->getString("WName").c_str();
		record.WGrade = result->getInt("WGrade");
		record.Atk = result->getInt("Atk");
		record.Sharpness = result->getInt("Sharpness");
		record.Critical = result->getInt("Critical");
		record.Type = result->getString("Type");
		records.push_back(record);
	}*/
	/*Network_Device.RecvUploadData(uploadData);
	UploadWorkShop(uploadData);*/

	return 0;
}

sql::ResultSet* SearchDataFromQuery(SearchData& searchData, int currentPage)
{
	sql::ResultSet* result;
	sql::PreparedStatement* PreStatement;


	std::string query;
	query = SELECT + " " + "*" + " " + FROM + " " + "record" + " " + ORDERBY + " " + "Record_ID" + " limit " + std::to_string(currentPage) + ", 20;";
	//query = SELECT + " " + "Record_ID, Last_Upload_Date, Download_Num, Like, Hate, Record_Title" + " " + FROM + " " + "record" + " " + ORDERBY + " " + "Record_ID";
	PreStatement = connection->prepareStatement(query.c_str());
	result = PreStatement->executeQuery();

	/*while (result->next())
	{
		int WID = result->getInt("WID");
		sql::SQLString WName_SQL = result->getString("WName");
		std::string WName = result->getString("WName").c_str();
		int WGrade = result->getInt("WGrade");
		int Atk = result->getInt("Atk");
		int Sharpness = result->getInt("Sharpness");
		int Critical = result->getInt("Critical");
		std::string Type = result->getString("Type");
		std::cout << WID << " " << WName << " " << WGrade << " " << Atk << " " << Sharpness << " " << Critical << " " << Type << std::endl;
	}*/

	delete PreStatement;

	return result;
}

sql::ResultSet* FindComponentSet(int RecordID)
{
	sql::ResultSet* result;
	sql::PreparedStatement* PreStatement;


	std::string query;
	query = SELECT + " " + "*" + " " + FROM + " " + "record" + " " + WHERE + " " + "Record_ID = " + std::to_string(RecordID) + ";";
	PreStatement = connection->prepareStatement(query.c_str());
	result = PreStatement->executeQuery();

	return result;
}

void IncreaseWorkshopParams(int RecordID, int updateParams)
{
	std::string UpDate = "UPDATE record ";
	std::string Where = "WHERE Record_ID = " + std::to_string(RecordID) + ";";
	std::string Set = "SET";
	if (updateParams & UPDATE_LIKE) Set += " nlike = nlike + 1,";
	if (updateParams & UPDATE_HATE) Set += " Hate = Hate + 1,";
	if (updateParams & UPDATE_DOWN) Set += " Download_Num = Download_Num + 1,";
	Set.pop_back();

	std::string query = UpDate + Set + " " + Where;
	sql::PreparedStatement* PreStatement;

	PreStatement = connection->prepareStatement(query.c_str());
	PreStatement->executeQuery();

	if (PreStatement)
		delete PreStatement;
}


void UploadWorkShop(UploadData& uploadData)
{
	std::time_t nowTimeT = std::time(nullptr);

	std::string query;
	int record_ID = 1;
	std::string timeStamp; std::asctime(std::localtime(&nowTimeT));
	char buf[20];
	std::strftime(buf, 20, "%Y-%m-%d", std::localtime(&nowTimeT));
	timeStamp = buf;
	std::string title = uploadData.RecordTitle;
	std::vector<int> componentSizes;
	std::stringstream stream[3];

	int index = 0;
	for (std::vector<char>& Blobs : uploadData.ComponentBlobs) {
		componentSizes.push_back(Blobs.size());

		stream[index].write(Blobs.data(), Blobs.size());
		stream[index].setf(std::ios_base::binary);
		index++;
	}

	sql::ResultSet* result;
	sql::PreparedStatement* PreStatement;

	sql::SQLString sqlString;


	query = INSERT + INTO + "record " + "(" + "Record_Title, Last_Upload_Date, ComponentSize_One, ComponentSize_Two, ComponentSize_Three, ComponentBlob_One, ComponentBlob_Two, ComponentBlob_Three" + ") " +
		VALUE + " (?, ?, ?, ?, ?, ?, ?, ?);";

	PreStatement = connection->prepareStatement(query.c_str());
	//PreStatement->setInt(1, record_ID);
	PreStatement->setString(1, sql::SQLString(title));
	PreStatement->setString(2, sql::SQLString(timeStamp));
	PreStatement->setInt(3, componentSizes[0]);
	PreStatement->setInt(4, componentSizes[1]);
	PreStatement->setInt(5, componentSizes[2]);
	PreStatement->setBlob(6, &stream[0]);
	PreStatement->setBlob(7, &stream[1]);
	PreStatement->setBlob(8, &stream[2]);
	PreStatement->executeUpdate();

	//delete result;
	//delete PreStatement;
	//system("pause");
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
	system("pause");
	return;
}

int main(int argc, char* argv[])
{
	try
	{
		driver = get_driver_instance();
		connection = driver->connect("tcp://localhost:3306", "root", "wlfjd36796001!");
	}
	catch (const std::exception& e)
	{
		std::cout << "Could not connect to server. Error message: " << e.what() << std::endl;
		system("pause");
		exit(1);
	}

	connection->setSchema("mydb");

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

	if (connection) {
		delete connection;
		connection = nullptr;
	}

	WSACleanup();
	return 0;
}