#pragma once
#include "utility.h"
#include "ClassModelRenderer.h"
class ClassObject
{
public:
	ClassObject() {}
	ClassObject(ClassModelRenderer *fModelRenderer);
	~ClassObject();

	// Draw
	virtual void Draw();
	virtual void Update();

	glm::mat4 mModelMatrix;
	ClassModelRenderer  *mModelRenderer;
private:
	// lighting
	glm::vec3 mLightPos;
	glm::vec3 mLightColor;
	glm::mat4 mProjection;
	glm::mat4 mView;
};

