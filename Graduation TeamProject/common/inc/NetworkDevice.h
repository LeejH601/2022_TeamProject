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
	bool RequestDataTable();
	bool ReturnDataTable(std::vector<Test_Record>& records);
	bool ReceiveRequest(SearchData& searchData);
	bool RecvDataTable();

	bool SendToNetwork() {};
	bool RecvByNetwork() {};
};