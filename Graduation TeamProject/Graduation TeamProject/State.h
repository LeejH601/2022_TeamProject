#pragma once

#include "Object.h"

//-----------------------------------------------------------------------------
// 22/09/22 CGameObject Ŭ���� �߰� - Leejh
//-----------------------------------------------------------------------------


class CState
{
public:
	CState();
	virtual ~CState();

	virtual void Excute(CGameObject* obj, float fTimeElapsed);
	virtual void Enter(CGameObject* obj, float fTimeElapsed);
	virtual void Exit(CGameObject* ojb, float fTimeElapsed);
};

//-----------------------------------------------------------------------------

