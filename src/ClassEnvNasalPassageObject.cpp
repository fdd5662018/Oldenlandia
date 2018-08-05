#include "ClassEnvNasalPassageObject.h"

static const float gMetallic = 0.0;
static const float gRoughness = 0.1;
static const float gAo = 0.8;
static const glm::vec3 gScale(0.3f, 0.3f, 0.3f);

ClassEnvNasalPassageObject::ClassEnvNasalPassageObject(ClassModelRenderer *fModelRenderer) :
	ClassEnvObject(fModelRenderer)
{
	// model constant
	mModelRenderer->use();
	mModelRenderer->setFloat("metallic", gMetallic);
	mModelRenderer->setFloat("roughness", gRoughness);
	mModelRenderer->setFloat("ao", gAo);

	// it's a bit too big for our scene, so scale it down
	mModelMatrix = glm::scale(mModelMatrix, gScale);
	mModelRenderer->setMat4("model", mModelMatrix);
}


ClassEnvNasalPassageObject::~ClassEnvNasalPassageObject()
{
}