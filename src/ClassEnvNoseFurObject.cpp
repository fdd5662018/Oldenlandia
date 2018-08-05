#include "ClassEnvNoseFurObject.h"

static const float gMetallic = 0.5;
static const float gRoughness = 0.001;
static const float gAo = 0.05;
static const glm::vec3 gScale(0.3f, 0.3f, 0.3f);
static const glm::vec3 gFurColor(0.0001, 0.0001, 0.0001);

ClassEnvNoseFurObject::ClassEnvNoseFurObject(ClassModelRenderer *fModelRenderer) :
	ClassEnvObject(fModelRenderer)
{
	// model constant
	mModelRenderer->use();
	mModelRenderer->setFloat("metallic", gMetallic);
	mModelRenderer->setFloat("roughness", gRoughness);
	mModelRenderer->setFloat("ao", gAo);
	mModelRenderer->setVec3("albedo", gFurColor);

	// it's a bit too big for our scene, so scale it down
	mModelMatrix = glm::scale(mModelMatrix, gScale);
	mModelRenderer->setMat4("model", mModelMatrix);
}


ClassEnvNoseFurObject::~ClassEnvNoseFurObject()
{
}
