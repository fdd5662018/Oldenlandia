//save some geometry data globally
#pragma once

#include "texture.h"
#include <learnopengl/shader.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define CUBE 0x00
#define NDCQUAD 0x01
typedef unsigned char RENDERTYPE;

class ClassRenderer
{
public:
	// render
	static void Render(Shader &shader, RENDERTYPE type);
private:
	ClassRenderer() {}
	// Render data
	static GLuint m_cubeVAO;
	static GLuint m_NDCQuadVAO;
	// renderer
	static void RenderCube(Shader &shader);
	static void RenderNDCQuad(Shader &shader);
	// Initializes and configures the quad's buffer and vertex attributes
	static void initCubeData();
	static void initNDCQuaddata();
};
