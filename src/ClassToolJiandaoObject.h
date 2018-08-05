#pragma once
#include "ClassToolObject.h"
class ClassToolJiandaoObject :
	public ClassToolObject
{
public:
	ClassToolJiandaoObject(ClassModelRenderer *fModelRenderer, env::DEV_HAPTIC_TYPE Type = HAPTIC_LEFT);
	~ClassToolJiandaoObject();
};

