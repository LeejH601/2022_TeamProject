#pragma once
#include <list>

#include "..\Global\stdafx.h"
#include "..\Object\Object.h"
#include "..\Global\Timer.h"




class CLagdollSimulateFramework
{
private:
	std::vector<RequestArticulationParam> m_vRequestParams;
	std::list<CGameObject*> simulateObjects;

	CGameTimer m_Timer;

public:
	bool m_bEnd = false;
	bool m_bReleased = false;

public:
	CLagdollSimulateFramework();
	~CLagdollSimulateFramework();

	void Run();
	void Update(float fTimeElapsed);
	void ReadbackArticulationMatrix();
	void RequestRegisterArticulation(RequestArticulationParam& requsetParam);

private:
	void RegisterArticulation();
};

extern DWORD WINAPI LagdollRun(LPVOID arg);
