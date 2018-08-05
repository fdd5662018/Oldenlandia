#pragma once
#include "ClassToolObject.h"
class ClassToolChongxiqiaoObject :
	public ClassToolObject
{
public:
	ClassToolChongxiqiaoObject(ClassModelRenderer *fModelRenderer, env::DEV_HAPTIC_TYPE Type = HAPTIC_LEFT);
	~ClassToolChongxiqiaoObject();

	virtual void Update();
	virtual void Draw();
};