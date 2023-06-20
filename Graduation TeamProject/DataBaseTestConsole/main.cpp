#include <iostream>
#include <fstream>
#include <filesystem>
#include <NetworkDevice.h>

#define BUFSIZE 4096
#define SERVERPORT 9000
char* address = (char*)"127.0.0.1";

void ShowRecords(std::vector<WorkShop_Record>& records);

int main()
{
	CNetworkDevice NetworkDevice;
	std::vector<WorkShop_Record> records;

	SOCKET sock;
	int retval = 0;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// 소켓 생성
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

	bool isLogin = true;
	while (isLogin)
	{
		if (isLogin == false)
			break;
		int input;
		std::cout << "Sine in : 0, Sine Up : 1" << std::endl << "input : ";
		std::cin >> input;

		switch (input)
		{
		case 0:
		{
			eSERVICE_TYPE svType = eSERVICE_TYPE::SINE_IN;
			NetworkDevice.SendServiceType(svType);

			Login_Info info;
			std::cout << "ID : ";
			std::cin >> info.LoginID;
			std::cout << "Password : ";
			std::cin >> info.Password;

			NetworkDevice.SendRequestLogin(info);
			isLogin = false; // 로그인 확인 필요
		}
			break;
		case 1:
		{
			eSERVICE_TYPE svType = eSERVICE_TYPE::SINE_UP;
			NetworkDevice.SendServiceType(svType);

			SineUp_Info info;
			std::cout << "ID : ";
			std::cin >> info.LoginID;
			std::cout << "Password : ";
			std::cin >> info.Password;
			std::cout << "UserName : ";
			std::cin >> info.UserName;

			NetworkDevice.SendRequestSineUp(info);
			
			int ret;
			NetworkDevice.RecvApproveSineUp(ret);

			if (ret == 0)
				std::cout << "Failed Sine Up" << std::endl;
			else
				std::cout << "Success Sine Up" << std::endl;
		}
			break;
		default:
			break;
		}
	

	}

	/*std::ifstream in_0{ "Data/Component0.bin", std::ios_base::binary };
	std::ifstream in_1{ "Data/Component1.bin", std::ios_base::binary };
	std::ifstream in_2{ "Data/Component2.bin", std::ios_base::binary };

	in_0.seekg(0, std::ios::end);
	size_t in0_Size = in_0.tellg();
	in_0.seekg(0, std::ios::beg);

	in_1.seekg(0, std::ios::end);
	size_t in1_Size = in_1.tellg();
	in_1.seekg(0, std::ios::beg);

	in_2.seekg(0, std::ios::end);
	size_t in2_Size = in_2.tellg();
	in_2.seekg(0, std::ios::beg);

	UploadData uploadData;
	uploadData.RecordTitle = "test1";
	uploadData.UserName = "testUser";
	uploadData.ComponentBlobs.resize(3);
	uploadData.ComponentBlobs[0].resize(in0_Size);
	in_0.read(uploadData.ComponentBlobs[0].data(), in0_Size);

	uploadData.ComponentBlobs[1].resize(in1_Size);
	in_1.read(uploadData.ComponentBlobs[1].data(), in1_Size); 

	uploadData.ComponentBlobs[2].resize(in2_Size);
	in_2.read(uploadData.ComponentBlobs[2].data(), in2_Size);*/



	eSERVICE_TYPE serviceType = eSERVICE_TYPE::UPLOAD_RECORD;

	while (true)
	{
		int input;
		std::cout << "Upload : 0, DownLoad : 1, UpdateTable : 2, Next : 3, Prev : 4, Like : 5, Hate : 6" << std::endl;
		std::cin >> input;
		serviceType = (eSERVICE_TYPE)input;

		NetworkDevice.SendServiceType(serviceType);

		switch (serviceType)
		{
		case eSERVICE_TYPE::UPLOAD_RECORD:
			//NetworkDevice.UploadWorkShop(uploadData);
			break;
		case eSERVICE_TYPE::DOWNLOAD_RECORD:
		{
			Download_Info info;
			info.RecordID = 1;
			std::string testStr = "test";
			memcpy(info.RecordTitle, testStr.data(), testStr.length());
			NetworkDevice.SendRequestDownload(info);
			std::vector<std::vector<char>> Blobs;
			NetworkDevice.RecvComponentDataSet(Blobs);
		}
		break;
		case eSERVICE_TYPE::UPDATE_TABLE:
			records.clear();
			NetworkDevice.RequestDataTable();
			NetworkDevice.RecvDataTable(records);
			ShowRecords(records);
			break;
		case eSERVICE_TYPE::NEXT_TABLE:
			NetworkDevice.RecvDataTable(records);
			ShowRecords(records);
			break;
		case eSERVICE_TYPE::PREV_TABLE:
			NetworkDevice.RecvDataTable(records);
			ShowRecords(records);
			break;
		case eSERVICE_TYPE::INCREASE_LIKE:
		{
			int rid;
			std::cout << "input Record ID" << std::endl;
			std::cin >> rid;
			Download_Info info;
			info.RecordID = rid;
			std::string testStr = "test";
			memcpy(info.RecordTitle, testStr.data(), testStr.length());
			NetworkDevice.SendRequestDownload(info);
		}
		break;
		case eSERVICE_TYPE::INCREASE_HATE:
		{
			int rid;
			std::cout << "input Record ID" << std::endl;
			std::cin >> rid;
			Download_Info info;
			info.RecordID = rid;
			std::string testStr = "test";
			memcpy(info.RecordTitle, testStr.data(), testStr.length());
			NetworkDevice.SendRequestDownload(info);
		}
		break;
		default:
			break;
		}
	}

	

	//NetworkDevice.RequestDataTable();
	//NetworkDevice.RecvDataTable(records);
	//NetworkDevice.UploadWorkShop(uploadData);

	system("pause");
	return 0;
}

void ShowRecords(std::vector<WorkShop_Record>& records)
{
	for (WorkShop_Record& record : records) {
		std::cout << record.RecordID << " " << record.RecordTitle << " " << record.LastUploadDate << " "
			<< record.nLike << " " << record.nHate << " " << record.DownloadNum << std::endl;
	}
}
