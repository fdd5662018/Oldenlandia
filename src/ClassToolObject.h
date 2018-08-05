#pragma once
#include "ClassObject.h"

class ClassToolObject :
	public ClassObject
{
public:
	ClassToolObject() {}
	ClassToolObject(ClassModelRenderer *fModelRenderer, env::DEV_HAPTIC_TYPE Type = HAPTIC_LEFT);
	~ClassToolObject();
	virtual void Update();
protected:
	int mButton;
private:
	env::DEV_HAPTIC_TYPE mDevType;
	glm::vec3 mPos;
	glm::vec3 mAngle;
	glm::vec3 mHapticOffset;
};

