#pragma once
//#include <Component.h>
//#include <AnimationComponent.h>
//#include <ParticleComponent.h>
//#include <BillBoardComponent.h>
//#include <SoundComponent.h>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <string>
#include <iostream>
#include <vector>
#include <bitset>
#include <fstream>

class Test_Record
{
public:
	int WID;
	std::string WName;
	int WGrade;
	int Atk;
	int Sharpness;
	int Critical;
	std::string Type;
};

#define MAX_RECORDTITLE 100
#define MAX_LASTDATESTR 20
#define MAX_LOGINIDLENGTH 45
#define MAX_PASSWORDLENGTH 45

class WorkShop_Record
{
public:
	int RecordID;
	int DownloadNum;
	int nLike;
	int nHate;
	char RecordTitle[MAX_RECORDTITLE];
	char LastUploadDate[MAX_LASTDATESTR];
};

class WorkShop_RecordInfo
{
public:
	int RecordTitleSize;
	int LastUploadDateSize;
};

class Download_Info
{
public:
	int RecordID;
	char RecordTitle[MAX_RECORDTITLE];
};

class Login_Info
{
public:
	char LoginID[MAX_LOGINIDLENGTH];
	char Password[MAX_PASSWORDLENGTH];
};

enum class SEARCH_METHOD {
	ASCENDING,
	DESCENDING,
};

enum class SORT_BY {
	DATE,
	STAR,
	LIKED,
	DISLIKE,
	DOWNLOAD,
};

enum class eSERVICE_TYPE {
	UPLOAD_RECORD,
	DOWNLOAD_RECORD,
	UPDATE_TABLE,
	NEXT_TABLE,
	PREV_TABLE,
	INCREASE_LIKE,
	INCREASE_HATE,
	DECREASE_LIKE,
	DECREASE_HATE,
};

class SearchDataBuilder;
class SearchData 
{
	friend class SearchDataBuilder;

	SEARCH_METHOD sortingMethod;
	SORT_BY sortBy;
	std::string searchWord;

public:
	std::string GetWord() { return searchWord; };
	SEARCH_METHOD GetSortinMethod() { return sortingMethod; };
	SORT_BY GetSortBy() { return sortBy; };
	SearchData() {};

private:
	SearchData(SearchDataBuilder& builder);
};

class UploadData
{
public:
	std::string UserName;
	std::string RecordTitle;
	std::vector<std::vector<char>> ComponentBlobs;
};

class SearchDataBuilder
{
	SEARCH_METHOD sortingMethod;
	SORT_BY sortBy;
	std::string searchWord;

public:
	SearchDataBuilder(std::string word) : searchWord{ word }, sortingMethod{ SEARCH_METHOD::ASCENDING }, sortBy{ SORT_BY::DATE } {};

	std::string GetWord() { return searchWord; };
	SEARCH_METHOD GetSortinMethod() { return sortingMethod; };
	SORT_BY GetSortBy() { return sortBy; };

	SearchDataBuilder& SetSortingMethod(SEARCH_METHOD method) { sortingMethod = method; return *this; };
	SearchDataBuilder& SetSortBy(SORT_BY srtBy) { sortBy = srtBy; return *this;};

	SearchData Build() {
		return SearchData(*this);
	}
};

class CNetworkDevice
{
	SOCKET m_client_sock;

public:
	CNetworkDevice();
	~CNetworkDevice();

	void init(SOCKET sock);
	bool RecvServiceType(eSERVICE_TYPE& serviceType);
	bool SendServiceType(eSERVICE_TYPE& serviceType);
	bool UploadWorkShop(UploadData& uploadData);
	bool RecvUploadData(UploadData& uploadData);
	bool RequestDataTable();
	bool ReturnDataTable(std::vector<WorkShop_Record>& records);
	bool ReceiveRequest(SearchData& searchData);
	bool RecvDataTable(std::vector<WorkShop_Record>& records);
	bool RecvUploadRecordInfo(Download_Info& info);
	bool SendRequestDownload(Download_Info& info);
	bool SendComponentDataSet(std::vector<std::vector<char>>& Blobs);
	bool RecvComponentDataSet(std::vector<std::vector<char>>& Blobs);
	bool SendRequestLogin(Login_Info& logininfo);
	bool RecvRequesLogin(Login_Info& logininfo);

	bool SendToNetwork() {};
	bool RecvByNetwork() {};
};