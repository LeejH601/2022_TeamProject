#pragma once
class CComponent
{
public:
	virtual void Update(float fElapsedTime) {};
	virtual bool Init() {};
	virtual void Reset() {};
};

