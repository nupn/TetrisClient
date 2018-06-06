#pragma once
#include <thread>

class CNetworkThread
{
public:
	CNetworkThread() = default;
	virtual ~CNetworkThread();

	void Start();
	void Work();
	void Terminate();
private:
	std::thread* m_pThread = nullptr;
};

