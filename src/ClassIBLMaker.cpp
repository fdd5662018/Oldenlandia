#include "ClassIBLMaker.h"
#include "ClassRenderer.h"
#include "resource_manager.h"

// constant
// ---------------------
// camera
static const glm::vec3 gCameraPos(1.77628f, 35.8592f, 12.6633f);
// light
static const glm::vec3 gLightPosOriginal(1.77628f, 35.8592f, 12.6633f);
static const glm::vec3 gLightColorOriginal(10.0f, 10.0f, 10.0f);
// material
static const glm::vec3 gScale(0.3f, 0.3f, 0.3f);
static const float gMetallic = 0.0;
static const float gRoughness = 0.1;
static const float gAo = 0.8;
// view matrix for (0,0,0) and the constant camera position
static const glm::mat4 gcCaptureViews[] =
{
	glm::lookAt(env::gCameraPos + glm::vec3(0.0f, 0.0f, 0.0f), env::gCameraPos + glm::vec3(1.0f,  0.0f,  0.0f), env::gCameraPos + glm::vec3(0.0f, -1.0f,  0.0f)),
	glm::lookAt(env::gCameraPos + glm::vec3(0.0f, 0.0f, 0.0f), env::gCameraPos + glm::vec3(-1.0f,  0.0f,  0.0f), env::gCameraPos + glm::vec3(0.0f, -1.0f,  0.0f)),
	glm::lookAt(env::gCameraPos + glm::vec3(0.0f, 0.0f, 0.0f), env::gCameraPos + glm::vec3(0.0f,  1.0f,  0.0f), env::gCameraPos + glm::vec3(0.0f,  0.0f,  1.0f)),
	glm::lookAt(env::gCameraPos + glm::vec3(0.0f, 0.0f, 0.0f), env::gCameraPos + glm::vec3(0.0f, -1.0f,  0.0f), env::gCameraPos + glm::vec3(0.0f,  0.0f, -1.0f)),
	glm::lookAt(env::gCameraPos + glm::vec3(0.0f, 0.0f, 0.0f), env::gCameraPos + glm::vec3(0.0f,  0.0f,  1.0f), env::gCameraPos + glm::vec3(0.0f, -1.0f,  0.0f)),
	glm::lookAt(env::gCameraPos + glm::vec3(0.0f, 0.0f, 0.0f), env::gCameraPos + glm::vec3(0.0f,  0.0f, -1.0f), env::gCameraPos + glm::vec3(0.0f, -1.0f,  0.0f))
};
static const glm::mat4 gcIrradianceViews[] =
{
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
};

ClassIBLMaker::ClassIBLMaker()
{
	ResourceManager::LoadShader("our", "shaders/IBLdiffuseAndSpecular/model_loading.vs", "shaders/IBLdiffuseAndSpecular/model_loading.fs");
	ResourceManager::LoadShader("irradiance", "shaders/IBLdiffuseAndSpecular/cubemap.vs", "shaders/IBLdiffuseAndSpecular/irradiance_convolution.fs");
	ResourceManager::LoadShader("prefilter", "shaders/IBLdiffuseAndSpecular/cubemap.vs", "shaders/IBLdiffuseAndSpecular/prefilter.fs");
	ResourceManager::LoadShader("brdf", "shaders/IBLdiffuseAndSpecular/brdf.vs", "shaders/IBLdiffuseAndSpecular/brdf.fs");

	ResourceManager::LoadpModel("nasalPassage", "resources/objects/biqiang/biqiang.obj");

	glGenFramebuffers(1, &mCaptureFBO);
	glGenRenderbuffers(1, &mCaptureRBO);

	// cubemap maker projection
	mCaptureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 100.0f);
}


ClassIBLMaker::~ClassIBLMaker()
{
}

void ClassIBLMaker::Make()
{
	renderEnvCubemap(ResourceManager::GetShader("our"), ResourceManager::GetpModel("nasalPassage"));
	renderIrrCubemap(ResourceManager::GetShader("irradiance"));
	renderPreFilterCubemap(ResourceManager::GetShader("prefilter"));
	renderLUT2Dtexture(ResourceManager::GetShader("brdf"));
}

GLuint ClassIBLMaker::getEnvCubemap() {
	return this->mEnvCubemap;
}
GLuint ClassIBLMaker::getIrradianceMap() {
	return this->mIrradianceMap;
}
GLuint ClassIBLMaker::getPrefilterMap() {
	return this->mPrefilterMap;
}
GLuint ClassIBLMaker::getBrdfLUTTexture() {
	return this->mBrdfLUTTexture;
}

void ClassIBLMaker::renderEnvCubemap(Shader shader, Model *pModel)
{
	// pbr: setup framebuffer
	// ----------------------
	glBindFramebuffer(GL_FRAMEBUFFER, mCaptureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, mCaptureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mCaptureRBO);

	//1. pbr: setup cubemap to render to and attach to framebuffer
	// ---------------------------------------------------------
	glGenTextures(1, &mEnvCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, mEnvCubemap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, NULL);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, mCaptureFBO);

	shader.use();
	shader.setMat4("projection", mCaptureProjection);
	glm::mat4 matrixModel;
	matrixModel = glm::scale(matrixModel, gScale);	// it's a bit too big for our scene, so scale it down
	shader.setMat4("model", matrixModel);
	shader.setVec3("lightPositions[0]", gLightPosOriginal);
	shader.setVec3("lightColors[0]", gLightColorOriginal);
	shader.setVec3("camPos", gCameraPos);
	shader.setFloat("metallic", gMetallic);
	shader.setFloat("roughness", gRoughness);
	shader.setFloat("ao", gAo);

	for (int i = 0; i < 6; i++)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mEnvCubemap, 0);

		// render
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//model render
		shader.use();
		shader.setMat4("view", gcCaptureViews[i]);

		pModel->Draw(shader);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// then let OpenGL generate mipmaps from first mip face (combatting visible dots artifact)
	glBindTexture(GL_TEXTURE_CUBE_MAP, mEnvCubemap);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

void ClassIBLMaker::renderIrrCubemap(Shader shader) {
	// 2.pbr: create an irradiance cubemap, and re - scale capture FBO to irradiance scale.
	// ---------------------------------------------------------------------------------
	glGenTextures(1, &mIrradianceMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, mIrradianceMap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//framebuffer setup
	glBindFramebuffer(GL_FRAMEBUFFER, mCaptureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, mCaptureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

	// pbr: solve diffuse integral by convolution to create an irradiance (cube)map.
	// -----------------------------------------------------------------------------
	shader.use();
	shader.setInt("environmentMap", 0);
	shader.setMat4("projection", mCaptureProjection);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, mEnvCubemap);
	glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, mCaptureFBO);
	for (unsigned int i = 0; i < 6; ++i)
	{
		shader.setMat4("view", gcIrradianceViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mIrradianceMap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ClassRenderer::Render(shader, CUBE);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ClassIBLMaker::renderPreFilterCubemap(Shader shader)
{
	// 3.pbr: create a pre-filter cubemap, and re-scale capture FBO to pre-filter scale.
	//-------------------
	glGenTextures(1, &mPrefilterMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, mPrefilterMap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // be sure to set minifcation filter to mip_linear 
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// generate mipmaps for the cubemap so OpenGL automatically allocates the required memory.
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	// pbr: run a quasi monte-carlo simulation on the environment lighting to create a prefilter (cube)map.
	// ----------------------------------------------------------------------------------------------------
	shader.use();
	shader.setInt("environmentMap", 0);
	shader.setMat4("projection", mCaptureProjection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, mEnvCubemap);

	glBindFramebuffer(GL_FRAMEBUFFER, mCaptureFBO);
	unsigned int maxMipLevels = 5;
	for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
	{
		// reisze framebuffer according to mip-level size.
		unsigned int mipWidth = 128 * std::pow(0.5, mip);
		unsigned int mipHeight = 128 * std::pow(0.5, mip);
		glBindRenderbuffer(GL_RENDERBUFFER, mCaptureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
		glViewport(0, 0, mipWidth, mipHeight);

		float roughness = (float)mip / (float)(maxMipLevels - 1);
		shader.setFloat("roughness", roughness);
		for (unsigned int i = 0; i < 6; ++i)
		{
			shader.setMat4("view", gcIrradianceViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mPrefilterMap, mip);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			ClassRenderer::Render(shader, CUBE);
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ClassIBLMaker::renderLUT2Dtexture(Shader shader)
{
	// 4.pbr: generate a 2D LUT from the BRDF equations used.
	// ----------------------------------------------------
	glGenTextures(1, &mBrdfLUTTexture);

	// pre-allocate enough memory for the LUT texture.
	glBindTexture(GL_TEXTURE_2D, mBrdfLUTTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);

	// be sure to set wrapping mode to GL_CLAMP_TO_EDGE
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// then re-configure capture framebuffer object and render screen-space quad with BRDF shader.
	glBindFramebuffer(GL_FRAMEBUFFER, mCaptureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, mCaptureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mBrdfLUTTexture, 0);

	glViewport(0, 0, 512, 512);
	shader.use();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ClassRenderer::Render(shader, NDCQUAD);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}