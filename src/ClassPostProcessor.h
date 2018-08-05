/*******************************************************************
// PostProcessor renders the game on a textured quad after which one 
// can enable specific effects.
// It is required to call BeginRender() before rendering the scene
// and EndRender() after rendering the scene for the class to work.
******************************************************************/
#ifndef POST_PROCESSOR_H
#define POST_PROCESSOR_H
// header
#include "utility.h"
// class
class PostProcessor
{
public:
    // State
    Shader PostProcessingShader;
    Texture2D Texture;
    GLuint Width, Height;
    // Options
    GLboolean isCut;
    // Constructor
    PostProcessor(Shader shader, GLuint width, GLuint height);
    // Prepares the postprocessor's framebuffer operations before rendering the game
    void BeginRender();
    // Should be called after rendering the game, so it stores all the rendered data into a texture object
    void EndRender();
    // Renders the PostProcessor texture quad (as a screen-encompassing large sprite)
    void Render(GLfloat time = 0);
private:
    // Render state
    GLuint MSFBO, FBO; // MSFBO = Multisampled FBO. FBO is regular, used for blitting MS color-buffer to texture
    GLuint RBO; // RBO is used for multisampled color buffer
    GLuint VAO;
    // Initialize quad for rendering postprocessing texture
    void initRenderData();
};

#endif