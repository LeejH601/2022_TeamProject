#pragma once
class CComponent
{
public:
	CComponent() = default;
	virtual ~CComponent() = default;

	virtual void Update(float fElapsedTime) {};
	virtual bool Init() {return true; };
	virtual void Reset() {};
};

