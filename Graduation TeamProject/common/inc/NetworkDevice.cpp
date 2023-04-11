#include "NetworkDevice.h"
#include <sstream>

#define BUFSIZE 4096

SearchData::SearchData(SearchDataBuilder& builder)
{
	searchWord = builder.GetWord();
	sortingMethod = builder.GetSortinMethod();
	sortBy = builder.GetSortBy();
}

CNetworkDevice::CNetworkDevice()
{
}

CNetworkDevice::~CNetworkDevice()
{
}

void CNetworkDevice::init(SOCKET sock)
{
	m_client_sock = sock;
}

bool CNetworkDevice::RecvServiceType(eSERVICE_TYPE& serviceType)
{
	int retval;

	retval = recv(m_client_sock, (char*)&serviceType, sizeof(eSERVICE_TYPE), MSG_WAITALL);

	if (retval == -1) {
		/*LPVOID lpMsgBuf;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, WSAGetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf, 0, NULL);
		MessageBox(NULL, (LPCTSTR)lpMsgBuf, L"recv", MB_ICONERROR);
		LocalFree(lpMsgBuf);*/
		return false;
	}

	if (retval == 0)
		return false;

	return true;
}

bool CNetworkDevice::SendServiceType(eSERVICE_TYPE& serviceType)
{
	int retval;
	char buf[BUFSIZE + 1];

	memcpy(buf, (const void*)&serviceType, sizeof(eSERVICE_TYPE));
	retval = send(m_client_sock, buf, sizeof(eSERVICE_TYPE), 0);

	if (retval == 0)
		return false;

	return true;
}

bool CNetworkDevice::UploadWorkShop(UploadData& uploadData)
{
	int DataSize;
	DataSize = uploadData.RecordTitle.size() + uploadData.UserName.size();

	std::vector<int> BlobSizes;
	for (std::vector<char>& bytes : uploadData.ComponentBlobs) {
		size_t BlobSize = bytes.size();
		DataSize += BlobSize;
		BlobSizes.push_back(BlobSize);
	}

	int dataLocations[5];
	dataLocations[0] = uploadData.RecordTitle.size();
	dataLocations[1] = uploadData.UserName.size();
	dataLocations[2] = BlobSizes[0];
	dataLocations[3] = BlobSizes[1];
	dataLocations[4] = BlobSizes[2];

	int retval;
	char buf[BUFSIZE + 1];

	memcpy(buf, &DataSize, sizeof(int));
	retval = send(m_client_sock, (const char*)buf, sizeof(int), 0);

	if (retval == 0)
		return false;

	memcpy(buf, dataLocations, sizeof(int) * 5);
	retval = send(m_client_sock, (const char*)buf, sizeof(int) * 5, 0);

	if (retval == 0)
		return false;

	char* Data = new char[DataSize];
	int offset = 0;
	memcpy(Data + offset, uploadData.RecordTitle.c_str(), uploadData.RecordTitle.size());
	offset += uploadData.RecordTitle.size();
	memcpy(Data + offset, uploadData.UserName.c_str(), uploadData.UserName.size());
	offset += uploadData.UserName.size();

	int sizeIndex = 0;
	for (std::vector<char>& bytes : uploadData.ComponentBlobs) {
		memcpy(Data + offset, bytes.data(), BlobSizes[sizeIndex]);
		offset += BlobSizes[sizeIndex++];
	}

	offset = 0;
	int remainSize = DataSize;
	while (true)
	{
		int sendSize = 0;
		if (remainSize > BUFSIZE)
			sendSize = BUFSIZE;
		else
			sendSize = remainSize;
		memcpy(buf, Data + offset, sendSize);
		retval = send(m_client_sock, (const char*)buf, sendSize, 0);

		offset += retval;
		remainSize -= retval;
		if (remainSize <= 0)
			break;
	}

	delete[] Data;

	return true;
}

bool CNetworkDevice::RecvUploadData(UploadData& uploadData)
{
	int DataSize = 0;
	int retval;
	char buf[BUFSIZE + 1];

	retval = recv(m_client_sock, (char*)&DataSize, sizeof(int), MSG_WAITALL);

	if (retval == 0)
		return false;

	std::cout << DataSize << std::endl;

	int dataLocations[5];
	retval = recv(m_client_sock, (char*)dataLocations, sizeof(int) * 5, MSG_WAITALL);

	std::cout << dataLocations[0] << std::endl;

	if (retval == 0)
		return false;

	char* Data = new char[DataSize];

	int remainSize = DataSize;
	int offset = 0;
	while (true)
	{
		retval = recv(m_client_sock, buf, BUFSIZE, 0);
		memcpy(Data + offset, buf, retval);
		offset += retval;
		remainSize -= retval;
		if (remainSize <= 0)
			break;
	}

	offset = 0;
	uploadData.RecordTitle.resize(dataLocations[0]);
	memcpy((void*)uploadData.RecordTitle.data(), (const void*)(Data + offset), dataLocations[0]);
	offset += dataLocations[0];
	uploadData.UserName.resize(dataLocations[1]);
	memcpy((void*)uploadData.UserName.data(), (const void*)(Data + offset), dataLocations[1]);
	offset += dataLocations[1];

	uploadData.ComponentBlobs.resize(3);
	for (int i = 0; i < 3; ++i) {
		uploadData.ComponentBlobs[i].resize(dataLocations[i + 2]);
		memcpy(uploadData.ComponentBlobs[i].data(), (void*)(Data + offset), dataLocations[i + 2]);
		offset += dataLocations[i + 2];
	}

	std::cout << uploadData.RecordTitle << " " << uploadData.UserName << std::endl;
	//std::vector<char>& bytes = uploadData.ComponentBlobs[0];

	/*std::stringstream ss;
	ss.write(bytes.data(), bytes.size());
	ss.setf(std::ios_base::binary);

	std::string str;
	str.resize(256);
	BYTE length;
	ss.read((char*)&length, sizeof(BYTE));
	ss.read(str.data(), length);
	while (!ss.eof())
	{
		ss.read((char*)&length, sizeof(BYTE));
		ss.read(str.data(), length);
		str.data()[length] = '\0';
		if (!strcmp(str.c_str(), "<CCameraMover>:")) {
			std::cout << str << std::endl;
			while (true)
			{
				ss.read((char*)&length, sizeof(BYTE));
				ss.read(str.data(), length);
				str.data()[length] = '\0';
				float param;
				int param_int;
				if (!strcmp(str.c_str(), "<MaxDistance>:"))
				{
					std::cout << str;
					ss.read((char*)&param, sizeof(float));
					std::cout << param << std::endl;
				}
				else if (!strcmp(str.c_str(), "<MovingTime>:"))
				{
					std::cout << str;
					ss.read((char*)&param, sizeof(float));
					std::cout << param << std::endl;
				}
				else if (!strcmp(str.c_str(), "<RollBackTime>:"))
				{
					std::cout << str;
					ss.read((char*)&param, sizeof(float));
					std::cout << param << std::endl;
				}
				else if (!strcmp(str.c_str(), "<Enable>:"))
				{
					std::cout << str;
					ss.read((char*)&param_int, sizeof(int));
					std::cout << param_int << std::endl;
				}
				else if (!strcmp(str.c_str(), "</CCameraMover>:"))
				{
					break;
				}
			}
		}
	}*/
	//for (std::vector<char>& bytes : uploadData.ComponentBlobs) {
	//	std::cout << "dataSize : " << bytes.size() << std::endl;
	//	std::cout << bytes.data() << std::endl;
	//}

	return true;
}

bool CNetworkDevice::RequestDataTable()
{
	SearchDataBuilder builder(std::string("TestSearchData"));
	builder.SetSortingMethod(SEARCH_METHOD(1)).SetSortBy(SORT_BY(3));
	SearchData searchData = builder.Build();

	int retval;
	int len;
	char buf[BUFSIZE + 1];

	len = searchData.GetWord().size();
	memcpy(buf, (void*)&len, sizeof(int));
	retval = send(m_client_sock, (const char*)buf, sizeof(int), 0);

	if (retval == 0)
		return false;

	memcpy(buf, &searchData, sizeof(SEARCH_METHOD) + sizeof(SORT_BY));
	memcpy(buf + sizeof(SEARCH_METHOD) + sizeof(SORT_BY), searchData.GetWord().data(), len);
	retval = send(m_client_sock, (const char*)buf, len + sizeof(SEARCH_METHOD) + sizeof(SORT_BY), 0);

	if (retval == 0)
		return false;

	return true;
}

#define DEBUG_SHOW_SEARCH_DATA

bool CNetworkDevice::ReturnDataTable(std::vector<WorkShop_Record>& records)
{
	int recordBaseSize = 4 * 4;
	int recordSize = records.size();

	int recordEachDynamicSizes = sizeof(WorkShop_RecordInfo) * records.size();
	std::vector<WorkShop_RecordInfo> EachDynaimcSizeInfos;

	char buf[BUFSIZE + 1];
	int len;
	int retval;

	len = recordBaseSize * records.size();
	for (WorkShop_Record& record : records) {
		len += record.RecordTitle.size();
		len += record.LastUploadDate.size();

		WorkShop_RecordInfo info; info.RecordTitleSize = record.RecordTitle.size(); info.LastUploadDateSize = record.LastUploadDate.size();
		EachDynaimcSizeInfos.push_back(info);
	}

	memcpy(buf, (void*)&recordSize, sizeof(int));
	retval = send(m_client_sock, (const char*)buf, sizeof(int), 0);

	memcpy(buf, (void*)&len, sizeof(int));
	retval = send(m_client_sock, (const char*)buf, sizeof(int), 0);

	char* metaInfo = (char*)EachDynaimcSizeInfos.data();
	int offset = 0;
	int remainSize = len;
	while (true)
	{
		int sendSize = 0;
		if (remainSize > BUFSIZE)
			sendSize = BUFSIZE;
		else
			sendSize = remainSize;
		memcpy(buf, metaInfo + offset, sendSize);
		retval = send(m_client_sock, (const char*)buf, sendSize, 0);
		offset += retval;

		if (retval == 0)
			return false;

		remainSize -= retval;
		if (remainSize <= 0)
			break;
	}

	//len = recordSize * sizeof(WorkShop_Record);
	//std::cout << "len " << len << std::endl;
	//std::cout << "recordSize " << recordSize << std::endl;

	if (retval == 0)
		return false;

	char* data = new char[len];
	offset = 0;
	for (WorkShop_Record& record : records) {
		memcpy((void*)(data + offset), (const char*)&record.RecordID, sizeof(int));
		offset += sizeof(int);
		memcpy((void*)(data + offset), (const char*)&record.DownloadNum, sizeof(int));
		offset += sizeof(int);
		memcpy((void*)(data + offset), (const char*)&record.nLike, sizeof(int));
		offset += sizeof(int);
		memcpy((void*)(data + offset), (const char*)&record.nHate, sizeof(int));
		offset += sizeof(int);

		memcpy((void*)(data + offset), record.RecordTitle.c_str(), record.RecordTitle.size());
		offset += record.RecordTitle.size();
		memcpy((void*)(data + offset), record.LastUploadDate.c_str(), record.LastUploadDate.size());
		offset += record.LastUploadDate.size();
	}

	/*WorkShop_Record TestRecord;
	offset = 0;
	memcpy(&TestRecord.RecordID, data + offset, sizeof(int));
	offset += sizeof(int);
	memcpy(&TestRecord.DownloadNum, data + offset, sizeof(int));
	offset += sizeof(int);
	memcpy(&TestRecord.nLike, data + offset, sizeof(int));
	offset += sizeof(int);
	memcpy(&TestRecord.nHate, data + offset, sizeof(int));
	offset += sizeof(int);

	TestRecord.RecordTitle.resize(30);
	memcpy(TestRecord.RecordTitle.data(), data + offset, 30);
	offset += 30;
	memcpy(TestRecord.LastUploadDate.data(), data + offset,10);
	offset += 10;*/

	offset = 0;
	remainSize = len;
	while (true)
	{
		int sendSize = 0;
		if (remainSize > BUFSIZE)
			sendSize = BUFSIZE;
		else
			sendSize = remainSize;

		memcpy(buf, data + offset, sendSize);
		retval = send(m_client_sock, (const char*)buf, sendSize, 0);
		offset += retval;

		if (retval == 0)
			return false;

		remainSize -= retval;
		if (remainSize <= 0)
			break;
	}

	return true;
}

bool CNetworkDevice::ReceiveRequest(SearchData& searchData)
{
	int retval;
	int len;
	char buf[BUFSIZE + 1];

	retval = recv(m_client_sock, (char*)&len, sizeof(int), MSG_WAITALL);
	//memcpy(&len, buf, sizeof(int));

	if (retval == 0)
		return false;

	std::string str;
	str.resize(len);

	retval = recv(m_client_sock, (char*)buf, len + sizeof(SEARCH_METHOD) + sizeof(SORT_BY), MSG_WAITALL);

	if (retval == 0)
		return false;

	memcpy((void*)str.c_str(), (void*)(buf + sizeof(SEARCH_METHOD) + sizeof(SORT_BY)), len);
	SearchDataBuilder builder{ str };
	memcpy(&builder, buf, sizeof(SEARCH_METHOD) + sizeof(SORT_BY));

	searchData = builder.Build();

#ifdef DEBUG_SHOW_SEARCH_DATA
	std::cout << searchData.GetWord() << " " << (int)(searchData.GetSortinMethod()) << " " << (int)(searchData.GetSortBy()) << std::endl;
#endif // DEBUG_SHOW_SEARCH_DATA

	return true;
}

bool CNetworkDevice::RecvDataTable(std::vector<WorkShop_Record>& records)
{
	int recordBaseSize = 4 * 4;
	int recordSize;

	int recordEachDynamicSizes;
	std::vector<WorkShop_RecordInfo> EachDynaimcSizeInfos;

	int retval;
	int len;
	char buf[BUFSIZE + 1];


	retval = recv(m_client_sock, (char*)&recordSize, sizeof(int), MSG_WAITALL);

	retval = recv(m_client_sock, (char*)&len, sizeof(int), MSG_WAITALL);

	if (retval == 0)
		return false;

	EachDynaimcSizeInfos.resize(recordSize);

	//records.resize(len / sizeof(WorkShop_Record));


	char* metaInfo = new char[recordSize * sizeof(WorkShop_RecordInfo)];
	int offset = 0;
	int remainSize = recordSize * sizeof(WorkShop_RecordInfo);
	while (true)
	{
		int recvSize;
		if (remainSize > BUFSIZE)
			recvSize = BUFSIZE;
		else
			recvSize = remainSize;

		retval = recv(m_client_sock, metaInfo + offset, recvSize, 0);

		if (retval == 0)
			return false;

		offset += retval;
		remainSize -= retval;

		if (remainSize <= 0)
			break;
	}
	memcpy(EachDynaimcSizeInfos.data(), metaInfo, recordSize * sizeof(WorkShop_RecordInfo));

	char* Data = new char[len];

	remainSize = len;
	offset = 0;
	while (true)
	{
		int recvSize;
		if (remainSize > BUFSIZE)
			recvSize = BUFSIZE;
		else
			recvSize = remainSize;

		retval = recv(m_client_sock, Data + offset, recvSize, 0);

		if (retval == 0)
			return false;

		offset += retval;
		remainSize -= retval;

		if (remainSize <= 0)
			break;
	}
	

	//memcpy(records.data(), Data, len);
	offset = 0;
	for (int i = 0; i < recordSize; ++i) {
		WorkShop_Record record;

		memcpy((char*)&record.RecordID, Data + offset, sizeof(int));
		offset += sizeof(int);
		memcpy((char*)&record.DownloadNum, Data + offset, sizeof(int));
		offset += sizeof(int);
		memcpy((char*)&record.nLike, Data + offset, sizeof(int));
		offset += sizeof(int);
		memcpy((char*)&record.nHate, Data + offset, sizeof(int));
		offset += sizeof(int);

		record.RecordTitle.resize(EachDynaimcSizeInfos[i].RecordTitleSize);
		memcpy((char*)record.RecordTitle.c_str(), (Data + offset), record.RecordTitle.size());
		offset += record.RecordTitle.size();

		record.LastUploadDate.resize(EachDynaimcSizeInfos[i].LastUploadDateSize);
		memcpy((char*)record.LastUploadDate.c_str(), (Data + offset), record.LastUploadDate.size());
		offset += record.LastUploadDate.size();


		

		

		records.push_back(record);
	}

	for (WorkShop_Record& record : records) {
		std::cout << record.RecordID << " " << record.RecordTitle << " " << record.LastUploadDate << " "
			<< record.nLike << " " << record.nLike << " " << record.DownloadNum << std::endl;
	}

	delete[] Data;
	system("puase");
	return false;
}


