/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#ifndef TEXTURE_H
#define TEXTURE_H

#include <iostream>
#include <glad/glad.h>
// Texture2D is able to store and configure a texture in OpenGL.
// It also hosts utility functions for easy management.
class Texture2D
{
public:
    // Holds the ID of the texture object, used for all texture operations to reference to this particlar texture
    GLuint ID;
	std::string type;
	std::string path;
    // Texture image dimensions
    GLint mWidth, mHeight, mNrComponents; // Width and height of loaded image in pixels
    // Texture Format
    GLuint mInternal_Format; // Format of texture object
    GLuint mImage_Format; // Format of loaded image
    // Texture configuration
    GLuint mWrap_S; // Wrapping mode on S axis
    GLuint mWrap_T; // Wrapping mode on T axis
    GLuint mFilter_Min; // Filtering mode if texture pixels < screen pixels
    GLuint mFilter_Max; // Filtering mode if texture pixels > screen pixels
    // Constructor (sets default texture modes)
    Texture2D();
	Texture2D(const GLchar *file, bool gammaCorrection = false, GLuint fWrap_S = GL_REPEAT, GLuint fWrap_T = GL_REPEAT, GLuint fFilter_Min = GL_LINEAR, GLuint fFilter_Max = GL_LINEAR);
	// generate
	void Generate(unsigned char* data, GLuint width = 0, GLuint height = 0);
    // Binds the texture as the current active GL_TEXTURE_2D texture object
    void Bind() const;
};

#endif