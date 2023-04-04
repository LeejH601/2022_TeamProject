#pragma once
#include "..\Global\stdafx.h"
#include "..\Object\Object.h"

struct LinkForceParam
{
	physx::PxVec3 froces;
	physx::PxForceMode mode;
	int linkindex;
};

struct RequestArticulationParam
{
	physx::PxArticulationReducedCoordinate& articulation;
	std::vector<LinkForceParam> linkForceParams;
};

class CLagdollSimulateFramework
{
public:
	CLagdollSimulateFramework();
	~CLagdollSimulateFramework();

	void Update(float fTimeElapsed);
	void ReadbackArticulationMatrix(std::vector<std::unique_ptr<CGameObject>>& pObjects);
	void RequestRegisterArticulation(RequestArticulationParam& requsetParam);

private:
	void RegisterArticulation();
};

