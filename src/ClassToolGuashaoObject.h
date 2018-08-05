#pragma once
#include "ClassToolObject.h"
class ClassToolGuashaoObject :
	public ClassToolObject
{
public:
	ClassToolGuashaoObject(ClassModelRenderer *fModelRenderer, env::DEV_HAPTIC_TYPE Type = HAPTIC_LEFT);
	~ClassToolGuashaoObject();
};

