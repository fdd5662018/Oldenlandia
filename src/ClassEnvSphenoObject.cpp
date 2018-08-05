#include "ClassEnvSphenoObject.h"

static const float gMetallic = 0.5;
static const float gRoughness = 0.3;
static const float gAo = 0.8;
static const glm::vec3 gScale(0.3f, 0.3f, 0.3f);
static const glm::vec3 gBoneColor(0.71, 0.69, 0.58);

ClassEnvSphenoObject::ClassEnvSphenoObject(ClassModelRenderer *fModelRenderer) :
	ClassEnvObject(fModelRenderer)
{
	// model constant
	mModelRenderer->use();
	mModelRenderer->setFloat("metallic", gMetallic);
	mModelRenderer->setFloat("roughness", gRoughness);
	mModelRenderer->setFloat("ao", gAo);
	mModelRenderer->setVec3("albedo", gBoneColor);

	// it's a bit too big for our scene, so scale it down
	mModelMatrix = glm::scale(mModelMatrix, gScale);
	mModelRenderer->setMat4("model", mModelMatrix);
}


ClassEnvSphenoObject::~ClassEnvSphenoObject()
{
}
