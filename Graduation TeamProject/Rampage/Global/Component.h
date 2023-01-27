#pragma once
class Telegram;

class CComponent
{
public:
	CComponent() = default;
	virtual ~CComponent() = default;

	virtual void Update(float fElapsedTime) {};
	virtual bool Init() {return true; };
	virtual void Reset() {};
	virtual bool HandleMessage(const Telegram& msg) { return true; };
};

