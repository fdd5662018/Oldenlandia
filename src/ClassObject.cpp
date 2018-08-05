#include "ClassObject.h"
#include "resource_manager.h"
#include<iostream>

ClassObject::ClassObject(ClassModelRenderer *fModelRenderer)
	:mLightPos(glm::vec3(1.19022f, 35.9603f, 11.6979f)), mLightColor(glm::vec3(15.0f, 15.0f, 15.0f))
{
	mModelRenderer = fModelRenderer;
	mModelRenderer->use();
	mModelRenderer->setInt("irradianceMap", env::irradianceMap);
	mModelRenderer->setInt("prefilterMap", env::prefilterMap);
	mModelRenderer->setInt("brdfLUT", env::brdfLUTTexture);
}

ClassObject::~ClassObject()
{
}

void ClassObject::Update() 
{
	mProjection = glm::perspective(glm::radians(ResourceManager::GetCamera()->Zoom), (float)env::SCR_WIDTH / (float)env::SCR_HEIGHT, 0.1f, 100.0f);
	mModelRenderer->use();
	mModelRenderer->setMat4("projection", mProjection);
	mModelRenderer->setVec3("lightPositions[0]", mLightPos);
	mModelRenderer->setVec3("lightColors[0]", mLightColor);
	mView = ResourceManager::GetCamera()->GetViewMatrix();
	mModelRenderer->setMat4("view", mView);
	mModelRenderer->setVec3("camPos", ResourceManager::GetCamera()->Position);

}

void ClassObject::Draw()
{
	mModelRenderer->Draw();
}
