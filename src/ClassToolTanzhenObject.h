#pragma once
#include "ClassToolObject.h"
class ClassToolTanzhenObject :
	public ClassToolObject
{
public:
	ClassToolTanzhenObject(ClassModelRenderer *fModelRenderer, env::DEV_HAPTIC_TYPE Type = HAPTIC_LEFT);
	~ClassToolTanzhenObject();
};

