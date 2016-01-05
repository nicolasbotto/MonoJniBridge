#include <string>

using namespace std;

#pragma once
class Request
{
public:
	Request();
	~Request();
	void setAssemblyName(string assemblyName);
	void setAssemblyPath(string assemblyPath);
	void setMethodName(string methodName);
	void setLog(bool log);
	void setNotifyEvents(bool notifyEvents);
	void setFullTrust(bool fullTrust);
	void setIsSingleton(bool isSingleton);

private:
	string assemblyName;
	string assemblyPath;
	string methodName;
	bool log;
	bool notifyEvents;
	bool fullTrust;
	bool isSingleton;
};

