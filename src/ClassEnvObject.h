#pragma once
#include "ClassObject.h"
class ClassEnvObject :
	public ClassObject
{
public:
	ClassEnvObject() {}
	ClassEnvObject(ClassModelRenderer *fModelRenderer);
	~ClassEnvObject();
private:
};

