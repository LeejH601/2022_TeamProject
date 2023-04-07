#include "NetworkDevice.h"

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

bool CNetworkDevice::ReturnDataTable(std::vector<Test_Record>& records)
{
	int recordSize = records.size();
	char buf[BUFSIZE + 1];
	int len;
	int retval;

	len = recordSize * sizeof(Test_Record);
	std::cout << "len " << len << std::endl;
	std::cout << "recordSize " << recordSize << std::endl;
	memcpy(buf, (void*)&len, sizeof(int));
	retval = send(m_client_sock, (const char*)buf, sizeof(int), 0);

	if (retval == 0)
		return false;

	char* data = (char*)records.data();
	int offset = 0;
	int remainSize = len;
	while (true)
	{
		int sendSize = 0;
		if (remainSize > BUFSIZE)
			sendSize = BUFSIZE;
		else
			sendSize = remainSize;
		memcpy(buf, data + offset, sendSize);
		retval = send(m_client_sock, (const char*)buf, sendSize, 0);
		offset += sendSize;

		if (retval == 0)
			return false;

		remainSize -= BUFSIZE;
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

bool CNetworkDevice::RecvDataTable()
{
	int retval;
	int len;
	char buf[BUFSIZE + 1];

	retval = recv(m_client_sock, (char*)&len, sizeof(int), MSG_WAITALL);

	if (retval == 0)
		return false;

	std::vector<Test_Record> records;
	records.resize(len / sizeof(Test_Record));

	char* Data = new char[len];

	int remainSize = len;
	int offset = 0;
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

	memcpy(records.data(), Data, len);

	for (Test_Record& record : records) {
		std::cout << record.WID << " " << record.WName << " " << record.WGrade << " " 
			<< record.Atk << " " << record.Sharpness << " " << record.Critical << " " << record.Type << std::endl;
	}
	system("puase");
	return false;
}


