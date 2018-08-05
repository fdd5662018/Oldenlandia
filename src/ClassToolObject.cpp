#include "ClassToolObject.h"

static const float gMetallic = 0.9;
static const float gRoughness = 0.15;
static const float gAo = 0.8;
static const glm::vec3 gScale(3.0f, 3.0f, 3.0f);
//static const glm::vec3 gToolColor(1 / 255.0, 1 / 255.0, 1.2 / 255.0);

ClassToolObject::ClassToolObject(ClassModelRenderer *fModelRenderer, env::DEV_HAPTIC_TYPE Type) :
	ClassObject(fModelRenderer), mDevType(Type), mButton(0), mHapticOffset(5, 5, 5)
{
	// model constant
	mModelRenderer->use();
	mModelRenderer->setFloat("metallic", gMetallic);
	mModelRenderer->setFloat("roughness", gRoughness);
	mModelRenderer->setFloat("ao", gAo);
	//mModelRenderer->setVec3("albedo", gToolColor);
}

ClassToolObject::~ClassToolObject()
{
}

void ClassToolObject::Update() 
{
	//printf("type: %d\n", mDevType);
	// father class
	ClassObject::Update();
	// haptic data update
	ResourceManager::getHapticPose((env::gToolInsertPos), mPos, mAngle, mButton, mDevType);
	// model
	mModelMatrix = glm::mat4();
	mModelMatrix = glm::translate(mModelMatrix, mPos);
	mModelMatrix = glm::rotate(mModelMatrix, mAngle[0], glm::vec3(0.0f, 1.0f, 0.0f)); // Then rotate
	mModelMatrix = glm::rotate(mModelMatrix, -mAngle[1], glm::vec3(1.0f, 0.0f, 0.0f)); // Then rotate
	mModelMatrix = glm::rotate(mModelMatrix, -mAngle[2], glm::vec3(0.0f, 0.0f, 1.0f)); // Then rotate
	mModelMatrix = glm::scale(mModelMatrix, gScale);
	// shader
	mModelRenderer->setMat4("model", mModelMatrix);
}