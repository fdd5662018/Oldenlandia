#pragma once
#include "ClassToolObject.h"
class ClassToolMozuanObject :
	public ClassToolObject
{
public:
	ClassToolMozuanObject(ClassModelRenderer *fModelRenderer, env::DEV_HAPTIC_TYPE Type = HAPTIC_LEFT);
	~ClassToolMozuanObject();
};

