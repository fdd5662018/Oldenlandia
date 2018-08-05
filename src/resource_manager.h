/*******************************************************************
 A static singleton ResourceManager class that hosts several
 functions to load Textures and Shaders. Each loaded texture
 and/or shader is also stored for future reference by string
 handles. All functions and resources are static and no
 public constructor is defined.
******************************************************************/
#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include "utility.h"
#include"haptic.h"
#include "ClassIBLMaker.h"

class ResourceManager
{
public:
    // Loads (and generates) a shader program from file loading vertex, 
	// fragment (and geometry) shader's source code. If gShaderFile is not nullptr, 
	// it also loads a geometry shader
    static Shader LoadShader(std::string name, const GLchar *vShaderFile, const GLchar *fShaderFile, \
		const GLchar *gShaderFile = NULL);
    // Retrieves a stored sader
    static Shader GetShader(std::string name);
    // Loads (and generates) a texture from file
	static Texture2D LoadTexture(std::string name, const GLchar *file, bool gammaCorrection = false);
    // Retrieves a stored texture
    static Texture2D GetTexture(std::string name);
    // Properly de-allocates all loaded resources
	// Loads (and generates) a model from file
	static Model *LoadpModel(std::string name, const GLchar *file, bool gammaCorrection = false);
	// Retrieves a stored model
	static Model *GetpModel(std::string name);
	// Retrives the work camera pointer
	static Camera* GetCamera(glm::vec3 fPosition = glm::vec3(0.0, 0.0, 0.0));
	// make the IBL textures
	static void LoadIBLMap();
	static GLuint getIrradianceMap();
	static GLuint getPrefilterMap();
	static GLuint getBrdfLUTTexture();
	static void BindIBLMap();
	// Intialize haptic device: next step we can construct it as the camera analogous class
	static void LoadHapticDevice(env::DEV_HAPTIC_TYPE devType = HAPTIC_BOTH);
	static void getHapticPose(glm::vec3 centralPos, glm::vec3 &offsitePos, glm::vec3 &glmAngle, int &Button, env::DEV_HAPTIC_TYPE devType);
	// Properly de-allocates all loaded resources
    static void Clear();
private:
	// Private constructor, that is we do not want any actual resource manager objects. 
	// Its members and functions should be publicly available (static).
	ResourceManager() {}
	// Resource storage
	static std::map<std::string, Shader>    Shaders;
	static std::map<std::string, Texture2D> Textures;
	static std::map<std::string, Model *> pModels;
	static Camera *pCamera;
	static ClassIBLMaker *pIBLMaker;
	static Haptic *pHapticRight;
	static Haptic *pHapticLeft;
};

#endif