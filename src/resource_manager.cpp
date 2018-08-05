/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include "resource_manager.h"
#include "ClassIBLMaker.h"
#include "haptic.h"

#include <iostream>
#include <sstream>
#include <fstream>

//#define STB_IMAGE_IMPLEMENTATION
//#include <stb_image.h>

// Instantiate static variables
std::map<std::string, Texture2D> ResourceManager::Textures;
std::map<std::string, Shader> ResourceManager::Shaders;
std::map<std::string, Model *> ResourceManager::pModels;
Camera *ResourceManager::pCamera = NULL;
ClassIBLMaker *ResourceManager::pIBLMaker = NULL;
Haptic *ResourceManager::pHapticRight = NULL;
Haptic *ResourceManager::pHapticLeft = NULL;

Shader ResourceManager::LoadShader(std::string name, const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile)
{
	if (Shaders.count(name) != 0) {
		cout << "Shaders already existence: " << name << endl;
		return Shaders[name];
	}
	Shaders[name] = Shader(vShaderFile, fShaderFile, gShaderFile);
	return Shaders[name];
}

Shader ResourceManager::GetShader(std::string name)
{
	if (Shaders.count(name) == 0) {
		cout << "Shaders not existence: " << name << endl;
	}
	return Shaders[name];
}

Texture2D ResourceManager::LoadTexture(std::string name, const GLchar *file, bool gammaCorrection)
{
	if (Textures.count(name) != 0) {
		cout << "Textures already existence: " << name << endl;
		return Textures[name];
	}
	Textures[name] = Texture2D(file, gammaCorrection);
	return Textures[name];
}

Texture2D ResourceManager::GetTexture(std::string name)
{
	if (Textures.count(name) == 0) {
		cout << "Textures not existence: " << name << endl;
	}
	return Textures[name];
}

Model *ResourceManager::LoadpModel(std::string name, const GLchar *file, bool gammaCorrection)
{
	if (pModels.count(name) != 0) {
		cout << "Model already existence: " << name << endl;
		return pModels[name];
	}
	pModels[name] = new Model(file, gammaCorrection);
	return pModels[name];
}

Model *ResourceManager::GetpModel(std::string name)
{
	if (pModels.count(name) == 0) {
		cout << "Model not existence: " << name << endl;
	}
	return pModels[name];
}

Camera *ResourceManager::GetCamera(glm::vec3 fPosition)
{
	if (pCamera == NULL) {
		pCamera = new Camera(fPosition);
		//pCamera->Front = fFront;
	}

	return pCamera;
}

void ResourceManager::LoadIBLMap(){
	if (pIBLMaker == NULL) {
		pIBLMaker = new ClassIBLMaker();
		pIBLMaker->Make();
	}
}

GLuint ResourceManager::getIrradianceMap() {
	return pIBLMaker->getIrradianceMap();
}
GLuint ResourceManager::getPrefilterMap() {
	return pIBLMaker->getPrefilterMap();
}
GLuint ResourceManager::getBrdfLUTTexture() {
	return pIBLMaker->getBrdfLUTTexture();
}

void ResourceManager::BindIBLMap() {
	// bind pre-computed IBL data
	glActiveTexture(GL_TEXTURE0 + env::irradianceMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, getIrradianceMap());
	glActiveTexture(GL_TEXTURE0 + env::prefilterMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, getPrefilterMap());
	glActiveTexture(GL_TEXTURE0 + env::brdfLUTTexture);
	glBindTexture(GL_TEXTURE_2D, getBrdfLUTTexture());
}

void ResourceManager::LoadHapticDevice(env::DEV_HAPTIC_TYPE devType) {

	switch (devType)
	{
		case HAPTIC_RIGHT: {
			if (pHapticRight == NULL)
				pHapticRight = new Haptic("Right Device");
			pHapticRight->Start();
		} break;
		case HAPTIC_LEFT: {
			if (pHapticLeft == NULL)
				pHapticLeft = new Haptic("Left Device");
			pHapticLeft->Start();
		} break;
		case HAPTIC_BOTH: {
			if (pHapticRight == NULL)
				pHapticRight = new Haptic("Right Device");
			if (pHapticLeft == NULL)
				pHapticLeft = new Haptic("Left Device");
			pHapticRight->Start();
			pHapticLeft->Start();
		} break;
		default:	printf("ResourceManager::LoadHapticDevice:Type error: %d\n", devType);
	}
}

void ResourceManager::getHapticPose(glm::vec3 centralPos, glm::vec3 &offsitePos, glm::vec3 &Angle, int &Button, env::DEV_HAPTIC_TYPE devType)
{
	switch (devType)
	{
		case HAPTIC_RIGHT: {
			if (pHapticRight == NULL) {
				printf("ResourceManager::getHapticPose: pHapticRight not initialized \n");
				LoadHapticDevice(HAPTIC_BOTH);
			}
			pHapticRight->Update(centralPos, offsitePos, Angle, Button);
			//std::cout << "----------------------->" << __LINE__ << endl;
		} break;
		case HAPTIC_LEFT: {
			if (pHapticLeft == NULL) {
				printf("ResourceManager::getHapticPose: pHapticLeft not initialized \n");
				LoadHapticDevice(HAPTIC_BOTH);
			}
			pHapticLeft->Update(centralPos, offsitePos, Angle, Button);
			//std::cout << "----------------------->" << __LINE__ << endl;
		} break;
		default:	printf("ResourceManager::LoadHapticDevice:Type error: %d\n", devType);
	}
}

void ResourceManager::Clear()
{
	// (Properly) delete all shaders	
	for (auto iter : Shaders)
		glDeleteProgram(iter.second.ID);
	// (Properly) delete all textures
	for (auto iter : Textures)
		glDeleteTextures(1, &iter.second.ID);

	delete pCamera;
	delete pIBLMaker;
	delete pHapticRight;
	delete pHapticLeft;
}