#pragma once

#include "Object.h"

//-----------------------------------------------------------------------------
// 22/09/22 CGameObject 클래스 추가 - Leejh
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

