#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <learnopengl/shader.h>
#include "model.h"

class ClassIBLMaker
{
public:
	ClassIBLMaker();
	~ClassIBLMaker();

	void Make();
	GLuint getEnvCubemap();
	GLuint getIrradianceMap();
	GLuint getPrefilterMap();
	GLuint getBrdfLUTTexture();

private:
	GLuint mCaptureFBO;
	GLuint mCaptureRBO;
	GLuint mEnvCubemap;
	GLuint mIrradianceMap;
	GLuint mPrefilterMap;
	GLuint mBrdfLUTTexture;
	glm::mat4 mCaptureProjection;

	void renderEnvCubemap(Shader shader, Model *pModel);
	void renderIrrCubemap(Shader shader);
	void renderPreFilterCubemap(Shader shader);
	void renderLUT2Dtexture(Shader shader);
};

