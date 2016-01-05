#include "Request.h"


Request::Request()
{
}


Request::~Request()
{
}

void Request::setAssemblyName(string assemblyName)
{
	this->assemblyName = assemblyName;
}

void Request::setMethodName(string methodName)
{
	this->methodName = methodName;
}

void Request::setAssemblyPath(string assemblyPath)
{
	this->assemblyPath = assemblyPath;
}

